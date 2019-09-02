#include "ovkCScenario.h"
#include "ovkCScenarioSettingKeywordParserCallback.h"

#include "ovkTBox.hpp"
#include "ovkCBoxUpdater.h"
#include "ovkCComment.h"
#include "ovkCMetadata.h"
#include "ovkCLink.h"

#include "../ovkCObjectVisitorContext.h"
#include "../../tools/ovk_setting_checker.h"

#include <iostream>
#include <cstdlib>
#include <algorithm>

#define OV_AttributeId_Box_Disabled                         OpenViBE::CIdentifier(0x341D3912, 0x1478DE86)
#define OVD_AttributeId_SettingOverrideFilename             OpenViBE::CIdentifier(0x8D21FF41, 0xDF6AFE7E)

using namespace std;
using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

// The following is a hack, can be removed once there is a copy constructor for scenarios, boxes, etc
#include <ovp_global_defines.h>
//___________________________________________________________________//
//                                                                   //

namespace
{
	template <class T>
	struct TTestTrue
	{
		bool operator()(typename map<CIdentifier, T>::const_iterator it) const { return true; }
	};

	struct TTestEqSourceBox
	{
		explicit TTestEqSourceBox(const CIdentifier& boxId) : m_BoxId(boxId) { }
		bool operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getSourceBoxIdentifier() == m_BoxId; }
		const CIdentifier& m_BoxId;
	};

	struct TTestEqSourceBoxOutput
	{
		TTestEqSourceBoxOutput(const CIdentifier& boxId, uint32_t index) : m_BoxId(boxId), m_OutputIndex(index) { }

		bool operator()(map<CIdentifier, CLink*>::const_iterator it) const
		{
			return it->second->getSourceBoxIdentifier() == m_BoxId && it->second->getSourceBoxOutputIndex() == m_OutputIndex;
		}

		const CIdentifier& m_BoxId;
		uint32_t m_OutputIndex;
	};

	struct TTestEqTargetBox
	{
		explicit TTestEqTargetBox(const CIdentifier& boxId) : m_BoxId(boxId) { }
		bool operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getTargetBoxIdentifier() == m_BoxId; }
		const CIdentifier& m_BoxId;
	};

	struct TTestEqTargetBoxInput
	{
		TTestEqTargetBoxInput(const CIdentifier& boxId, uint32_t index) : m_BoxId(boxId), m_InputIndex(index) { }

		bool operator()(map<CIdentifier, CLink*>::const_iterator it) const
		{
			return it->second->getTargetBoxIdentifier() == m_BoxId && it->second->getTargetBoxInputIndex() == m_InputIndex;
		}

		const CIdentifier& m_BoxId;
		uint32_t m_InputIndex;
	};

	template <class T, class TTest>
	CIdentifier getNextTIdentifier(const map<CIdentifier, T>& elementMap, const CIdentifier& previousID, const TTest& testFunctor)
	{
		typename map<CIdentifier, T>::const_iterator it;

		if (previousID == OV_UndefinedIdentifier) { it = elementMap.begin(); }
		else
		{
			it = elementMap.find(previousID);
			if (it == elementMap.end()) { return OV_UndefinedIdentifier; }
			++it;
		}

		while (it != elementMap.end())
		{
			if (testFunctor(it)) { return it->first; }
			++it;
		}

		return OV_UndefinedIdentifier;
	}

	/*
	template <class T, class TTest>
	CIdentifier getNextTIdentifier(const map<CIdentifier, T*>& elementMap, const CIdentifier& previousID, const TTest& testFunctor)
	{
		typename map<CIdentifier, T*>::const_iterator it;

		if(previousID==OV_UndefinedIdentifier)
		{
			it=elementMap.begin();
		}
		else
		{
			it=elementMap.find(previousID);
			if(it==elementMap.end()) { return OV_UndefinedIdentifier; }
			++it;
		}

		while(it!=elementMap.end())
		{
			if(testFunctor(it)) { return it->first; }
			++it;
		}

		return OV_UndefinedIdentifier;
	}
	*/
} // namespace

//___________________________________________________________________//
//                                                                   //

CScenario::CScenario(const IKernelContext& kernelContext, const CIdentifier& identifier)
	: TBox<IScenario>(kernelContext)
	  , m_FirstMetadataIdentifier(OV_UndefinedIdentifier)
{
	// Some operations on boxes manipulate the owner scenario, for example removing inputs
	// by default we set the scenario as owning itself to avoid segfaults
	this->setOwnerScenario(this);
	this->m_oIdentifier = identifier;
}

CScenario::~CScenario() { this->clear(); }

//___________________________________________________________________//
//                                                                   //

bool CScenario::clear()
{
	this->getLogManager() << LogLevel_Debug << "Clearing scenario\n";

	for (auto& box : m_Boxes) { delete box.second; }
	m_Boxes.clear();

	for (auto& comment : m_Comments) { delete comment.second; }
	m_Comments.clear();

	for (auto& metadata : m_Metadata) { delete metadata.second; }
	m_Metadata.clear();
	m_FirstMetadataIdentifier = OV_UndefinedIdentifier;
	m_NextMetadataIdentifier.clear();

	for (auto& link : m_Links) { delete link.second; }
	m_Links.clear();

	m_OutdatedBoxes.clear();

	while (this->getSettingCount()) { this->removeSetting(0); }

	while (this->getInputCount()) { this->removeScenarioInput(0); }

	while (this->getOutputCount()) { this->removeScenarioOutput(0); }

	this->removeAllAttributes();

	return true;
}

bool CScenario::removeScenarioInput(const uint32_t index)
{
	OV_ERROR_UNLESS_KRF(index < this->getInputCount(), "Input index = [" << index << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
						ErrorType::OutOfBound);

	this->removeInput(index);

	// Remove the link within the scenario to this input
	if (index < m_ScenarioInputLinks.size()) { m_ScenarioInputLinks.erase(m_ScenarioInputLinks.begin() + index); }

	return true;
}

bool CScenario::removeScenarioOutput(const uint32_t index)
{
	OV_ERROR_UNLESS_KRF(index < this->getOutputCount(),
						"Output index = [" << index << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])", ErrorType::OutOfBound);

	this->removeOutput(index);

	// Remove the link within the scenario to this output
	if (index < m_ScenarioOutputLinks.size()) { m_ScenarioOutputLinks.erase(m_ScenarioOutputLinks.begin() + index); }

	return true;
}

