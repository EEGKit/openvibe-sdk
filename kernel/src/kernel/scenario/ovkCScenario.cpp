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
		TTestEqSourceBoxOutput(const CIdentifier& boxId, uint32_t outputIndex) : m_BoxId(boxId), m_OutputIndex(outputIndex) { }
		bool operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getSourceBoxIdentifier() == m_BoxId && it->second->getSourceBoxOutputIndex() == m_OutputIndex; }
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
		bool operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getTargetBoxIdentifier() == m_BoxId && it->second->getTargetBoxInputIndex() == m_InputIndex; }
		const CIdentifier& m_BoxId;
		uint32_t m_InputIndex;
	};

	template <class T, class TTest>
	CIdentifier getNextTIdentifier(const map<CIdentifier, T>& elementMap, const CIdentifier& previousIdentifier, const TTest& testFunctor)
	{
		typename map<CIdentifier, T>::const_iterator it;

		if (previousIdentifier == OV_UndefinedIdentifier)
		{
			it = elementMap.begin();
		}
		else
		{
			it = elementMap.find(previousIdentifier);
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
	CIdentifier getNextTIdentifier(const map<CIdentifier, T*>& elementMap, const CIdentifier& previousIdentifier, const TTest& testFunctor)
	{
		typename map<CIdentifier, T*>::const_iterator it;

		if(previousIdentifier==OV_UndefinedIdentifier)
		{
			it=elementMap.begin();
		}
		else
		{
			it=elementMap.find(previousIdentifier);
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
	OV_ERROR_UNLESS_KRF(index < this->getInputCount(), "Input index = [" << index << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])", ErrorType::OutOfBound);

	this->removeInput(index);

	// Remove the link within the scenario to this input
	if (index < m_ScenarioInputLinks.size())
	{
		m_ScenarioInputLinks.erase(m_ScenarioInputLinks.begin() + index);
	}

	return true;
}

bool CScenario::removeScenarioOutput(const uint32_t outputIndex)
{
	OV_ERROR_UNLESS_KRF(outputIndex < this->getOutputCount(), "Output index = [" << outputIndex << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])", ErrorType::OutOfBound);

	this->removeOutput(outputIndex);

	// Remove the link within the scenario to this output
	if (outputIndex < m_ScenarioOutputLinks.size())
	{
		m_ScenarioOutputLinks.erase(m_ScenarioOutputLinks.begin() + outputIndex);
	}

	return true;
}

bool CScenario::merge(const IScenario& scenario, IScenarioMergeCallback* scenarioMergeCallback, bool mergeSettings, bool shouldPreserveIdentifiers)
{
	map<CIdentifier, CIdentifier> oldToNewIdMap;

	// Copy boxes
	{
		CIdentifier* identifierList = nullptr;
		size_t nbElems              = 0;
		scenario.getBoxIdentifierList(&identifierList, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			CIdentifier boxIdentifier = identifierList[i];
			const IBox* box           = scenario.getBoxDetails(boxIdentifier);
			CIdentifier newIdentifier;
			CIdentifier suggestedNewIdentifier = shouldPreserveIdentifiers ? box->getIdentifier() : OV_UndefinedIdentifier;
			this->addBox(newIdentifier, *box, suggestedNewIdentifier);
			oldToNewIdMap[boxIdentifier] = newIdentifier;

			if (scenarioMergeCallback)
			{
				scenarioMergeCallback->process(boxIdentifier, newIdentifier);
			}
		}
		scenario.releaseIdentifierList(identifierList);
	}

	// Copy links
	{
		CIdentifier* identifierList = nullptr;
		size_t nbElems              = 0;
		scenario.getLinkIdentifierList(&identifierList, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			CIdentifier linkIdentifier = identifierList[i];
			const ILink* link          = scenario.getLinkDetails(linkIdentifier);
			CIdentifier newIdentifier;
			this->connect(newIdentifier,
						  oldToNewIdMap[link->getSourceBoxIdentifier()],
						  link->getSourceBoxOutputIndex(),
						  oldToNewIdMap[link->getTargetBoxIdentifier()],
						  link->getTargetBoxInputIndex(),
						  OV_UndefinedIdentifier);

			if (scenarioMergeCallback)
			{
				scenarioMergeCallback->process(linkIdentifier, newIdentifier);
			}
		}
		scenario.releaseIdentifierList(identifierList);
	}
	// Copy comments

	// Copy metadata
	{
		CIdentifier* identifierList = nullptr;
		size_t nbElems              = 0;
		scenario.getMetadataIdentifierList(&identifierList, &nbElems);
		for (size_t i = 0; i < nbElems; ++i)
		{
			CIdentifier metadataIdentifier = identifierList[i];
			const IMetadata* metadata      = scenario.getMetadataDetails(metadataIdentifier);
			CIdentifier newIdentifier;
			CIdentifier suggestedNewIdentifier = shouldPreserveIdentifiers ? metadataIdentifier : OV_UndefinedIdentifier;
			this->addMetadata(newIdentifier, suggestedNewIdentifier);
			IMetadata* newMetadata = this->getMetadataDetails(newIdentifier);
			newMetadata->initializeFromExistingMetadata(*metadata);
		}
		scenario.releaseIdentifierList(identifierList);
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

CIdentifier CScenario::getNextBoxIdentifier(const CIdentifier& previousIdentifier) const
{
	return getNextTIdentifier<CBox*, TTestTrue<CBox*>>(m_Boxes, previousIdentifier, TTestTrue<CBox*>());
}

const IBox* CScenario::getBoxDetails(const CIdentifier& boxIdentifier) const
{
	auto itBox = m_Boxes.find(boxIdentifier);

	OV_ERROR_UNLESS_KRN(
		itBox != m_Boxes.end(),
		"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itBox->second;
}

bool CScenario::isBox(const CIdentifier& identifier) const
{
	return m_Boxes.count(identifier) == 1;
}

IBox* CScenario::getBoxDetails(const CIdentifier& boxIdentifier)
{
	//	this->getLogManager() << LogLevel_Debug << "Getting box details from scenario\n";

	map<CIdentifier, CBox*>::const_iterator itBox = m_Boxes.find(boxIdentifier);

	OV_ERROR_UNLESS_KRN(itBox != m_Boxes.end(), "Box [" << boxIdentifier.toString() << "] is not part of the scenario", ErrorType::ResourceNotFound);

	return itBox->second;
}

bool CScenario::addBox(CIdentifier& boxIdentifier, const CIdentifier& suggestedBoxIdentifier)
{
	boxIdentifier = getUnusedIdentifier(suggestedBoxIdentifier);
	CBox* box     = new CBox(this->getKernelContext());
	box->setOwnerScenario(this);
	box->setIdentifier(boxIdentifier);

	m_Boxes[boxIdentifier] = box;
	return true;
}

bool CScenario::addBox(CIdentifier& boxIdentifier, const IBox& box, const CIdentifier& suggestedBoxIdentifier)
{
	if (!addBox(boxIdentifier, suggestedBoxIdentifier))
	{
		// error is handled in addBox
		return false;
	}

	IBox* newBox = getBoxDetails(boxIdentifier);
	if (!newBox)
	{
		// error is handled in getBoxDetails
		return false;
	}

	return newBox->initializeFromExistingBox(box);
}

bool CScenario::addBox(CIdentifier& boxIdentifier, const CIdentifier& boxAlgorithmIdentifier, const CIdentifier& suggestedBoxIdentifier)
{
	if (!addBox(boxIdentifier, suggestedBoxIdentifier))
	{
		// error is handled in addBox
		return false;
	}

	IBox* newBox = getBoxDetails(boxIdentifier);
	if (!newBox)
	{
		// error is handled in getBoxDetails
		return false;
	}

	return newBox->initializeFromAlgorithmClassIdentifier(boxAlgorithmIdentifier);
}

bool CScenario::addBox(CIdentifier& boxIdentifier, const IBoxAlgorithmDesc& boxAlgorithmDesc, const CIdentifier& suggestedBoxIdentifier)
{
	if (!addBox(boxIdentifier, suggestedBoxIdentifier))
	{
		// error is handled in addBox
		return false;
	}

	IBox* newBox = getBoxDetails(boxIdentifier);
	if (!newBox)
	{
		// error is handled in getBoxDetails
		return false;
	}

	return dynamic_cast<CBox*>(newBox)->initializeFromBoxAlgorithmDesc(boxAlgorithmDesc);
}

bool CScenario::removeBox(const CIdentifier& boxIdentifier)
{
	// Finds the box according to its identifier
	auto itBox = m_Boxes.find(boxIdentifier);

	OV_ERROR_UNLESS_KRF(
		itBox != m_Boxes.end(),
		"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	// Find all the links that are used by this box
	auto itLink = m_Links.begin();
	while (itLink != m_Links.end())
	{
		auto itLinkCurrent = itLink;
		++itLink;

		if (itLinkCurrent->second->getSourceBoxIdentifier() == boxIdentifier || itLinkCurrent->second->getTargetBoxIdentifier() == boxIdentifier)
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

CIdentifier CScenario::getNextCommentIdentifier(const CIdentifier& previousIdentifier) const
{
	return getNextTIdentifier<CComment*, TTestTrue<CComment*>>(m_Comments, previousIdentifier, TTestTrue<CComment*>());
}

const IComment* CScenario::getCommentDetails(const CIdentifier& commentIdentifier) const
{
	const auto itComment = m_Comments.find(commentIdentifier);

	OV_ERROR_UNLESS_KRN(
		itComment != m_Comments.end(),
		"Comment [" << commentIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itComment->second;
}

bool CScenario::isComment(const CIdentifier& identifier) const
{
	const auto itComment = m_Comments.find(identifier);
	return itComment != m_Comments.end();
}

IComment* CScenario::getCommentDetails(const CIdentifier& commentIdentifier)
{
	const auto itComment = m_Comments.find(commentIdentifier);

	OV_ERROR_UNLESS_KRN(
		itComment != m_Comments.end(),
		"Comment [" << commentIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itComment->second;
}

bool CScenario::addComment(CIdentifier& commentIdentifier, const CIdentifier& suggestedCommentIdentifier)
{
	commentIdentifier    = getUnusedIdentifier(suggestedCommentIdentifier);
	CComment* newComment = new CComment(this->getKernelContext(), *this);
	newComment->setIdentifier(commentIdentifier);

	m_Comments[commentIdentifier] = newComment;
	return true;
}

bool CScenario::addComment(CIdentifier& commentIdentifier, const IComment& comment, const CIdentifier& suggestedCommentIdentifier)
{
	if (!addComment(commentIdentifier, suggestedCommentIdentifier))
	{
		// error is handled in addComment above
		return false;
	}

	IComment* newComment = getCommentDetails(commentIdentifier);
	if (!newComment)
	{
		// error is handled in getCommentDetails
		return false;
	}

	return newComment->initializeFromExistingComment(comment);
}

bool CScenario::removeComment(const CIdentifier& commentIdentifier)
{
	// Finds the comment according to its identifier
	auto itComment = m_Comments.find(commentIdentifier);

	OV_ERROR_UNLESS_KRF(
		itComment != m_Comments.end(),
		"Comment [" << commentIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	// Deletes the comment itself
	delete itComment->second;

	// Removes comment from the comment list
	m_Comments.erase(itComment);

	return true;
}

CIdentifier CScenario::getNextMetadataIdentifier(const CIdentifier& previousIdentifier) const
{
	if (previousIdentifier == OV_UndefinedIdentifier) { return m_FirstMetadataIdentifier; }

	if (m_Metadata.count(previousIdentifier) == 0) { return OV_UndefinedIdentifier; }

	return m_NextMetadataIdentifier.at(previousIdentifier);
}

const IMetadata* CScenario::getMetadataDetails(const CIdentifier& metadataIdentifier) const
{
	auto itMetadata = m_Metadata.find(metadataIdentifier);

	OV_ERROR_UNLESS_KRN(
		itMetadata != m_Metadata.end(),
		"Metadata [" << metadataIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itMetadata->second;
}

IMetadata* CScenario::getMetadataDetails(const CIdentifier& metadataIdentifier)
{
	auto itMetadata = m_Metadata.find(metadataIdentifier);

	OV_ERROR_UNLESS_KRN(
		itMetadata != m_Metadata.end(),
		"Metadata [" << metadataIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itMetadata->second;
}

bool CScenario::isMetadata(const CIdentifier& identifier) const { return m_Metadata.count(identifier) > 0; }

bool CScenario::addMetadata(CIdentifier& metadataIdentifier, const CIdentifier& suggestedMetadataIdentifier)
{
	metadataIdentifier  = getUnusedIdentifier(suggestedMetadataIdentifier);
	CMetadata* metadata = new CMetadata(this->getKernelContext(), *this);
	metadata->setIdentifier(metadataIdentifier);

	m_NextMetadataIdentifier[metadataIdentifier] = m_FirstMetadataIdentifier;
	m_FirstMetadataIdentifier                    = metadataIdentifier;
	m_Metadata[metadataIdentifier]               = metadata;
	return true;
}

bool CScenario::removeMetadata(const CIdentifier& metadataIdentifier)
{
	// Finds the comment according to its identifier
	auto itMetadata = m_Metadata.find(metadataIdentifier);

	OV_ERROR_UNLESS_KRF(
		itMetadata != m_Metadata.end(),
		"Comment [" << metadataIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	// Deletes the metadata and remove it from the cache
	delete itMetadata->second;

	m_Metadata.erase(itMetadata);

	if (metadataIdentifier == m_FirstMetadataIdentifier)
	{
		m_FirstMetadataIdentifier = m_NextMetadataIdentifier[m_FirstMetadataIdentifier];
		m_NextMetadataIdentifier.erase(metadataIdentifier);
	}
	else
	{
		auto previousIdentifier = std::find_if(m_NextMetadataIdentifier.begin(), m_NextMetadataIdentifier.end(), [metadataIdentifier](const std::pair<CIdentifier, CIdentifier>& v)
		{
			return v.second == metadataIdentifier;
		});

		OV_FATAL_UNLESS_K(previousIdentifier != m_NextMetadataIdentifier.end(), "Removing metadata [" << metadataIdentifier << "] which is not in the cache ", ErrorType::Internal);

		m_NextMetadataIdentifier[previousIdentifier->first] = m_NextMetadataIdentifier[metadataIdentifier];
		m_NextMetadataIdentifier.erase(metadataIdentifier);
	}

	return true;
}

// Links

CIdentifier CScenario::getNextLinkIdentifier(const CIdentifier& previousIdentifier) const
{
	return getNextTIdentifier<CLink*, TTestTrue<CLink*>>(m_Links, previousIdentifier, TTestTrue<CLink*>());
}

CIdentifier CScenario::getNextLinkIdentifierFromBox(const CIdentifier& previousIdentifier, const CIdentifier& boxIdentifier) const
{
	return getNextTIdentifier<CLink*, TTestEqSourceBox>(m_Links, previousIdentifier, TTestEqSourceBox(boxIdentifier));
}

CIdentifier CScenario::getNextLinkIdentifierFromBoxOutput(const CIdentifier& previousIdentifier, const CIdentifier& boxIdentifier, const uint32_t outputIndex) const
{
	return getNextTIdentifier<CLink*, TTestEqSourceBoxOutput>(m_Links, previousIdentifier, TTestEqSourceBoxOutput(boxIdentifier, outputIndex));
}

CIdentifier CScenario::getNextLinkIdentifierToBox(const CIdentifier& previousIdentifier, const CIdentifier& boxIdentifier) const
{
	return getNextTIdentifier<CLink*, TTestEqTargetBox>(m_Links, previousIdentifier, TTestEqTargetBox(boxIdentifier));
}

CIdentifier CScenario::getNextLinkIdentifierToBoxInput(const CIdentifier& previousIdentifier, const CIdentifier& boxIdentifier, const uint32_t index) const
{
	return getNextTIdentifier<CLink*, TTestEqTargetBoxInput>(m_Links, previousIdentifier, TTestEqTargetBoxInput(boxIdentifier, index));
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

bool CScenario::setScenarioInputLink(const uint32_t scenarioInputIndex, const CIdentifier& boxIdentifier, const uint32_t boxInputIndex)
{
	if (boxIdentifier != OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			scenarioInputIndex < this->getInputCount(),
			"Scenario Input index = [" << scenarioInputIndex << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
			ErrorType::OutOfBound);

		OV_ERROR_UNLESS_KRF(
			this->isBox(boxIdentifier),
			"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
			ErrorType::ResourceNotFound);

		OV_ERROR_UNLESS_KRF(
			boxInputIndex < this->getBoxDetails(boxIdentifier)->getInputCount(),
			"Box Input index = [" << boxInputIndex << "] is out of range (max index = [" << (this->getBoxDetails(boxIdentifier)->getInputCount() - 1) << "])",
			ErrorType::OutOfBound);
	}

	if (scenarioInputIndex >= m_ScenarioInputLinks.size()) { m_ScenarioInputLinks.resize(this->getInputCount()); }

	// Remove any existing inputs connected to the target
	for (size_t inputLinkIndex = 0; inputLinkIndex < m_ScenarioInputLinks.size(); inputLinkIndex++)
	{
		CIdentifier alreadyConnectedBoxIdentifier;
		uint32_t alreadyConnectedBoxInputIndex;
		this->getScenarioInputLink(uint32_t(inputLinkIndex), alreadyConnectedBoxIdentifier, alreadyConnectedBoxInputIndex);

		if (alreadyConnectedBoxIdentifier == boxIdentifier && alreadyConnectedBoxInputIndex == boxInputIndex)
		{
			this->removeScenarioInputLink(uint32_t(inputLinkIndex), alreadyConnectedBoxIdentifier, alreadyConnectedBoxInputIndex);
		}
	}

	// Remove any existing link to this input
	for (auto& kv : m_Links)
	{
		CIdentifier linkIdentifier = kv.first;
		const CLink* link          = kv.second;
		if (link->getTargetBoxIdentifier() == boxIdentifier && link->getTargetBoxInputIndex() == boxInputIndex) { this->disconnect(linkIdentifier); }
	}

	m_ScenarioInputLinks[scenarioInputIndex] = std::make_pair(boxIdentifier, boxInputIndex);
	return true;
}

bool CScenario::setScenarioInputLink(const uint32_t scenarioInputIndex, const CIdentifier& boxIdentifier, const CIdentifier& boxInputIdentifier)
{
	uint32_t boxInputIndex = OV_Value_UndefinedIndexUInt;

	if (boxIdentifier != OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			this->isBox(boxIdentifier),
			"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
			ErrorType::ResourceNotFound);
		this->getBoxDetails(boxIdentifier)->getInterfacorIndex(Input, boxInputIdentifier, boxInputIndex);
	}
	return this->setScenarioInputLink(scenarioInputIndex, boxIdentifier, boxInputIndex);
}

bool CScenario::setScenarioOutputLink(const uint32_t scenarioOutputIndex, const CIdentifier& boxIdentifier, const uint32_t boxOutputIndex)
{
	if (boxIdentifier != OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			scenarioOutputIndex < this->getOutputCount(),
			"Scenario output index = [" << scenarioOutputIndex << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
			ErrorType::OutOfBound);

		OV_ERROR_UNLESS_KRF(
			this->isBox(boxIdentifier),
			"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
			ErrorType::ResourceNotFound);

		OV_ERROR_UNLESS_KRF(
			boxOutputIndex < this->getBoxDetails(boxIdentifier)->getOutputCount(),
			"Box output index = [" << boxOutputIndex << "] is out of range (max index = [" << (this->getBoxDetails(boxIdentifier)->getOutputCount() - 1) << "])",
			ErrorType::OutOfBound);
	}

	if (scenarioOutputIndex >= m_ScenarioOutputLinks.size()) { m_ScenarioOutputLinks.resize(this->getOutputCount()); }

	// Remove any existing outputs connected to the target
	for (size_t outputLinkIndex = 0; outputLinkIndex < m_ScenarioOutputLinks.size(); outputLinkIndex++)
	{
		CIdentifier alreadyConnectedBoxIdentifier;
		uint32_t alreadyConnectedBoxOutputIndex;
		this->getScenarioOutputLink(uint32_t(outputLinkIndex), alreadyConnectedBoxIdentifier, alreadyConnectedBoxOutputIndex);

		if (alreadyConnectedBoxIdentifier == boxIdentifier && alreadyConnectedBoxOutputIndex == boxOutputIndex)
		{
			this->removeScenarioOutputLink(uint32_t(outputLinkIndex), alreadyConnectedBoxIdentifier, alreadyConnectedBoxOutputIndex);
		}
	}

	m_ScenarioOutputLinks[scenarioOutputIndex] = std::make_pair(boxIdentifier, boxOutputIndex);

	return true;
}

bool CScenario::setScenarioOutputLink(const uint32_t scenarioOutputIndex, const CIdentifier& boxIdentifier, const CIdentifier& boxOutputIdentifier)
{
	uint32_t boxOutputIndex = OV_Value_UndefinedIndexUInt;

	if (boxIdentifier != OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			this->isBox(boxIdentifier),
			"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
			ErrorType::ResourceNotFound);

		this->getBoxDetails(boxIdentifier)->getInterfacorIndex(Output, boxOutputIdentifier, boxOutputIndex);
	}
	return this->setScenarioOutputLink(scenarioOutputIndex, boxIdentifier, boxOutputIndex);
}

bool CScenario::getScenarioInputLink(const uint32_t scenarioInputIndex, CIdentifier& boxIdentifier, uint32_t& boxInputIndex) const
{
	OV_ERROR_UNLESS_KRF(
		scenarioInputIndex < this->getInputCount(),
		"Scenario input index = [" << scenarioInputIndex << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
		ErrorType::OutOfBound);

	if (scenarioInputIndex >= m_ScenarioInputLinks.size()) { m_ScenarioInputLinks.resize(this->getInputCount()); }

	boxIdentifier = m_ScenarioInputLinks[scenarioInputIndex].first;
	boxInputIndex = m_ScenarioInputLinks[scenarioInputIndex].second;

	return true;
}

bool CScenario::getScenarioInputLink(const uint32_t scenarioInputIndex, CIdentifier& boxIdentifier, CIdentifier& boxOutputIdentifier) const
{
	uint32_t boxInputIndex;
	boxOutputIdentifier = OV_UndefinedIdentifier;

	this->getScenarioInputLink(scenarioInputIndex, boxIdentifier, boxInputIndex);

	if (boxIdentifier != OV_UndefinedIdentifier)
	{
		if (m_Boxes.find(boxIdentifier) != m_Boxes.end())
		{
			this->getBoxDetails(boxIdentifier)->getInterfacorIdentifier(Input, boxInputIndex, boxOutputIdentifier);
		}
	}

	return true;
}

bool CScenario::getScenarioOutputLink(const uint32_t scenarioOutputIndex, CIdentifier& boxIdentifier, uint32_t& boxOutputIndex) const
{
	OV_ERROR_UNLESS_KRF(
		scenarioOutputIndex < this->getOutputCount(),
		"Scenario output index = [" << scenarioOutputIndex << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound);

	if (scenarioOutputIndex >= m_ScenarioOutputLinks.size()) { m_ScenarioOutputLinks.resize(this->getOutputCount()); }

	boxIdentifier  = m_ScenarioOutputLinks[scenarioOutputIndex].first;
	boxOutputIndex = m_ScenarioOutputLinks[scenarioOutputIndex].second;

	return true;
}

bool CScenario::getScenarioOutputLink(const uint32_t scenarioOutputIndex, CIdentifier& boxIdentifier, CIdentifier& boxOutputIdentifier) const
{
	uint32_t boxOutputIndex;
	boxOutputIdentifier = OV_UndefinedIdentifier;

	this->getScenarioOutputLink(scenarioOutputIndex, boxIdentifier, boxOutputIndex);

	if (boxIdentifier != OV_UndefinedIdentifier)
	{
		if (m_Boxes.find(boxIdentifier) != m_Boxes.end())
		{
			this->getBoxDetails(boxIdentifier)->getInterfacorIdentifier(Output, boxOutputIndex, boxOutputIdentifier);
		}
	}

	return true;
}

// Note: In current implementation only the scenarioInputIndex is necessary as it can only be connected to one input
// but to keep things simpler we give it all the info
bool CScenario::removeScenarioInputLink(const uint32_t scenarioInputIndex, const CIdentifier& boxIdentifier, const uint32_t boxInputIndex)
{
	if (boxIdentifier != OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			scenarioInputIndex < this->getInputCount(),
			"Scenario Input index = [" << scenarioInputIndex << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
			ErrorType::OutOfBound);

		OV_ERROR_UNLESS_KRF(
			this->isBox(boxIdentifier),
			"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
			ErrorType::ResourceNotFound);

		OV_ERROR_UNLESS_KRF(
			boxInputIndex < this->getBoxDetails(boxIdentifier)->getInputCount(),
			"Box Input index = [" << boxInputIndex << "] is out of range (max index = [" << (this->getBoxDetails(boxIdentifier)->getInputCount() - 1) << "])",
			ErrorType::OutOfBound);
	}

	// This should not happen either
	if (scenarioInputIndex >= m_ScenarioInputLinks.size()) { m_ScenarioInputLinks.resize(this->getInputCount()); }

	m_ScenarioInputLinks[scenarioInputIndex] = std::make_pair(OV_UndefinedIdentifier, 0);
	return true;
}

// Note: In current implementation only the scenarioOutputIndex is necessary as it can only be connected to one Output
// but to keep things simpler we give it all the info
bool CScenario::removeScenarioOutputLink(const uint32_t scenarioOutputIndex, const CIdentifier& boxIdentifier, const uint32_t boxOutputIndex)
{
	if (boxIdentifier != OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			scenarioOutputIndex < this->getOutputCount(),
			"Scenario output index = [" << scenarioOutputIndex << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
			ErrorType::OutOfBound);

		OV_ERROR_UNLESS_KRF(
			this->isBox(boxIdentifier),
			"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
			ErrorType::ResourceNotFound);

		OV_ERROR_UNLESS_KRF(
			boxOutputIndex < this->getBoxDetails(boxIdentifier)->getOutputCount(),
			"Box output index = [" << boxOutputIndex << "] is out of range (max index = [" << (this->getBoxDetails(boxIdentifier)->getOutputCount() - 1) << "])",
			ErrorType::OutOfBound);
	}

	// This should not happen either
	if (scenarioOutputIndex >= m_ScenarioOutputLinks.size()) { m_ScenarioOutputLinks.resize(this->getOutputCount()); }

	m_ScenarioOutputLinks[scenarioOutputIndex] = std::make_pair(OV_UndefinedIdentifier, 0);
	return true;
}

const ILink* CScenario::getLinkDetails(const CIdentifier& linkIdentifier) const
{
	const auto itLink = m_Links.find(linkIdentifier);

	OV_ERROR_UNLESS_KRN(
		itLink != m_Links.end(),
		"link [" << linkIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itLink->second;
}

ILink* CScenario::getLinkDetails(const CIdentifier& linkIdentifier)
{
	const auto itLink = m_Links.find(linkIdentifier);

	OV_ERROR_UNLESS_KRN(
		itLink != m_Links.end(),
		"Link [" << linkIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	return itLink->second;
}

bool CScenario::connect(
	CIdentifier& linkIdentifier,
	const CIdentifier& sourceBoxIdentifier,
	const uint32_t sourceBoxOutputIndex,
	const CIdentifier& targetBoxIdentifier,
	const uint32_t targetBoxInputIndex,
	const CIdentifier& suggestedLinkIdentifier)
{
	const auto itBox1 = m_Boxes.find(sourceBoxIdentifier);
	const auto itBox2 = m_Boxes.find(targetBoxIdentifier);

	OV_ERROR_UNLESS_KRF(
		itBox1 != m_Boxes.end(),
		"Source Box [" << sourceBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	OV_ERROR_UNLESS_KRF(
		itBox2 != m_Boxes.end(),
		"Target Box [" << targetBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	CBox* sourceBox = itBox1->second;
	CBox* targetBox = itBox2->second;

	OV_ERROR_UNLESS_KRF(
		targetBoxInputIndex < targetBox->getInterfacorCountIncludingDeprecated(Input),
		"Target box input index = [" << targetBoxInputIndex << "] is out of range (max index = [" << (targetBox->getInputCount() - 1) << "])",
		ErrorType::OutOfBound);

	OV_ERROR_UNLESS_KRF(
		sourceBoxOutputIndex < sourceBox->getInterfacorCountIncludingDeprecated(Output),
		"Source box output index = [" << sourceBoxOutputIndex << "] is out of range (max index = [" << (sourceBox->getOutputCount() - 1) << "])",
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
			if (link->getTargetBoxIdentifier() == targetBoxIdentifier && link->getTargetBoxInputIndex() == targetBoxInputIndex)
			{
				delete link;
				m_Links.erase(itLinkCurrent);
			}
		}
	}

	linkIdentifier = getUnusedIdentifier(suggestedLinkIdentifier);

	CLink* link = new CLink(this->getKernelContext(), *this);
	CIdentifier sourceBoxOutputIdentifier;
	CIdentifier targetBoxInputIdentifier;

	this->getSourceBoxOutputIdentifier(sourceBoxIdentifier, sourceBoxOutputIndex, sourceBoxOutputIdentifier);
	this->getTargetBoxInputIdentifier(targetBoxIdentifier, targetBoxInputIndex, targetBoxInputIdentifier);

	link->setIdentifier(linkIdentifier);
	link->setSource(sourceBoxIdentifier, sourceBoxOutputIndex, sourceBoxOutputIdentifier);
	link->setTarget(targetBoxIdentifier, targetBoxInputIndex, targetBoxInputIdentifier);

	m_Links[link->getIdentifier()] = link;

	return true;
}

bool CScenario::connect(
	CIdentifier& linkIdentifier,
	const CIdentifier& sourceBoxIdentifier,
	const CIdentifier& sourceBoxOutputIdentifier,
	const CIdentifier& targetBoxIdentifier,
	const CIdentifier& targetBoxInputIdentifier,
	const CIdentifier& suggestedLinkIdentifier)
{
	uint32_t sourceBoxOutputIndex;
	uint32_t targetBoxInputIndex;

	this->getSourceBoxOutputIndex(sourceBoxIdentifier, sourceBoxOutputIdentifier, sourceBoxOutputIndex);
	this->getTargetBoxInputIndex(targetBoxIdentifier, targetBoxInputIdentifier, targetBoxInputIndex);

	return this->connect(linkIdentifier, sourceBoxIdentifier, sourceBoxOutputIndex, targetBoxIdentifier, targetBoxInputIndex, suggestedLinkIdentifier);
}


bool CScenario::disconnect(const CIdentifier& sourceBoxIdentifier, const uint32_t sourceBoxOutputIndex, const CIdentifier& targetBoxIdentifier, const uint32_t targetBoxInputIndex)
{
	// Looks for any link with the same signature
	for (auto itLink = m_Links.begin(); itLink != m_Links.end(); ++itLink)
	{
		CLink* link = itLink->second;
		if (link)
		{
			if (link->getTargetBoxIdentifier() == targetBoxIdentifier && link->getTargetBoxInputIndex() == targetBoxInputIndex)
			{
				if (link->getSourceBoxIdentifier() == sourceBoxIdentifier && link->getSourceBoxOutputIndex() == sourceBoxOutputIndex)
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

bool CScenario::disconnect(const CIdentifier& sourceBoxIdentifier, const CIdentifier& sourceBoxOutputIdentifier, const CIdentifier& targetBoxIdentifier, const CIdentifier& targetBoxInputIdentifier)
{
	uint32_t sourceBoxOutputIndex;
	uint32_t targetBoxInputIndex;

	this->getSourceBoxOutputIndex(sourceBoxIdentifier, sourceBoxOutputIdentifier, sourceBoxOutputIndex);
	this->getTargetBoxInputIndex(targetBoxIdentifier, targetBoxInputIdentifier, targetBoxInputIndex);

	return this->disconnect(sourceBoxIdentifier, sourceBoxOutputIndex, targetBoxIdentifier, targetBoxInputIndex);
}

bool CScenario::disconnect(const CIdentifier& linkIdentifier)
{
	// Finds the link according to its identifier
	auto itLink = m_Links.find(linkIdentifier);

	OV_ERROR_UNLESS_KRF(
		itLink != m_Links.end(),
		"Link [" << linkIdentifier.toString() << "] is not part of the scenario",
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
				box.second->setAttributeValue(OVD_AttributeId_SettingOverrideFilename, this->getConfigurationManager().expandOnlyKeyword("var", settingValue, true));
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

	for (auto& box : m_Boxes)
	{
		if (!box.second->acceptVisitor(objectVisitor)) { return false; }
	}

	for (auto& comment : m_Comments)
	{
		if (!comment.second->acceptVisitor(objectVisitor)) { return false; }
	}

	for (auto& link : m_Links)
	{
		if (!link.second->acceptVisitor(objectVisitor)) { return false; }
	}

	if (!objectVisitor.processEnd(objectVisitorContext, *this)) { return false; }

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getUnusedIdentifier(const CIdentifier& suggestedIdentifier) const
{
	uint64_t newIdentifier = (((uint64_t)rand()) << 32) + ((uint64_t)rand());
	if (suggestedIdentifier != OV_UndefinedIdentifier)
	{
		newIdentifier = suggestedIdentifier.toUInteger() - 1;
	}

	CIdentifier result;
	map<CIdentifier, CBox*>::const_iterator itBox;
	map<CIdentifier, CComment*>::const_iterator itComment;
	map<CIdentifier, CLink*>::const_iterator itLink;
	do
	{
		newIdentifier++;
		result    = CIdentifier(newIdentifier);
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
				CIdentifier typeIdentifier;

				if (box.second->hasAttribute(OVD_AttributeId_SettingOverrideFilename)) { return true; }
				box.second->getSettingName(settingIndex, settingName);
				box.second->getSettingValue(settingIndex, rawSettingValue);
				box.second->getSettingType(settingIndex, typeIdentifier);

				CString settingValue = rawSettingValue;
				if (configurationManager)
				{
					settingValue = configurationManager->expand(settingValue);
				}
				else
				{
					settingValue = this->getConfigurationManager().expandOnlyKeyword("var", settingValue);
				}

				auto settingTypeName = this->getTypeManager().getTypeName(typeIdentifier);

				OV_ERROR_UNLESS_KRF(
					::checkSettingValue(settingValue, typeIdentifier, this->getTypeManager()),
					"<" << box.second->getName() << "> The following value: ["<< rawSettingValue <<"] expanded as ["<< settingValue <<"] given as setting is not a valid [" << settingTypeName << "] value.",
					ErrorType::BadValue);
			}

			this->getConfigurationManager().unregisterKeywordParser("var");
		}
	}
	return true;
}


//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextOutdatedBoxIdentifier(const CIdentifier& previousIdentifier) const
{
	return getNextTIdentifier<std::shared_ptr<CBox>, TTestTrue<std::shared_ptr<CBox>>>(m_OutdatedBoxes, previousIdentifier, TTestTrue<std::shared_ptr<CBox>>());
}

bool CScenario::hasOutdatedBox()
{
	for (auto& box : m_Boxes)
	{
		if (box.second->hasAttribute(OV_AttributeId_Box_ToBeUpdated)) return true;
	}
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
	else
	{
		boxHashCode1 = this->getKernelContext().getPluginManager().getPluginObjectHashValue(box->getAlgorithmClassIdentifier());
	}

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
		auto boxAlgorithmClassIdentifier = box.second->getAlgorithmClassIdentifier();
		if (boxAlgorithmClassIdentifier != OVP_ClassId_BoxAlgorithm_Metabox
			&& !dynamic_cast<const IBoxAlgorithmDesc*>(this->getKernelContext().getPluginManager().getPluginObjectDescCreating(boxAlgorithmClassIdentifier))) { continue; }

		// Do not attempt to update metaboxes which do not have an associated scenario
		if (boxAlgorithmClassIdentifier == OVP_ClassId_BoxAlgorithm_Metabox)
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
				toBeUpdatedBox->initializeFromAlgorithmClassIdentifierNoInit(boxAlgorithmClassIdentifier);
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
			boxUpdater.getUpdatedBox().setAlgorithmClassIdentifier(boxAlgorithmClassIdentifier);
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
void getIdentifierList(const map<CIdentifier, T>& elementMap, const TTest& testFunctor, CIdentifier** identifierList, size_t* size)
{
	*identifierList = new CIdentifier[elementMap.size()];

	size_t index = 0;
	for (auto it = elementMap.begin(); it != elementMap.end(); ++it)
	{
		if (testFunctor(it))
		{
			(*identifierList)[index++] = it->first;
		}
	}
	*size = index;
}

void CScenario::getBoxIdentifierList(CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CBox*, TTestTrue<CBox*>>(m_Boxes, TTestTrue<CBox*>(), identifierList, size);
}

void CScenario::getCommentIdentifierList(CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CComment*, TTestTrue<CComment*>>(m_Comments, TTestTrue<CComment*>(), identifierList, size);
}

void CScenario::getMetadataIdentifierList(CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CMetadata*, TTestTrue<CMetadata*>>(m_Metadata, TTestTrue<CMetadata*>(), identifierList, size);
}

void CScenario::getLinkIdentifierList(CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CLink*, TTestTrue<CLink*>>(m_Links, TTestTrue<CLink*>(), identifierList, size);
}

void CScenario::getLinkIdentifierFromBoxList(const CIdentifier& boxIdentifier, CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CLink*, TTestEqSourceBox>(m_Links, TTestEqSourceBox(boxIdentifier), identifierList, size);
}

void CScenario::getLinkIdentifierFromBoxOutputList(const CIdentifier& boxIdentifier, const uint32_t outputIndex, CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CLink*, TTestEqSourceBoxOutput>(m_Links, TTestEqSourceBoxOutput(boxIdentifier, outputIndex), identifierList, size);
}

void CScenario::getLinkIdentifierToBoxList(const CIdentifier& boxIdentifier, CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CLink*, TTestEqTargetBox>(m_Links, TTestEqTargetBox(boxIdentifier), identifierList, size);
}

void CScenario::getLinkIdentifierToBoxInputList(const CIdentifier& boxIdentifier, const uint32_t index, CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CLink*, TTestEqTargetBoxInput>(m_Links, TTestEqTargetBoxInput(boxIdentifier, index), identifierList, size);
}

void CScenario::getOutdatedBoxIdentifierList(CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<std::shared_ptr<CBox>, TTestTrue<std::shared_ptr<CBox>>>(m_OutdatedBoxes, TTestTrue<std::shared_ptr<CBox>>(), identifierList, size);
}

void CScenario::releaseIdentifierList(CIdentifier* identifierList) const { delete[] identifierList; }

bool CScenario::getSourceBoxOutputIndex(const CIdentifier& sourceBoxIdentifier, const CIdentifier& sourceBoxOutputIdentifier, uint32_t& sourceBoxOutputIndex)
{
	const auto itSourceBox = m_Boxes.find(sourceBoxIdentifier);

	OV_ERROR_UNLESS_KRF(
		itSourceBox != m_Boxes.end(),
		"Source Box [" << sourceBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	m_Boxes[sourceBoxIdentifier]->getInterfacorIndex(Output, sourceBoxOutputIdentifier, sourceBoxOutputIndex);

	return true;
}

bool CScenario::getTargetBoxInputIndex(const CIdentifier& targetBoxIdentifier, const CIdentifier& targetBoxInputIdentifier, uint32_t& targetBoxInputIndex)
{
	const auto itTargetBox = m_Boxes.find(targetBoxIdentifier);

	OV_ERROR_UNLESS_KRF(
		itTargetBox != m_Boxes.end(),
		"Target Box [" << targetBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	m_Boxes[targetBoxIdentifier]->getInterfacorIndex(Input, targetBoxInputIdentifier, targetBoxInputIndex);

	return true;
}

bool CScenario::getSourceBoxOutputIdentifier(const CIdentifier& sourceBoxIdentifier, const uint32_t& sourceBoxOutputIndex, CIdentifier& sourceBoxOutputIdentifier)
{
	const auto itSourceBox = m_Boxes.find(sourceBoxIdentifier);

	OV_ERROR_UNLESS_KRF(
		itSourceBox != m_Boxes.end(),
		"Source Box [" << sourceBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	m_Boxes[sourceBoxIdentifier]->getInterfacorIdentifier(Output, sourceBoxOutputIndex, sourceBoxOutputIdentifier);

	return true;
}

bool CScenario::getTargetBoxInputIdentifier(const CIdentifier& targetBoxIdentifier, const uint32_t& targetBoxInputIndex, CIdentifier& targetBoxInputIdentifier)
{
	const auto itTargetBox = m_Boxes.find(targetBoxIdentifier);

	OV_ERROR_UNLESS_KRF(
		itTargetBox != m_Boxes.end(),
		"Target Box [" << targetBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	m_Boxes[targetBoxIdentifier]->getInterfacorIdentifier(Input, targetBoxInputIndex, targetBoxInputIdentifier);

	return true;
}
/**
 * \brief Process to the update of the identified box.
 *		It consists in recreate the prototype of the box according to the updated reference box which is the box
 *		resulting of the add/pull requests to the kernel prototype.
 * \param boxIdentifier		the identifier of the box to be updated
 * \return   true when update has been done successfully
 * \return   false in case of failure
 */
bool CScenario::updateBox(const CIdentifier& boxIdentifier)
{
	// Check if box must be updated
	const auto itSourceBox = m_Boxes.find(boxIdentifier);
	OV_ERROR_UNLESS_KRF(
		itSourceBox != m_Boxes.end(),
		"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound);

	auto itUpdateBox = m_OutdatedBoxes.find(boxIdentifier);

	if (itUpdateBox == m_OutdatedBoxes.end())
	{
		this->checkOutdatedBoxes();
		itUpdateBox = m_OutdatedBoxes.find(boxIdentifier);
	}

	OV_ERROR_UNLESS_KRF(
		itUpdateBox != m_OutdatedBoxes.end(),
		"Box [" << boxIdentifier.toString() << "] misses an updated version",
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
		OV_ERROR_KRF(m_Boxes[boxIdentifier]->getName()
					 << " must be manually updated. Its prototype is too complex.",
					 ErrorType::NotImplemented);
	}
	OV_ERROR_UNLESS_KRF(
		itUpdateBox != m_OutdatedBoxes.end(),
		"Box [" << boxIdentifier.toString() << "] does not have to be updated",
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
			if (interfacorType == Input)
			{
				this->getLinkIdentifierToBoxInputList(boxIdentifier, index, &linkIdentifierList, &linkCount);
			}
			else if (interfacorType == Output)
			{
				this->getLinkIdentifierFromBoxOutputList(boxIdentifier, index, &linkIdentifierList, &linkCount);
			}

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

	OV_FATAL_UNLESS_K(this->removeBox(boxIdentifier),
					  "Failed to remove redundant box",
					  ErrorType::Internal);

	CIdentifier updatedBoxIdentifier;
	OV_FATAL_UNLESS_K(this->addBox(updatedBoxIdentifier, *(itUpdateBox->second.get()), boxIdentifier),
					  "Failed to add box to the scenario",
					  ErrorType::Internal);

	OV_FATAL_UNLESS_K(updatedBoxIdentifier == boxIdentifier,
					  "Updated box failed to initialize with same identifier",
					  ErrorType::Internal);

	auto updatedBox = this->getBoxDetails(boxIdentifier);

	for (const auto& attr : nonUpdatableAttributes)
	{
		if (attr.first == OV_AttributeId_Box_ToBeUpdated) { continue; }
		if (updatedBox->hasAttribute(attr.first))
		{
			updatedBox->setAttributeValue(attr.first, attr.second);
		}
		else
		{
			updatedBox->addAttribute(attr.first, attr.second);
		}
	}

	// Reconnect links
	std::map<BoxInterfacorType, std::set<uint32_t>> isInterfacorConnected;
	isInterfacorConnected[Input]  = std::set<uint32_t>();
	isInterfacorConnected[Output] = std::set<uint32_t>();
	for (auto& link : links[Input])
	{
		CIdentifier newLinkIdentifier;
		auto index = m_UpdatedBoxIOCorrespondence.at(Input).at(boxIdentifier).at(link->getTargetBoxInputIndex());
		this->connect(newLinkIdentifier,
					  link->getSourceBoxIdentifier(),
					  link->getSourceBoxOutputIndex(),
					  boxIdentifier,
					  index,
					  link->getIdentifier());
		isInterfacorConnected[Input].insert(index);
		if (link->hasAttribute(OV_AttributeId_Link_Invalid))
		{
			this->getLinkDetails(newLinkIdentifier)->setAttributeValue(OV_AttributeId_Link_Invalid, "");
		}
	}
	for (const auto& link : links[Output])
	{
		CIdentifier newLinkIdentifier;
		auto outputIndex = m_UpdatedBoxIOCorrespondence.at(Output).at(boxIdentifier).at(link->getSourceBoxOutputIndex());
		this->connect(newLinkIdentifier,
					  boxIdentifier,
					  outputIndex,
					  link->getTargetBoxIdentifier(),
					  link->getTargetBoxInputIndex(),
					  link->getIdentifier());
		isInterfacorConnected[Output].insert(outputIndex);
		if (link->hasAttribute(OV_AttributeId_Link_Invalid))
		{
			this->getLinkDetails(newLinkIdentifier)->setAttributeValue(OV_AttributeId_Link_Invalid, "");
		}
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
			if (isDeprecated && isInterfacorConnected.at(t).find(i) == isInterfacorConnected.at(t).end())
			{
				updatedBox->removeInterfacor(t, i, true);
			}
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
		if (isDeprecated && value == defaultValue)
		{
			updatedBox->removeInterfacor(Setting, settingIndex, true);
		}
	}


	bool hasDeprecatedInterfacor = false;
	for (auto t : { Setting, Input, Output })
	{
		hasDeprecatedInterfacor |= (updatedBox->getInterfacorCount(t) != updatedBox->getInterfacorCountIncludingDeprecated(t));
	}

	if (hasDeprecatedInterfacor)
	{
		OV_WARNING_K(m_Boxes[boxIdentifier]->getName()
			<< " box has not been fully updated. Deprecated Inputs, Outputs or Settings are pending.\n"
			<< " Please remove them before exporting scenario\n");
		this->getBoxDetails(boxIdentifier)->setAttributeValue(OV_AttributeId_Box_PendingDeprecatedInterfacors, "");
	}
	else
	{
		this->getLogManager() << LogLevel_Info << m_Boxes[boxIdentifier]->getName()
				<< " box has been updated successfully\n";
	}

	return true;
}

bool CScenario::removeDeprecatedInterfacorsFromBox(const CIdentifier& boxIdentifier)
{
	// Check if box must be updated
	IBox* box = getBoxDetails(boxIdentifier);
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
			if (isDeprecated)
			{
				box->removeInterfacor(interfacorType, index);
			}
		} while (index != 0);
	}

	this->getLogManager() << LogLevel_Info << m_Boxes[boxIdentifier]->getName()
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