bool CScenario::merge(const IScenario& scenario, IScenarioMergeCallback* scenarioMergeCallback, bool mergeSettings, bool shouldPreserveIDs)
{
	map<CIdentifier, CIdentifier> oldToNewIdMap;

	// Copy boxes
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		scenario.getBoxIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			CIdentifier boxID = listID[i];
			const IBox* box   = scenario.getBoxDetails(boxID);
			CIdentifier newID;
			CIdentifier suggestedNewID = shouldPreserveIDs ? box->getIdentifier() : OV_UndefinedIdentifier;
			this->addBox(newID, *box, suggestedNewID);
			oldToNewIdMap[boxID] = newID;

			if (scenarioMergeCallback) { scenarioMergeCallback->process(boxID, newID); }
		}
		scenario.releaseIdentifierList(listID);
	}

	// Copy links
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		scenario.getLinkIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			CIdentifier linkID = listID[i];
			const ILink* link  = scenario.getLinkDetails(linkID);
			CIdentifier newID;
			this->connect(newID,
						  oldToNewIdMap[link->getSourceBoxIdentifier()],
						  link->getSourceBoxOutputIndex(),
						  oldToNewIdMap[link->getTargetBoxIdentifier()],
						  link->getTargetBoxInputIndex(),
						  OV_UndefinedIdentifier);

			if (scenarioMergeCallback) { scenarioMergeCallback->process(linkID, newID); }
		}
		scenario.releaseIdentifierList(listID);
	}
	// Copy comments

	// Copy metadata
	{
		CIdentifier* listID = nullptr;
		size_t nbElems      = 0;
		scenario.getMetadataIdentifierList(&listID, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			CIdentifier metadataID    = listID[i];
			const IMetadata* metadata = scenario.getMetadataDetails(metadataID);
			CIdentifier newID;
			CIdentifier suggestedNewID = shouldPreserveIDs ? metadataID : OV_UndefinedIdentifier;
			this->addMetadata(newID, suggestedNewID);
			IMetadata* newMetadata = this->getMetadataDetails(newID);
			newMetadata->initializeFromExistingMetadata(*metadata);
		}
		scenario.releaseIdentifierList(listID);
	}

	// Copy settings if requested

	uint32_t previousSettingCount = this->getSettingCount();

	if (mergeSettings)
	{
		for (uint32_t settingIndex = 0; settingIndex < scenario.getSettingCount(); settingIndex++)
		{
			CIdentifier settingTypeIdentifier;
			CString settingName;
			CString defaultValue;
			CString value;
			bool isModifiable;
			CIdentifier l_oIdentifier;
			scenario.getSettingType(settingIndex, settingTypeIdentifier);
			scenario.getSettingName(settingIndex, settingName);
			scenario.getSettingDefaultValue(settingIndex, defaultValue);
			scenario.getSettingValue(settingIndex, value);
			scenario.getSettingMod(settingIndex, isModifiable);
			scenario.getInterfacorIdentifier(Setting, settingIndex, l_oIdentifier);

			this->addSetting(settingName, settingTypeIdentifier, defaultValue, OV_Value_UndefinedIndexUInt, isModifiable, l_oIdentifier, true);
			this->setSettingValue(previousSettingCount + settingIndex, value);
		}

		// In this case we also merge the attributes
		CIdentifier attributeIdentifier;
		while ((attributeIdentifier = scenario.getNextAttributeIdentifier(attributeIdentifier)) != OV_UndefinedIdentifier)
		{
			CString attributeValue = scenario.getAttributeValue(attributeIdentifier);
			this->addAttribute(attributeIdentifier, attributeValue);
		}
	}

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextBoxIdentifier(const CIdentifier& previousID) const
{
	return getNextTIdentifier<CBox*, TTestTrue<CBox*>>(m_Boxes, previousID, TTestTrue<CBox*>());
}

const IBox* CScenario::getBoxDetails(const CIdentifier& boxID) const
{
	auto itBox = m_Boxes.find(boxID);

	OV_ERROR_UNLESS_KRN(
		itBox != m_Boxes.end(),
		"Box [" << boxID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itBox->second;
}

bool CScenario::isBox(const CIdentifier& identifier) const { return m_Boxes.count(identifier) == 1; }

IBox* CScenario::getBoxDetails(const CIdentifier& boxID)
{
	//	this->getLogManager() << LogLevel_Debug << "Getting box details from scenario\n";

	map<CIdentifier, CBox*>::const_iterator itBox = m_Boxes.find(boxID);

	OV_ERROR_UNLESS_KRN(itBox != m_Boxes.end(), "Box [" << boxID.toString() << "] is not part of the scenario", ErrorType::ResourceNotFound);

	return itBox->second;
}

bool CScenario::addBox(CIdentifier& boxID, const CIdentifier& suggestedBoxID)
{
	boxID     = getUnusedIdentifier(suggestedBoxID);
	CBox* box = new CBox(this->getKernelContext());
	box->setOwnerScenario(this);
	box->setIdentifier(boxID);

	m_Boxes[boxID] = box;
	return true;
}

bool CScenario::addBox(CIdentifier& boxID, const IBox& box, const CIdentifier& suggestedBoxID)
{
	if (!addBox(boxID, suggestedBoxID))
	{
		// error is handled in addBox
		return false;
	}

	IBox* newBox = getBoxDetails(boxID);
	if (!newBox)
	{
		// error is handled in getBoxDetails
		return false;
	}

	return newBox->initializeFromExistingBox(box);
}

bool CScenario::addBox(CIdentifier& boxID, const CIdentifier& boxAlgorithmID, const CIdentifier& suggestedBoxID)
{
	if (!addBox(boxID, suggestedBoxID))
	{
		// error is handled in addBox
		return false;
	}

	IBox* newBox = getBoxDetails(boxID);
	if (!newBox)
	{
		// error is handled in getBoxDetails
		return false;
	}

	return newBox->initializeFromAlgorithmClassIdentifier(boxAlgorithmID);
}

bool CScenario::addBox(CIdentifier& boxID, const IBoxAlgorithmDesc& boxAlgorithmDesc, const CIdentifier& suggestedBoxID)
{
	if (!addBox(boxID, suggestedBoxID))
	{
		// error is handled in addBox
		return false;
	}

	IBox* newBox = getBoxDetails(boxID);
	if (!newBox)
	{
		// error is handled in getBoxDetails
		return false;
	}

	return dynamic_cast<CBox*>(newBox)->initializeFromBoxAlgorithmDesc(boxAlgorithmDesc);
}

bool CScenario::removeBox(const CIdentifier& boxID)
{
	// Finds the box according to its identifier
	auto itBox = m_Boxes.find(boxID);

	OV_ERROR_UNLESS_KRF(
		itBox != m_Boxes.end(),
		"Box [" << boxID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	// Find all the links that are used by this box
	auto itLink = m_Links.begin();
	while (itLink != m_Links.end())
	{
		auto itLinkCurrent = itLink;
		++itLink;

		if (itLinkCurrent->second->getSourceBoxIdentifier() == boxID || itLinkCurrent->second->getTargetBoxIdentifier() == boxID)
		{
			// Deletes this link
			delete itLinkCurrent->second;

			// Removes link from the link list
			m_Links.erase(itLinkCurrent);
		}
	}

	// Deletes the box itself
	delete itBox->second;

	// Removes box from the box list
	m_Boxes.erase(itBox);

	return true;
}

//___________________________________________________________________//

CIdentifier CScenario::getNextCommentIdentifier(const CIdentifier& previousID) const
{
	return getNextTIdentifier<CComment*, TTestTrue<CComment*>>(m_Comments, previousID, TTestTrue<CComment*>());
}

const IComment* CScenario::getCommentDetails(const CIdentifier& commentID) const
{
	const auto itComment = m_Comments.find(commentID);

	OV_ERROR_UNLESS_KRN(
		itComment != m_Comments.end(),
		"Comment [" << commentID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itComment->second;
}

bool CScenario::isComment(const CIdentifier& identifier) const
{
	const auto itComment = m_Comments.find(identifier);
	return itComment != m_Comments.end();
}

IComment* CScenario::getCommentDetails(const CIdentifier& commentID)
{
	const auto itComment = m_Comments.find(commentID);

	OV_ERROR_UNLESS_KRN(
		itComment != m_Comments.end(),
		"Comment [" << commentID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itComment->second;
}

bool CScenario::addComment(CIdentifier& commentID, const CIdentifier& suggestedCommentID)
{
	commentID            = getUnusedIdentifier(suggestedCommentID);
	CComment* newComment = new CComment(this->getKernelContext(), *this);
	newComment->setIdentifier(commentID);

	m_Comments[commentID] = newComment;
	return true;
}

bool CScenario::addComment(CIdentifier& commentID, const IComment& comment, const CIdentifier& suggestedCommentID)
{
	if (!addComment(commentID, suggestedCommentID))
	{
		// error is handled in addComment above
		return false;
	}

	IComment* newComment = getCommentDetails(commentID);
	if (!newComment)
	{
		// error is handled in getCommentDetails
		return false;
	}

	return newComment->initializeFromExistingComment(comment);
}

bool CScenario::removeComment(const CIdentifier& commentID)
{
	// Finds the comment according to its identifier
	auto itComment = m_Comments.find(commentID);

	OV_ERROR_UNLESS_KRF(
		itComment != m_Comments.end(),
		"Comment [" << commentID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	// Deletes the comment itself
	delete itComment->second;

	// Removes comment from the comment list
	m_Comments.erase(itComment);

	return true;
}

CIdentifier CScenario::getNextMetadataIdentifier(const CIdentifier& previousID) const
{
	if (previousID == OV_UndefinedIdentifier) { return m_FirstMetadataIdentifier; }

	if (m_Metadata.count(previousID) == 0) { return OV_UndefinedIdentifier; }

	return m_NextMetadataIdentifier.at(previousID);
}

const IMetadata* CScenario::getMetadataDetails(const CIdentifier& metadataID) const
{
	auto itMetadata = m_Metadata.find(metadataID);

	OV_ERROR_UNLESS_KRN(
		itMetadata != m_Metadata.end(),
		"Metadata [" << metadataID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itMetadata->second;
}

IMetadata* CScenario::getMetadataDetails(const CIdentifier& metadataID)
{
	auto itMetadata = m_Metadata.find(metadataID);

	OV_ERROR_UNLESS_KRN(
		itMetadata != m_Metadata.end(),
		"Metadata [" << metadataID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itMetadata->second;
}

bool CScenario::isMetadata(const CIdentifier& identifier) const { return m_Metadata.count(identifier) > 0; }

bool CScenario::addMetadata(CIdentifier& metadataID, const CIdentifier& suggestedMetadataID)
{
	metadataID          = getUnusedIdentifier(suggestedMetadataID);
	CMetadata* metadata = new CMetadata(this->getKernelContext(), *this);
	metadata->setIdentifier(metadataID);

	m_NextMetadataIdentifier[metadataID] = m_FirstMetadataIdentifier;
	m_FirstMetadataIdentifier            = metadataID;
	m_Metadata[metadataID]               = metadata;
	return true;
}

bool CScenario::removeMetadata(const CIdentifier& metadataID)
{
	// Finds the comment according to its identifier
	auto itMetadata = m_Metadata.find(metadataID);

	OV_ERROR_UNLESS_KRF(
		itMetadata != m_Metadata.end(),
		"Comment [" << metadataID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	// Deletes the metadata and remove it from the cache
	delete itMetadata->second;

	m_Metadata.erase(itMetadata);

	if (metadataID == m_FirstMetadataIdentifier)
	{
		m_FirstMetadataIdentifier = m_NextMetadataIdentifier[m_FirstMetadataIdentifier];
		m_NextMetadataIdentifier.erase(metadataID);
	}
	else
	{
		auto previousID = std::find_if(m_NextMetadataIdentifier.begin(), m_NextMetadataIdentifier.end(),
									   [metadataID](const std::pair<CIdentifier, CIdentifier>& v) { return v.second == metadataID; });

		OV_FATAL_UNLESS_K(previousID != m_NextMetadataIdentifier.end(), "Removing metadata [" << metadataID << "] which is not in the cache ",
						  ErrorType::Internal);

		m_NextMetadataIdentifier[previousID->first] = m_NextMetadataIdentifier[metadataID];
		m_NextMetadataIdentifier.erase(metadataID);
	}

	return true;
}

// Links

CIdentifier CScenario::getNextLinkIdentifier(const CIdentifier& previousID) const
{
	return getNextTIdentifier<CLink*, TTestTrue<CLink*>>(m_Links, previousID, TTestTrue<CLink*>());
}

CIdentifier CScenario::getNextLinkIdentifierFromBox(const CIdentifier& previousID, const CIdentifier& boxID) const
{
	return getNextTIdentifier<CLink*, TTestEqSourceBox>(m_Links, previousID, TTestEqSourceBox(boxID));
}

CIdentifier CScenario::getNextLinkIdentifierFromBoxOutput(const CIdentifier& previousID, const CIdentifier& boxID, const uint32_t index) const
{
	return getNextTIdentifier<CLink*, TTestEqSourceBoxOutput>(m_Links, previousID, TTestEqSourceBoxOutput(boxID, index));
}

CIdentifier CScenario::getNextLinkIdentifierToBox(const CIdentifier& previousID, const CIdentifier& boxID) const
{
	return getNextTIdentifier<CLink*, TTestEqTargetBox>(m_Links, previousID, TTestEqTargetBox(boxID));
}

CIdentifier CScenario::getNextLinkIdentifierToBoxInput(const CIdentifier& previousID, const CIdentifier& boxID, const uint32_t index) const
{
	return getNextTIdentifier<CLink*, TTestEqTargetBoxInput>(m_Links, previousID, TTestEqTargetBoxInput(boxID, index));
}

bool CScenario::isLink(const CIdentifier& identifier) const
{
	const auto itLink = m_Links.find(identifier);
	return itLink != m_Links.end();
}

bool CScenario::setHasIO(const bool bHasIO)
{
	m_HasIO = bHasIO;
	return true;
}

bool CScenario::hasIO() const { return m_HasIO; }

bool CScenario::setScenarioInputLink(const uint32_t scenarioInputIdx, const CIdentifier& boxID, const uint32_t boxInputIdx)
{
	if (boxID != OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			scenarioInputIdx < this->getInputCount(),
			"Scenario Input index = [" << scenarioInputIdx << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
			ErrorType::OutOfBound);

		OV_ERROR_UNLESS_KRF(
			this->isBox(boxID),
			"Box [" << boxID.toString() << "] is not part of the scenario",
			ErrorType::ResourceNotFound);

		OV_ERROR_UNLESS_KRF(
			boxInputIdx < this->getBoxDetails(boxID)->getInputCount(),
			"Box Input index = [" << boxInputIdx << "] is out of range (max index = [" << (this->getBoxDetails(boxID)->getInputCount() - 1) << "])",
			ErrorType::OutOfBound);
	}

	if (scenarioInputIdx >= m_ScenarioInputLinks.size()) { m_ScenarioInputLinks.resize(this->getInputCount()); }

	// Remove any existing inputs connected to the target
	for (size_t inputLinkIndex = 0; inputLinkIndex < m_ScenarioInputLinks.size(); inputLinkIndex++)
	{
		CIdentifier alreadyConnectedBoxIdentifier;
		uint32_t alreadyConnectedBoxInputIndex;
		this->getScenarioInputLink(uint32_t(inputLinkIndex), alreadyConnectedBoxIdentifier, alreadyConnectedBoxInputIndex);

		if (alreadyConnectedBoxIdentifier == boxID && alreadyConnectedBoxInputIndex == boxInputIdx)
		{
			this->removeScenarioInputLink(uint32_t(inputLinkIndex), alreadyConnectedBoxIdentifier, alreadyConnectedBoxInputIndex);
		}
	}

	// Remove any existing link to this input
	for (auto& kv : m_Links)
	{
		CIdentifier linkID = kv.first;
		const CLink* link  = kv.second;
		if (link->getTargetBoxIdentifier() == boxID && link->getTargetBoxInputIndex() == boxInputIdx) { this->disconnect(linkID); }
	}

	m_ScenarioInputLinks[scenarioInputIdx] = std::make_pair(boxID, boxInputIdx);
	return true;
}

bool CScenario::setScenarioInputLink(const uint32_t scenarioInputIdx, const CIdentifier& boxID, const CIdentifier& boxInputID)
{
	uint32_t boxInputIdx = OV_Value_UndefinedIndexUInt;

	if (boxID != OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			this->isBox(boxID),
			"Box [" << boxID.toString() << "] is not part of the scenario",
			ErrorType::ResourceNotFound);
		this->getBoxDetails(boxID)->getInterfacorIndex(Input, boxInputID, boxInputIdx);
	}
	return this->setScenarioInputLink(scenarioInputIdx, boxID, boxInputIdx);
}

bool CScenario::setScenarioOutputLink(const uint32_t scenarioOutputIdx, const CIdentifier& boxID, const uint32_t boxOutputIdx)
{
	if (boxID != OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			scenarioOutputIdx < this->getOutputCount(),
			"Scenario output index = [" << scenarioOutputIdx << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
			ErrorType::OutOfBound);

		OV_ERROR_UNLESS_KRF(
			this->isBox(boxID),
			"Box [" << boxID.toString() << "] is not part of the scenario",
			ErrorType::ResourceNotFound);

		OV_ERROR_UNLESS_KRF(
			boxOutputIdx < this->getBoxDetails(boxID)->getOutputCount(),
			"Box output index = [" << boxOutputIdx << "] is out of range (max index = [" << (this->getBoxDetails(boxID)->getOutputCount() - 1) << "])",
			ErrorType::OutOfBound);
	}

	if (scenarioOutputIdx >= m_ScenarioOutputLinks.size()) { m_ScenarioOutputLinks.resize(this->getOutputCount()); }

	// Remove any existing outputs connected to the target
	for (size_t outputLinkIndex = 0; outputLinkIndex < m_ScenarioOutputLinks.size(); outputLinkIndex++)
	{
		CIdentifier alreadyConnectedBoxIdentifier;
		uint32_t alreadyConnectedBoxOutputIndex;
		this->getScenarioOutputLink(uint32_t(outputLinkIndex), alreadyConnectedBoxIdentifier, alreadyConnectedBoxOutputIndex);

		if (alreadyConnectedBoxIdentifier == boxID && alreadyConnectedBoxOutputIndex == boxOutputIdx)
		{
			this->removeScenarioOutputLink(uint32_t(outputLinkIndex), alreadyConnectedBoxIdentifier, alreadyConnectedBoxOutputIndex);
		}
	}

	m_ScenarioOutputLinks[scenarioOutputIdx] = std::make_pair(boxID, boxOutputIdx);

	return true;
}

bool CScenario::setScenarioOutputLink(const uint32_t scenarioOutputIdx, const CIdentifier& boxID, const CIdentifier& boxOutputID)
{
	uint32_t boxOutputIdx = OV_Value_UndefinedIndexUInt;

	if (boxID != OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			this->isBox(boxID),
			"Box [" << boxID.toString() << "] is not part of the scenario",
			ErrorType::ResourceNotFound);

		this->getBoxDetails(boxID)->getInterfacorIndex(Output, boxOutputID, boxOutputIdx);
	}
	return this->setScenarioOutputLink(scenarioOutputIdx, boxID, boxOutputIdx);
}

bool CScenario::getScenarioInputLink(const uint32_t scenarioInputIdx, CIdentifier& boxID, uint32_t& boxInputIdx) const
{
	OV_ERROR_UNLESS_KRF(
		scenarioInputIdx < this->getInputCount(),
		"Scenario input index = [" << scenarioInputIdx << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
		ErrorType::OutOfBound);

	if (scenarioInputIdx >= m_ScenarioInputLinks.size()) { m_ScenarioInputLinks.resize(this->getInputCount()); }

	boxID       = m_ScenarioInputLinks[scenarioInputIdx].first;
	boxInputIdx = m_ScenarioInputLinks[scenarioInputIdx].second;

	return true;
}

bool CScenario::getScenarioInputLink(const uint32_t scenarioInputIdx, CIdentifier& boxID, CIdentifier& boxOutputID) const
{
	uint32_t boxInputIdx;
	boxOutputID = OV_UndefinedIdentifier;

	this->getScenarioInputLink(scenarioInputIdx, boxID, boxInputIdx);

	if (boxID != OV_UndefinedIdentifier)
	{
		if (m_Boxes.find(boxID) != m_Boxes.end()) { this->getBoxDetails(boxID)->getInterfacorIdentifier(Input, boxInputIdx, boxOutputID); }
	}

	return true;
}

bool CScenario::getScenarioOutputLink(const uint32_t scenarioOutputIdx, CIdentifier& boxID, uint32_t& boxOutputIdx) const
{
	OV_ERROR_UNLESS_KRF(
		scenarioOutputIdx < this->getOutputCount(),
		"Scenario output index = [" << scenarioOutputIdx << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound);

	if (scenarioOutputIdx >= m_ScenarioOutputLinks.size()) { m_ScenarioOutputLinks.resize(this->getOutputCount()); }

	boxID        = m_ScenarioOutputLinks[scenarioOutputIdx].first;
	boxOutputIdx = m_ScenarioOutputLinks[scenarioOutputIdx].second;

	return true;
}

bool CScenario::getScenarioOutputLink(const uint32_t scenarioOutputIdx, CIdentifier& boxID, CIdentifier& boxOutputID) const
{
	uint32_t boxOutputIdx;
	boxOutputID = OV_UndefinedIdentifier;

	this->getScenarioOutputLink(scenarioOutputIdx, boxID, boxOutputIdx);

	if (boxID != OV_UndefinedIdentifier)
	{
		if (m_Boxes.find(boxID) != m_Boxes.end()) { this->getBoxDetails(boxID)->getInterfacorIdentifier(Output, boxOutputIdx, boxOutputID); }
	}

	return true;
}

// Note: In current implementation only the scenarioInputIdx is necessary as it can only be connected to one input
// but to keep things simpler we give it all the info
bool CScenario::removeScenarioInputLink(const uint32_t scenarioInputIdx, const CIdentifier& boxID, const uint32_t boxInputIdx)
{
	if (boxID != OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			scenarioInputIdx < this->getInputCount(),
			"Scenario Input index = [" << scenarioInputIdx << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
			ErrorType::OutOfBound);

		OV_ERROR_UNLESS_KRF(
			this->isBox(boxID),
			"Box [" << boxID.toString() << "] is not part of the scenario",
			ErrorType::ResourceNotFound);

		OV_ERROR_UNLESS_KRF(
			boxInputIdx < this->getBoxDetails(boxID)->getInputCount(),
			"Box Input index = [" << boxInputIdx << "] is out of range (max index = [" << (this->getBoxDetails(boxID)->getInputCount() - 1) << "])",
			ErrorType::OutOfBound);
	}

	// This should not happen either
	if (scenarioInputIdx >= m_ScenarioInputLinks.size()) { m_ScenarioInputLinks.resize(this->getInputCount()); }

	m_ScenarioInputLinks[scenarioInputIdx] = std::make_pair(OV_UndefinedIdentifier, 0);
	return true;
}

// Note: In current implementation only the scenarioOutputIdx is necessary as it can only be connected to one Output
// but to keep things simpler we give it all the info
bool CScenario::removeScenarioOutputLink(const uint32_t scenarioOutputIdx, const CIdentifier& boxID, const uint32_t boxOutputIdx)
{
	if (boxID != OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			scenarioOutputIdx < this->getOutputCount(),
			"Scenario output index = [" << scenarioOutputIdx << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
			ErrorType::OutOfBound);

		OV_ERROR_UNLESS_KRF(
			this->isBox(boxID),
			"Box [" << boxID.toString() << "] is not part of the scenario",
			ErrorType::ResourceNotFound);

		OV_ERROR_UNLESS_KRF(
			boxOutputIdx < this->getBoxDetails(boxID)->getOutputCount(),
			"Box output index = [" << boxOutputIdx << "] is out of range (max index = [" << (this->getBoxDetails(boxID)->getOutputCount() - 1) << "])",
			ErrorType::OutOfBound);
	}

	// This should not happen either
	if (scenarioOutputIdx >= m_ScenarioOutputLinks.size()) { m_ScenarioOutputLinks.resize(this->getOutputCount()); }

	m_ScenarioOutputLinks[scenarioOutputIdx] = std::make_pair(OV_UndefinedIdentifier, 0);
	return true;
}

const ILink* CScenario::getLinkDetails(const CIdentifier& linkID) const
{
	const auto itLink = m_Links.find(linkID);

	OV_ERROR_UNLESS_KRN(
		itLink != m_Links.end(),
		"link [" << linkID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itLink->second;
}

ILink* CScenario::getLinkDetails(const CIdentifier& linkID)
{
	const auto itLink = m_Links.find(linkID);

	OV_ERROR_UNLESS_KRN(
		itLink != m_Links.end(),
		"Link [" << linkID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itLink->second;
}

bool CScenario::connect(CIdentifier& linkID, const CIdentifier& srcBoxID, const uint32_t srcBoxOutputIdx,
						const CIdentifier& dstBoxID, const uint32_t dstBoxInputIdx, const CIdentifier& suggestedLinkID)
{
	const auto itBox1 = m_Boxes.find(srcBoxID);
	const auto itBox2 = m_Boxes.find(dstBoxID);

	OV_ERROR_UNLESS_KRF(
		itBox1 != m_Boxes.end(),
		"Source Box [" << srcBoxID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	OV_ERROR_UNLESS_KRF(
		itBox2 != m_Boxes.end(),
		"Target Box [" << dstBoxID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	CBox* srcBox = itBox1->second;
	CBox* dstBox = itBox2->second;

	OV_ERROR_UNLESS_KRF(
		dstBoxInputIdx < dstBox->getInterfacorCountIncludingDeprecated(Input),
		"Target box input index = [" << dstBoxInputIdx << "] is out of range (max index = [" << (dstBox->getInputCount() - 1) << "])",
		ErrorType::OutOfBound);

	OV_ERROR_UNLESS_KRF(
		srcBoxOutputIdx < srcBox->getInterfacorCountIncludingDeprecated(Output),
		"Source box output index = [" << srcBoxOutputIdx << "] is out of range (max index = [" << (srcBox->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound);

	// Looks for any connected link to this box input and removes it
	auto itLink = m_Links.begin();
	while (itLink != m_Links.end())
	{
		auto itLinkCurrent = itLink;
		++itLink;

		CLink* link = itLinkCurrent->second;
		if (link)
		{
			if (link->getTargetBoxIdentifier() == dstBoxID && link->getTargetBoxInputIndex() == dstBoxInputIdx)
			{
				delete link;
				m_Links.erase(itLinkCurrent);
			}
		}
	}

	linkID = getUnusedIdentifier(suggestedLinkID);

	CLink* link = new CLink(this->getKernelContext(), *this);
	CIdentifier srcBoxOutputID;
	CIdentifier dstBoxInputID;

	this->getSourceBoxOutputIdentifier(srcBoxID, srcBoxOutputIdx, srcBoxOutputID);
	this->getTargetBoxInputIdentifier(dstBoxID, dstBoxInputIdx, dstBoxInputID);

	link->setIdentifier(linkID);
	link->setSource(srcBoxID, srcBoxOutputIdx, srcBoxOutputID);
	link->setTarget(dstBoxID, dstBoxInputIdx, dstBoxInputID);

	m_Links[link->getIdentifier()] = link;

	return true;
}

bool CScenario::connect(CIdentifier& linkID, const CIdentifier& srcBoxID, const CIdentifier& srcBoxOutputID,
						const CIdentifier& dstBoxID, const CIdentifier& dstBoxInputID, const CIdentifier& suggestedLinkID)
{
	uint32_t srcBoxOutputIdx;
	uint32_t dstBoxInputIdx;

	this->getSourceBoxOutputIndex(srcBoxID, srcBoxOutputID, srcBoxOutputIdx);
	this->getTargetBoxInputIndex(dstBoxID, dstBoxInputID, dstBoxInputIdx);

	return this->connect(linkID, srcBoxID, srcBoxOutputIdx, dstBoxID, dstBoxInputIdx, suggestedLinkID);
}


bool CScenario::disconnect(const CIdentifier& srcBoxID, uint32_t srcBoxOutputIdx, const CIdentifier& dstBoxID, const uint32_t dstBoxInputIdx)
{
	// Looks for any link with the same signature
	for (auto itLink = m_Links.begin(); itLink != m_Links.end(); ++itLink)
	{
		CLink* link = itLink->second;
		if (link)
		{
			if (link->getTargetBoxIdentifier() == dstBoxID && link->getTargetBoxInputIndex() == dstBoxInputIdx)
			{
				if (link->getSourceBoxIdentifier() == srcBoxID && link->getSourceBoxOutputIndex() == srcBoxOutputIdx)
				{
					// Found a link, so removes it
					delete link;
					m_Links.erase(itLink);

					return true;
				}
			}
		}
	}

	OV_ERROR_KRF("Link is not part of the scenario", ErrorType::ResourceNotFound);
}

bool CScenario::disconnect(const CIdentifier& srcBoxID, const CIdentifier& srcBoxOutputID, const CIdentifier& dstBoxID, const CIdentifier& dstBoxInputID)
{
	uint32_t srcBoxOutputIdx;
	uint32_t dstBoxInputIdx;

	this->getSourceBoxOutputIndex(srcBoxID, srcBoxOutputID, srcBoxOutputIdx);
	this->getTargetBoxInputIndex(dstBoxID, dstBoxInputID, dstBoxInputIdx);

	return this->disconnect(srcBoxID, srcBoxOutputIdx, dstBoxID, dstBoxInputIdx);
}

bool CScenario::disconnect(const CIdentifier& linkID)
{
	// Finds the link according to its identifier
	auto itLink = m_Links.find(linkID);

	OV_ERROR_UNLESS_KRF(
		itLink != m_Links.end(),
		"Link [" << linkID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	// Deletes the link itself
	delete itLink->second;

	// Removes link from the link list
	m_Links.erase(itLink);

	return true;
}

//___________________________________________________________________//
//                                                                   //

bool CScenario::applyLocalSettings()
{
	for (auto box : m_Boxes)
	{
		// Expand all the variables inside the newly created scenario by replacing only the $var variables
		CScenarioSettingKeywordParserCallback scenarioSettingKeywordParserCallback(*this);
		this->getConfigurationManager().registerKeywordParser("var", scenarioSettingKeywordParserCallback);

		for (uint32_t settingIndex = 0; settingIndex < box.second->getSettingCount(); settingIndex++)
		{
			CString settingName  = "";
			CString settingValue = "";

			box.second->getSettingName(settingIndex, settingName);
			box.second->getSettingValue(settingIndex, settingValue);

			box.second->setSettingValue(settingIndex, this->getConfigurationManager().expandOnlyKeyword("var", settingValue, true));
			box.second->getSettingValue(settingIndex, settingValue);

			if (box.second->hasAttribute(OVD_AttributeId_SettingOverrideFilename))
			{
				settingValue = box.second->getAttributeValue(OVD_AttributeId_SettingOverrideFilename);
				box.second->setAttributeValue(
					OVD_AttributeId_SettingOverrideFilename, this->getConfigurationManager().expandOnlyKeyword("var", settingValue, true));
			}
		}

		this->getConfigurationManager().unregisterKeywordParser("var");
	}
	return true;
}

bool CScenario::isMetabox()
{
	// A scenario with inputs and/or outputs is a metabox
	if (this->getInputCount() + this->getOutputCount() > 0) { return true; }

	// TODO_JL: Find a way to check for other conditions as well

	return false;
}

//___________________________________________________________________//
//                                                                   //

bool CScenario::acceptVisitor(IObjectVisitor& objectVisitor)
{
	CObjectVisitorContext objectVisitorContext(getKernelContext());

	if (!objectVisitor.processBegin(objectVisitorContext, *this)) { return false; }

	for (auto& box : m_Boxes) { if (!box.second->acceptVisitor(objectVisitor)) { return false; } }

	for (auto& comment : m_Comments) { if (!comment.second->acceptVisitor(objectVisitor)) { return false; } }

	for (auto& link : m_Links) { if (!link.second->acceptVisitor(objectVisitor)) { return false; } }

	if (!objectVisitor.processEnd(objectVisitorContext, *this)) { return false; }

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getUnusedIdentifier(const CIdentifier& suggestedID) const
{
	uint64_t newID = (((uint64_t)rand()) << 32) + ((uint64_t)rand());
	if (suggestedID != OV_UndefinedIdentifier) { newID = suggestedID.toUInteger() - 1; }

	CIdentifier result;
	map<CIdentifier, CBox*>::const_iterator itBox;
	map<CIdentifier, CComment*>::const_iterator itComment;
	map<CIdentifier, CLink*>::const_iterator itLink;
	do
	{
		newID++;
		result    = CIdentifier(newID);
		itBox     = m_Boxes.find(result);
		itComment = m_Comments.find(result);
		itLink    = m_Links.find(result);
	} while (itBox != m_Boxes.end() || itComment != m_Comments.end() || itLink != m_Links.end() || result == OV_UndefinedIdentifier);
	return result;
}

bool CScenario::checkSettings(IConfigurationManager* configurationManager)
{
	for (auto box : m_Boxes)
	{
		if (!box.second->hasAttribute(OV_AttributeId_Box_Disabled))
		{
			this->applyLocalSettings();
			// Expand all the variables inside the newly created scenario by replacing only the $var variables
			CScenarioSettingKeywordParserCallback scenarioSettingKeywordParserCallback(*this);
			this->getConfigurationManager().registerKeywordParser("var", scenarioSettingKeywordParserCallback);

			for (uint32_t settingIndex = 0; settingIndex < box.second->getSettingCount(); settingIndex++)
			{
				CString settingName     = "";
				CString rawSettingValue = "";
				CIdentifier typeID;

				if (box.second->hasAttribute(OVD_AttributeId_SettingOverrideFilename)) { return true; }
				box.second->getSettingName(settingIndex, settingName);
				box.second->getSettingValue(settingIndex, rawSettingValue);
				box.second->getSettingType(settingIndex, typeID);

				CString settingValue = rawSettingValue;
				if (configurationManager) { settingValue = configurationManager->expand(settingValue); }
				else { settingValue = this->getConfigurationManager().expandOnlyKeyword("var", settingValue); }

				auto settingTypeName = this->getTypeManager().getTypeName(typeID);

				OV_ERROR_UNLESS_KRF(
					::checkSettingValue(settingValue, typeID, this->getTypeManager()),
					"<" << box.second->getName() << "> The following value: ["<< rawSettingValue <<"] expanded as ["<< settingValue <<
					"] given as setting is not a valid [" << settingTypeName << "] value.",
					ErrorType::BadValue);
			}

			this->getConfigurationManager().unregisterKeywordParser("var");
		}
	}
	return true;
}


//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextOutdatedBoxIdentifier(const CIdentifier& previousID) const
{
	return getNextTIdentifier<std::shared_ptr<CBox>, TTestTrue<std::shared_ptr<CBox>>>(m_OutdatedBoxes, previousID, TTestTrue<std::shared_ptr<CBox>>());
}

bool CScenario::hasOutdatedBox()
{
	for (auto& box : m_Boxes) { if (box.second->hasAttribute(OV_AttributeId_Box_ToBeUpdated)) return true; }
	return false;
}

bool CScenario::isBoxOutdated(const CIdentifier& boxId)
{
	IBox* box = getBoxDetails(boxId);
	if (!box) { return false; }
	CIdentifier boxHashCode1;
	CIdentifier boxHashCode2;
	if (box->getAlgorithmClassIdentifier() == OVP_ClassId_BoxAlgorithm_Metabox)
	{
		CIdentifier metaboxId;
		metaboxId.fromString(box->getAttributeValue(OVP_AttributeId_Metabox_Identifier));
		boxHashCode1 = getKernelContext().getMetaboxManager().getMetaboxHash(metaboxId);
	}
	else { boxHashCode1 = this->getKernelContext().getPluginManager().getPluginObjectHashValue(box->getAlgorithmClassIdentifier()); }

	boxHashCode2.fromString(box->getAttributeValue(OV_AttributeId_Box_InitialPrototypeHashValue));

	if (!(boxHashCode1 == OV_UndefinedIdentifier || boxHashCode1 == boxHashCode2)) { return true; }

	return false;
}

bool CScenario::checkOutdatedBoxes()
{
	bool result = false;

	m_OutdatedBoxes.clear();
	m_UpdatedBoxIOCorrespondence[Input]  = std::map<CIdentifier, std::map<uint32_t, uint32_t>>();
	m_UpdatedBoxIOCorrespondence[Output] = std::map<CIdentifier, std::map<uint32_t, uint32_t>>();

	for (auto box : m_Boxes)
	{
		// Do not attempt to update boxes which do not have existing box algorithm identifiers
		auto boxAlgorithmClassID = box.second->getAlgorithmClassIdentifier();
		if (boxAlgorithmClassID != OVP_ClassId_BoxAlgorithm_Metabox
			&& !dynamic_cast<const IBoxAlgorithmDesc*>(this->getKernelContext().getPluginManager().getPluginObjectDescCreating(boxAlgorithmClassID)))
		{
			continue;
		}

		// Do not attempt to update metaboxes which do not have an associated scenario
		if (boxAlgorithmClassID == OVP_ClassId_BoxAlgorithm_Metabox)
		{
			CString metaboxIdentifier = box.second->getAttributeValue(OVP_AttributeId_Metabox_Identifier);
			if (metaboxIdentifier == CString("")) { continue; }

			CIdentifier metaboxId;
			metaboxId.fromString(metaboxIdentifier);
			CString metaboxScenarioPath(this->getKernelContext().getMetaboxManager().getMetaboxFilePath(metaboxId));

			if (metaboxScenarioPath == CString("")) { continue; }
		}

		// Box Updater instance which is in charge of create updated boxes and links
		CBoxUpdater boxUpdater(*this, box.second);

		if (!boxUpdater.initialize())
		{
			OV_WARNING_K("Could not check for update the box with id " << box.second->getIdentifier());
			continue;
		}

		// exception for boxes that could not be automatically updated
		if (boxUpdater.flaggedForManualUpdate())
		{
			if (this->isBoxOutdated(box.second->getIdentifier()))
			{
				auto toBeUpdatedBox = std::shared_ptr<CBox>(new CBox(getKernelContext()));
				toBeUpdatedBox->initializeFromAlgorithmClassIdentifierNoInit(boxAlgorithmClassID);
				m_OutdatedBoxes[box.second->getIdentifier()] = toBeUpdatedBox;
				m_Boxes[box.first]->setAttributeValue(OV_AttributeId_Box_ToBeUpdated, "");
				result = true;
			}
			continue;
		}

		// collect updated boxes
		if (boxUpdater.isUpdateRequired())
		{
			m_UpdatedBoxIOCorrespondence[Input][box.second->getIdentifier()]  = boxUpdater.getOriginalToUpdatedInterfacorCorrespondence(Input);
			m_UpdatedBoxIOCorrespondence[Output][box.second->getIdentifier()] = boxUpdater.getOriginalToUpdatedInterfacorCorrespondence(Output);
			// it is important to set box algorithm at
			// last so the box listener is never called
			boxUpdater.getUpdatedBox().setAlgorithmClassIdentifier(boxAlgorithmClassID);
			// copy requested box into a new instance managed in scenario
			auto newBox = std::shared_ptr<CBox>(new CBox(this->getKernelContext()));
			newBox->initializeFromExistingBox(boxUpdater.getUpdatedBox());
			m_OutdatedBoxes[box.second->getIdentifier()] = newBox;
			m_Boxes[box.first]->setAttributeValue(OV_AttributeId_Box_ToBeUpdated, "");
			result = true;
		}
	}

	return result;
}


template <class T, class TTest>
void getIdentifierList(const map<CIdentifier, T>& elementMap, const TTest& testFunctor, CIdentifier** listID, size_t* size)
{
	*listID = new CIdentifier[elementMap.size()];

	size_t index = 0;
	for (auto it = elementMap.begin(); it != elementMap.end(); ++it) { if (testFunctor(it)) { (*listID)[index++] = it->first; } }
	*size = index;
}

void CScenario::getBoxIdentifierList(CIdentifier** listID, size_t* size) const
{
	getIdentifierList<CBox*, TTestTrue<CBox*>>(m_Boxes, TTestTrue<CBox*>(), listID, size);
}

void CScenario::getCommentIdentifierList(CIdentifier** listID, size_t* size) const
{
	getIdentifierList<CComment*, TTestTrue<CComment*>>(m_Comments, TTestTrue<CComment*>(), listID, size);
}

void CScenario::getMetadataIdentifierList(CIdentifier** listID, size_t* size) const
{
	getIdentifierList<CMetadata*, TTestTrue<CMetadata*>>(m_Metadata, TTestTrue<CMetadata*>(), listID, size);
}

void CScenario::getLinkIdentifierList(CIdentifier** listID, size_t* size) const
{
	getIdentifierList<CLink*, TTestTrue<CLink*>>(m_Links, TTestTrue<CLink*>(), listID, size);
}

void CScenario::getLinkIdentifierFromBoxList(const CIdentifier& boxID, CIdentifier** listID, size_t* size) const
{
	getIdentifierList<CLink*, TTestEqSourceBox>(m_Links, TTestEqSourceBox(boxID), listID, size);
}

void CScenario::getLinkIdentifierFromBoxOutputList(const CIdentifier& boxID, const uint32_t index, CIdentifier** listID, size_t* size) const
{
	getIdentifierList<CLink*, TTestEqSourceBoxOutput>(m_Links, TTestEqSourceBoxOutput(boxID, index), listID, size);
}

void CScenario::getLinkIdentifierToBoxList(const CIdentifier& boxID, CIdentifier** listID, size_t* size) const
{
	getIdentifierList<CLink*, TTestEqTargetBox>(m_Links, TTestEqTargetBox(boxID), listID, size);
}

void CScenario::getLinkIdentifierToBoxInputList(const CIdentifier& boxID, const uint32_t index, CIdentifier** listID, size_t* size) const
{
	getIdentifierList<CLink*, TTestEqTargetBoxInput>(m_Links, TTestEqTargetBoxInput(boxID, index), listID, size);
}

void CScenario::getOutdatedBoxIdentifierList(CIdentifier** listID, size_t* size) const
{
	getIdentifierList<std::shared_ptr<CBox>, TTestTrue<std::shared_ptr<CBox>>>(m_OutdatedBoxes, TTestTrue<std::shared_ptr<CBox>>(), listID, size);
}

void CScenario::releaseIdentifierList(CIdentifier* listID) const { delete[] listID; }

bool CScenario::getSourceBoxOutputIndex(const CIdentifier& srcBoxID, const CIdentifier& srcBoxOutputID, uint32_t& srcBoxOutputIdx)
{
	const auto itSourceBox = m_Boxes.find(srcBoxID);

	OV_ERROR_UNLESS_KRF(itSourceBox != m_Boxes.end(), "Source Box [" << srcBoxID.toString() << "] is not part of the scenario", ErrorType::ResourceNotFound);

	m_Boxes[srcBoxID]->getInterfacorIndex(Output, srcBoxOutputID, srcBoxOutputIdx);

	return true;
}

bool CScenario::getTargetBoxInputIndex(const CIdentifier& dstBoxID, const CIdentifier& dstBoxInputID, uint32_t& dstBoxInputIdx)
{
	const auto itTargetBox = m_Boxes.find(dstBoxID);

	OV_ERROR_UNLESS_KRF(itTargetBox != m_Boxes.end(), "Target Box [" << dstBoxID.toString() << "] is not part of the scenario", ErrorType::ResourceNotFound);

	m_Boxes[dstBoxID]->getInterfacorIndex(Input, dstBoxInputID, dstBoxInputIdx);

	return true;
}

bool CScenario::getSourceBoxOutputIdentifier(const CIdentifier& srcBoxID, const uint32_t& srcBoxOutputIdx, CIdentifier& srcBoxOutputID)
{
	const auto itSourceBox = m_Boxes.find(srcBoxID);

	OV_ERROR_UNLESS_KRF(itSourceBox != m_Boxes.end(), "Source Box [" << srcBoxID.toString() << "] is not part of the scenario", ErrorType::ResourceNotFound);

	m_Boxes[srcBoxID]->getInterfacorIdentifier(Output, srcBoxOutputIdx, srcBoxOutputID);

	return true;
}

bool CScenario::getTargetBoxInputIdentifier(const CIdentifier& dstBoxID, const uint32_t& dstBoxInputIdx, CIdentifier& dstBoxInputID)
{
	const auto itTargetBox = m_Boxes.find(dstBoxID);

	OV_ERROR_UNLESS_KRF(itTargetBox != m_Boxes.end(), "Target Box [" << dstBoxID.toString() << "] is not part of the scenario", ErrorType::ResourceNotFound);

	m_Boxes[dstBoxID]->getInterfacorIdentifier(Input, dstBoxInputIdx, dstBoxInputID);

	return true;
}
/**
 * \brief Process to the update of the identified box.
 *		It consists in recreate the prototype of the box according to the updated reference box which is the box
 *		resulting of the add/pull requests to the kernel prototype.
 * \param boxID		the identifier of the box to be updated
 * \return   true when update has been done successfully
 * \return   false in case of failure
 */
bool CScenario::updateBox(const CIdentifier& boxID)
{
	// Check if box must be updated
	const auto itSourceBox = m_Boxes.find(boxID);
	OV_ERROR_UNLESS_KRF(
		itSourceBox != m_Boxes.end(),
		"Box [" << boxID.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	auto itUpdateBox = m_OutdatedBoxes.find(boxID);

	if (itUpdateBox == m_OutdatedBoxes.end())
	{
		this->checkOutdatedBoxes();
		itUpdateBox = m_OutdatedBoxes.find(boxID);
	}

	OV_ERROR_UNLESS_KRF(
		itUpdateBox != m_OutdatedBoxes.end(),
		"Box [" << boxID.toString() << "] misses an updated version",
		ErrorType::ResourceNotFound);


	if (itUpdateBox->second->hasAttribute(OV_AttributeId_Box_FlagNeedsManualUpdate)
		|| itUpdateBox->second->hasAttribute(OV_AttributeId_Box_FlagCanAddInput)
		|| itUpdateBox->second->hasAttribute(OV_AttributeId_Box_FlagCanAddOutput)
		|| itUpdateBox->second->hasAttribute(OV_AttributeId_Box_FlagCanAddSetting)
		|| itUpdateBox->second->hasAttribute(OV_AttributeId_Box_FlagCanModifyInput)
		|| itUpdateBox->second->hasAttribute(OV_AttributeId_Box_FlagCanModifyOutput)
		|| itUpdateBox->second->hasAttribute(OV_AttributeId_Box_FlagCanModifySetting)
	)
	{
		OV_ERROR_KRF(m_Boxes[boxID]->getName()
					 << " must be manually updated. Its prototype is too complex.",
					 ErrorType::NotImplemented);
	}
	OV_ERROR_UNLESS_KRF(
		itUpdateBox != m_OutdatedBoxes.end(),
		"Box [" << boxID.toString() << "] does not have to be updated",
		ErrorType::ResourceNotFound);

	// get all non-updatable attributes from the source box
	std::map<CIdentifier, CString> nonUpdatableAttributes;
	{
		CIdentifier attributeId = OV_UndefinedIdentifier;
		while ((attributeId = itSourceBox->second->getNextAttributeIdentifier(attributeId)) != OV_UndefinedIdentifier)
		{
			const auto& updatableAttrs = CBoxUpdater::updatableAttributes;
			if (std::find(updatableAttrs.cbegin(), updatableAttrs.cend(), attributeId) == updatableAttrs.cend())
			{
				nonUpdatableAttributes[attributeId] = itSourceBox->second->getAttributeValue(attributeId);
			}
		}
	}

	// gather links coming to and from the box
	std::map<BoxInterfacorType, std::vector<shared_ptr<CLink>>> links;
	for (auto interfacorType : { Input, Output })
	{
		links[interfacorType] = std::vector<shared_ptr<CLink>>();
		for (uint32_t index = 0; index < itUpdateBox->second->getInterfacorCountIncludingDeprecated(interfacorType); ++index)
		{
			CIdentifier* linkIdentifierList = nullptr;
			size_t linkCount                = 0;
			if (interfacorType == Input) { this->getLinkIdentifierToBoxInputList(boxID, index, &linkIdentifierList, &linkCount); }
			else if (interfacorType == Output) { this->getLinkIdentifierFromBoxOutputList(boxID, index, &linkIdentifierList, &linkCount); }

			for (size_t i = 0; i < linkCount; ++i)
			{
				auto link = shared_ptr<CLink>(new CLink(this->getKernelContext(), *this));
				link->InitializeFromExistingLink(*this->getLinkDetails(linkIdentifierList[i]));
				if (this->getLinkDetails(linkIdentifierList[i])->hasAttribute(OV_AttributeId_Link_Invalid))
				{
					link->addAttribute(OV_AttributeId_Link_Invalid, "");
				}
				links[interfacorType].emplace_back(link);
			}

			this->releaseIdentifierList(linkIdentifierList);
		}
	}

	OV_FATAL_UNLESS_K(this->removeBox(boxID),
					  "Failed to remove redundant box",
					  ErrorType::Internal);

	CIdentifier updatedBoxIdentifier;
	OV_FATAL_UNLESS_K(this->addBox(updatedBoxIdentifier, *(itUpdateBox->second.get()), boxID), "Failed to add box to the scenario", ErrorType::Internal);
	OV_FATAL_UNLESS_K(updatedBoxIdentifier == boxID, "Updated box failed to initialize with same identifier", ErrorType::Internal);

	auto updatedBox = this->getBoxDetails(boxID);

	for (const auto& attr : nonUpdatableAttributes)
	{
		if (attr.first == OV_AttributeId_Box_ToBeUpdated) { continue; }
		if (updatedBox->hasAttribute(attr.first)) { updatedBox->setAttributeValue(attr.first, attr.second); }
		else { updatedBox->addAttribute(attr.first, attr.second); }
	}

	// Reconnect links
	std::map<BoxInterfacorType, std::set<uint32_t>> isInterfacorConnected;
	isInterfacorConnected[Input]  = std::set<uint32_t>();
	isInterfacorConnected[Output] = std::set<uint32_t>();
	for (auto& link : links[Input])
	{
		CIdentifier newLinkIdentifier;
		auto index = m_UpdatedBoxIOCorrespondence.at(Input).at(boxID).at(link->getTargetBoxInputIndex());
		this->connect(newLinkIdentifier, link->getSourceBoxIdentifier(), link->getSourceBoxOutputIndex(), boxID, index, link->getIdentifier());
		isInterfacorConnected[Input].insert(index);
		if (link->hasAttribute(OV_AttributeId_Link_Invalid)) { this->getLinkDetails(newLinkIdentifier)->setAttributeValue(OV_AttributeId_Link_Invalid, ""); }
	}
	for (const auto& link : links[Output])
	{
		CIdentifier newLinkIdentifier;
		auto index = m_UpdatedBoxIOCorrespondence.at(Output).at(boxID).at(link->getSourceBoxOutputIndex());
		this->connect(newLinkIdentifier, boxID, index, link->getTargetBoxIdentifier(), link->getTargetBoxInputIndex(), link->getIdentifier());
		isInterfacorConnected[Output].insert(index);
		if (link->hasAttribute(OV_AttributeId_Link_Invalid)) { this->getLinkDetails(newLinkIdentifier)->setAttributeValue(OV_AttributeId_Link_Invalid, ""); }
	}

	// Cleanup the i/o that are redundant and disconnected
	for (auto t : { Input, Output })
	{
		uint32_t i = updatedBox->getInterfacorCountIncludingDeprecated(t);
		while (i > 0)
		{
			--i;
			bool isDeprecated;
			updatedBox->getInterfacorDeprecatedStatus(t, i, isDeprecated);
			if (isDeprecated && isInterfacorConnected.at(t).find(i) == isInterfacorConnected.at(t).end()) { updatedBox->removeInterfacor(t, i, true); }
		}
	}

	// Cleanup the settings that are redundant and have the same value as default
	uint32_t settingIndex = updatedBox->getInterfacorCountIncludingDeprecated(Setting);
	while (settingIndex > 0)
	{
		settingIndex--;
		bool isDeprecated;
		CString value;
		CString defaultValue;
		updatedBox->getInterfacorDeprecatedStatus(Setting, settingIndex, isDeprecated);
		updatedBox->getSettingValue(settingIndex, value);
		updatedBox->getSettingDefaultValue(settingIndex, defaultValue);
		if (isDeprecated && value == defaultValue) { updatedBox->removeInterfacor(Setting, settingIndex, true); }
	}


	bool hasDeprecatedInterfacor = false;
	for (auto t : { Setting, Input, Output })
	{
		hasDeprecatedInterfacor |= (updatedBox->getInterfacorCount(t) != updatedBox->getInterfacorCountIncludingDeprecated(t));
	}

	if (hasDeprecatedInterfacor)
	{
		OV_WARNING_K(m_Boxes[boxID]->getName()
			<< " box has not been fully updated. Deprecated Inputs, Outputs or Settings are pending.\n"
			<< " Please remove them before exporting scenario\n");
		this->getBoxDetails(boxID)->setAttributeValue(OV_AttributeId_Box_PendingDeprecatedInterfacors, "");
	}
	else
	{
		this->getLogManager() << LogLevel_Info << m_Boxes[boxID]->getName()
				<< " box has been updated successfully\n";
	}

	return true;
}

bool CScenario::removeDeprecatedInterfacorsFromBox(const CIdentifier& boxID)
{
	// Check if box must be updated
	IBox* box = getBoxDetails(boxID);
	if (!box) { return false; }

	for (auto interfacorType : { Input, Output, Setting })
	{
		auto interfacorCount = box->getInterfacorCountIncludingDeprecated(interfacorType);
		if (interfacorCount == 0) { continue; }
		auto index = interfacorCount;
		do
		{
			index--;
			bool isDeprecated = false;
			box->getInterfacorDeprecatedStatus(interfacorType, index, isDeprecated);
			if (isDeprecated) { box->removeInterfacor(interfacorType, index); }
		} while (index != 0);
	}

	this->getLogManager() << LogLevel_Info << m_Boxes[boxID]->getName()
			<< " Deprecated I/O and settings have been removed successfully\n";

	box->removeAttribute(OV_AttributeId_Box_PendingDeprecatedInterfacors);

	return true;
}

bool CScenario::containsBoxWithDeprecatedInterfacors() const
{
	for (auto box : m_Boxes)
	{
		for (const auto interfacorType : { Input, Output, Setting })
		{
			if (box.second->getInterfacorCountIncludingDeprecated(interfacorType) > box.second->getInterfacorCount(interfacorType)) { return true; }
		}
	}
	return false;
}
