#include "ovkCScenario.h"
#include "ovkCScenarioSettingKeywordParserCallback.h"

#include "ovkTBox.hpp"
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
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

// The following is a hack, can be removed once there is a copy constructor for scenarios, boxes, etc
#include <ovp_global_defines.h>
//___________________________________________________________________//
//                                                                   //

namespace
{
	template <class T>
	struct TTestTrue
	{
		bool operator()(typename map<CIdentifier, T*>::const_iterator it) const { return true; }
	};

	struct TTestEqSourceBox
	{
		explicit TTestEqSourceBox(const CIdentifier& boxId) : m_BoxId(boxId) { }
		bool operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getSourceBoxIdentifier()==m_BoxId; }
		const CIdentifier& m_BoxId;
	};

	struct TTestEqSourceBoxOutput
	{
		TTestEqSourceBoxOutput(const CIdentifier& boxId, uint32 outputIndex) : m_BoxId(boxId), m_OutputIndex(outputIndex) { }
		bool operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getSourceBoxIdentifier()==m_BoxId && it->second->getSourceBoxOutputIndex()==m_OutputIndex; }
		const CIdentifier& m_BoxId;
		uint32 m_OutputIndex;
	};

	struct TTestEqTargetBox
	{
		explicit TTestEqTargetBox(const CIdentifier& boxId) : m_BoxId(boxId) { }
		bool operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getTargetBoxIdentifier()==m_BoxId; }
		const CIdentifier& m_BoxId;
	};

	struct TTestEqTargetBoxInput
	{
		TTestEqTargetBoxInput(const CIdentifier& boxId, uint32 inputIndex) : m_BoxId(boxId), m_InputIndex(inputIndex) { }
		bool operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getTargetBoxIdentifier()==m_BoxId && it->second->getTargetBoxInputIndex()==m_InputIndex; }
		const CIdentifier& m_BoxId;
		uint32 m_InputIndex;
	};

	template <class T, class TTest>
	CIdentifier getNextTIdentifier(
		const map<CIdentifier, T*>& elementMap,
		const CIdentifier& previousIdentifier,
		const TTest& testFunctor)
	{
		typename map<CIdentifier, T*>::const_iterator it;

		if(previousIdentifier==OV_UndefinedIdentifier)
		{
			it=elementMap.begin();
		}
		else
		{
			it=elementMap.find(previousIdentifier);
			if(it==elementMap.end())
			{
				return OV_UndefinedIdentifier;
			}
			++it;
		}

		while(it!=elementMap.end())
		{
			if(testFunctor(it))
			{
				return it->first;
			}
			++it;
		}

		return OV_UndefinedIdentifier;
	}
};

//___________________________________________________________________//
//                                                                   //

CScenario::CScenario(const IKernelContext& kernelContext, const CIdentifier& identifier)
	:TBox< IScenario > (kernelContext)
	,m_FirstMetadataIdentifier(OV_UndefinedIdentifier)
{
	// Some operations on boxes manipulate the owner scenario, for example removing inputs
	// by default we set the scenario as owning itself to avoid segfaults
	this->setOwnerScenario(this);
	this->m_oIdentifier = identifier;
}

CScenario::~CScenario(void)
{
	this->clear();
}

//___________________________________________________________________//
//                                                                   //

bool CScenario::clear(void)
{
	this->getLogManager() << LogLevel_Debug << "Clearing scenario\n";

	for (auto& box : m_Boxes)
	{
		delete box.second;
	}
	m_Boxes.clear();

	for (auto& comment : m_Comments)
	{
		delete comment.second;
	}
	m_Comments.clear();

	for (auto& metadata : m_Metadata)
	{
		delete metadata.second;
	}
	m_Metadata.clear();
	m_FirstMetadataIdentifier = OV_UndefinedIdentifier;
	m_NextMetadataIdentifier.clear();

	for (auto& link : m_Links)
	{
		delete link.second;
	}
	m_Links.clear();

	while (this->getSettingCount())
	{
		this->removeSetting(0);
	}

	while (this->getInputCount())
	{
		this->removeScenarioInput(0);
	}

	while (this->getOutputCount())
	{
		this->removeScenarioOutput(0);
	}

	this->removeAllAttributes();

	return true;
}

bool CScenario::removeScenarioInput(const uint32 inputIndex)
{
	OV_ERROR_UNLESS_KRF(
		inputIndex < this->getInputCount(),
		"Input index = [" << inputIndex << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	// Remove the link within the scenario to this input
	if (inputIndex < m_ScenarioInputLinks.size())
	{
		m_ScenarioInputLinks.erase(m_ScenarioInputLinks.begin() + inputIndex);
	}
	this->removeInput(inputIndex);

	return true;
}

bool CScenario::removeScenarioOutput(const uint32 outputIndex)
{
	OV_ERROR_UNLESS_KRF(
		outputIndex < this->getOutputCount(),
		"Output index = [" << outputIndex << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	// Remove the link within the scenario to this output
	if (outputIndex < m_ScenarioOutputLinks.size())
	{
		m_ScenarioOutputLinks.erase(m_ScenarioOutputLinks.begin() + outputIndex);
	}
	this->removeOutput(outputIndex);

	return true;
}

bool CScenario::merge(const IScenario& scenario, IScenarioMergeCallback* scenarioMergeCallback, bool mergeSettings, bool shouldPreserveIdentifiers)
{
	map<CIdentifier, CIdentifier> oldToNewIdMap;

	// Copy boxes
	{
		CIdentifier* identifierList = nullptr;
		size_t nbElems = 0;
		scenario.getBoxIdentifierList(&identifierList, &nbElems);
		for(size_t i = 0; i < nbElems; ++i)
		{
			CIdentifier boxIdentifier = identifierList[i];
			const IBox* box = scenario.getBoxDetails(boxIdentifier);
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
		size_t nbElems = 0;
		scenario.getLinkIdentifierList(&identifierList, &nbElems);
		for(size_t i = 0; i < nbElems; ++i)
		{
			CIdentifier linkIdentifier = identifierList[i];
			const ILink* link = scenario.getLinkDetails(linkIdentifier);
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
		size_t nbElems = 0;
		scenario.getMetadataIdentifierList(&identifierList, &nbElems);
		for(size_t i = 0; i < nbElems; ++i)
		{
			CIdentifier metadataIdentifier = identifierList[i];
			const IMetadata* metadata = scenario.getMetadataDetails(metadataIdentifier);
			CIdentifier newIdentifier;
			CIdentifier suggestedNewIdentifier = shouldPreserveIdentifiers ? metadataIdentifier : OV_UndefinedIdentifier;
			this->addMetadata(newIdentifier, suggestedNewIdentifier);
			IMetadata* newMetadata = this->getMetadataDetails(newIdentifier);
			newMetadata->initializeFromExistingMetadata(*metadata);
		}
		scenario.releaseIdentifierList(identifierList);
 	}

	// Copy settings if requested

	uint32 previousSettingCount = this->getSettingCount();

	if (mergeSettings)
	{
		for (uint32 settingIndex = 0; settingIndex < scenario.getSettingCount(); settingIndex++)
		{
			CIdentifier settingTypeIdentifier;
			CString settingName;
			CString defaultValue;
			CString value;
			bool isModifiable;
			scenario.getSettingType(settingIndex, settingTypeIdentifier);
			scenario.getSettingName(settingIndex, settingName);
			scenario.getSettingDefaultValue(settingIndex, defaultValue);
			scenario.getSettingValue(settingIndex, value);
			scenario.getSettingMod(settingIndex, isModifiable);

			this->addSetting(settingName, settingTypeIdentifier, defaultValue, -1, isModifiable);
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
	return getNextTIdentifier<CBox, TTestTrue<CBox> >(m_Boxes, previousIdentifier, TTestTrue<CBox>());
}

const IBox* CScenario::getBoxDetails(const CIdentifier& boxIdentifier) const
{
	auto itBox = m_Boxes.find(boxIdentifier);

	OV_ERROR_UNLESS_KRN(
		itBox != m_Boxes.end(),
		"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itBox->second;
}

bool CScenario::isBox(
	const CIdentifier& identifier) const
{
	return m_Boxes.count(identifier) == 1;
}

IBox* CScenario::getBoxDetails(
	const CIdentifier& boxIdentifier)
{
//	this->getLogManager() << LogLevel_Debug << "Getting box details from scenario\n";

	map<CIdentifier, CBox*>::const_iterator itBox;
	itBox=m_Boxes.find(boxIdentifier);

	OV_ERROR_UNLESS_KRN(
		itBox != m_Boxes.end(),
		"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itBox->second;
}

bool CScenario::addBox(CIdentifier& boxIdentifier, const CIdentifier& suggestedBoxIdentifier)
{
	boxIdentifier = getUnusedIdentifier(suggestedBoxIdentifier);
	CBox* box = new CBox(this->getKernelContext());
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
		ErrorType::ResourceNotFound
	);

	// Find all the links that are used by this box
	auto itLink = m_Links.begin();
	while (itLink != m_Links.end())
	{
		auto itLinkCurrent = itLink;
		++itLink;

		if (itLinkCurrent->second->getSourceBoxIdentifier()==boxIdentifier || itLinkCurrent->second->getTargetBoxIdentifier()==boxIdentifier)
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
	return getNextTIdentifier<CComment, TTestTrue<CComment>>(m_Comments, previousIdentifier, TTestTrue<CComment>());
}

const IComment* CScenario::getCommentDetails(const CIdentifier& commentIdentifier) const
{
	const auto itComment = m_Comments.find(commentIdentifier);

	OV_ERROR_UNLESS_KRN(
		itComment != m_Comments.end(),
		"Comment [" << commentIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

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
		ErrorType::ResourceNotFound
	);

	return itComment->second;
}

bool CScenario::addComment(CIdentifier& commentIdentifier, const CIdentifier& suggestedCommentIdentifier)
{
	commentIdentifier = getUnusedIdentifier(suggestedCommentIdentifier);
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
		ErrorType::ResourceNotFound
	);

	// Deletes the comment itself
	delete itComment->second;

	// Removes comment from the comment list
	m_Comments.erase(itComment);

	return true;
}

CIdentifier CScenario::getNextMetadataIdentifier(const CIdentifier& previousIdentifier) const
{
	if (previousIdentifier == OV_UndefinedIdentifier)
	{
		return m_FirstMetadataIdentifier;
	}

	if (m_Metadata.count(previousIdentifier) == 0)
	{
		return OV_UndefinedIdentifier;
	}

	return m_NextMetadataIdentifier.at(previousIdentifier);
}

const IMetadata* CScenario::getMetadataDetails(const CIdentifier& metadataIdentifier) const
{
	auto itMetadata = m_Metadata.find(metadataIdentifier);

	OV_ERROR_UNLESS_KRN(
		itMetadata != m_Metadata.end(),
		"Metadata [" << metadataIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itMetadata->second;
}

IMetadata* CScenario::getMetadataDetails(const CIdentifier& metadataIdentifier)
{
	auto itMetadata = m_Metadata.find(metadataIdentifier);

	OV_ERROR_UNLESS_KRN(
		itMetadata != m_Metadata.end(),
		"Metadata [" << metadataIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itMetadata->second;
}

bool CScenario::isMetadata(const CIdentifier& identifier) const
{
	return m_Metadata.count(identifier) > 0;
}

bool CScenario::addMetadata(CIdentifier& metadataIdentifier, const CIdentifier& suggestedMetadataIdentifier)
{
	metadataIdentifier = getUnusedIdentifier(suggestedMetadataIdentifier);
	CMetadata* metadata = new CMetadata(this->getKernelContext(), *this);
	metadata->setIdentifier(metadataIdentifier);

	m_NextMetadataIdentifier[metadataIdentifier] = m_FirstMetadataIdentifier;
	m_FirstMetadataIdentifier = metadataIdentifier;
	m_Metadata[metadataIdentifier] = metadata;
	return true;
}

bool CScenario::removeMetadata(const CIdentifier& metadataIdentifier)
{
	// Finds the comment according to its identifier
	auto itMetadata = m_Metadata.find(metadataIdentifier);

	OV_ERROR_UNLESS_KRF(
		itMetadata != m_Metadata.end(),
		"Comment [" << metadataIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

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
		auto previousIdentifier = std::find_if(m_NextMetadataIdentifier.begin(), m_NextMetadataIdentifier.end(), [metadataIdentifier](const std::pair<CIdentifier, CIdentifier>& v) {
			return v.second == metadataIdentifier;
		});

		OV_FATAL_UNLESS_K(
		            previousIdentifier != m_NextMetadataIdentifier.end(),
		            "Removing metadata [" << metadataIdentifier << "] which is not in the cache ",
		            ErrorType::Internal
		            );

		m_NextMetadataIdentifier[previousIdentifier->first] = m_NextMetadataIdentifier[metadataIdentifier];
		m_NextMetadataIdentifier.erase(metadataIdentifier);
	}

	return true;
}

// Links

CIdentifier CScenario::getNextLinkIdentifier(const CIdentifier& previousIdentifier) const
{
	return getNextTIdentifier<CLink, TTestTrue<CLink> >(m_Links, previousIdentifier, TTestTrue<CLink>());
}

CIdentifier CScenario::getNextLinkIdentifierFromBox(const CIdentifier& previousIdentifier, const CIdentifier& boxIdentifier) const
{
	return getNextTIdentifier<CLink, TTestEqSourceBox>(m_Links, previousIdentifier, TTestEqSourceBox(boxIdentifier));
}

CIdentifier CScenario::getNextLinkIdentifierFromBoxOutput(const CIdentifier& previousIdentifier, const CIdentifier& boxIdentifier, const uint32 outputIndex) const
{
	return getNextTIdentifier<CLink, TTestEqSourceBoxOutput>(m_Links, previousIdentifier, TTestEqSourceBoxOutput(boxIdentifier, outputIndex));
}

CIdentifier CScenario::getNextLinkIdentifierToBox(const CIdentifier& previousIdentifier, const CIdentifier& boxIdentifier) const
{
	return getNextTIdentifier<CLink, TTestEqTargetBox>(m_Links, previousIdentifier, TTestEqTargetBox(boxIdentifier));
}

CIdentifier CScenario::getNextLinkIdentifierToBoxInput(const CIdentifier& previousIdentifier, const CIdentifier& boxIdentifier, const uint32 inputIndex) const
{
	return getNextTIdentifier<CLink, TTestEqTargetBoxInput>(m_Links, previousIdentifier, TTestEqTargetBoxInput(boxIdentifier, inputIndex));
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

bool CScenario::hasIO() const
{
	return m_HasIO;
}

bool CScenario::setScenarioInputLink(const uint32 scenarioInputIndex, const CIdentifier& boxIdentifier, const uint32 boxInputIndex)
{
	OV_ERROR_UNLESS_KRF(
		scenarioInputIndex < this->getInputCount(),
		"Scenario Input index = [" << scenarioInputIndex << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRF(
		this->isBox(boxIdentifier),
		"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	OV_ERROR_UNLESS_KRF(
		boxInputIndex < this->getBoxDetails(boxIdentifier)->getInputCount(),
		"Box Input index = [" << boxInputIndex << "] is out of range (max index = [" << (this->getBoxDetails(boxIdentifier)->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	if (scenarioInputIndex >= m_ScenarioInputLinks.size())
	{
		m_ScenarioInputLinks.resize(this->getInputCount());
	}

	// Remove any existing inputs connected to the
	for (size_t inputLinkIndex = 0; inputLinkIndex < m_ScenarioInputLinks.size(); inputLinkIndex++)
	{
		CIdentifier alreadyConnectedBoxIdentifier;
		uint32 alreadyConnectedBoxInputIndex;
		this->getScenarioInputLink(static_cast<uint32>(inputLinkIndex), alreadyConnectedBoxIdentifier, alreadyConnectedBoxInputIndex);

		if (alreadyConnectedBoxIdentifier == boxIdentifier && alreadyConnectedBoxInputIndex == boxInputIndex)
		{
			this->removeScenarioInputLink(static_cast<uint32>(inputLinkIndex), alreadyConnectedBoxIdentifier, alreadyConnectedBoxInputIndex);
		}
	}

	// Remove any existing link to this input
	for (auto& kv : m_Links)
	{
		CIdentifier linkIdentifier = kv.first;
		const CLink* link = kv.second;
		if (link->getTargetBoxIdentifier() == boxIdentifier && link->getTargetBoxInputIndex() == boxInputIndex)
		{
			this->disconnect(linkIdentifier);
		}
	}

	m_ScenarioInputLinks[scenarioInputIndex] = std::make_pair(boxIdentifier, boxInputIndex);
	return true;
}

bool CScenario::setScenarioOutputLink(const uint32 scenarioOutputIndex, const CIdentifier& boxIdentifier, const uint32 boxOutputIndex)
{
	OV_ERROR_UNLESS_KRF(
		scenarioOutputIndex < this->getOutputCount(),
		"Scenario output index = [" << scenarioOutputIndex << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRF(
		this->isBox(boxIdentifier),
		"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	OV_ERROR_UNLESS_KRF(
		boxOutputIndex < this->getBoxDetails(boxIdentifier)->getOutputCount(),
		"Box output index = [" << boxOutputIndex << "] is out of range (max index = [" << (this->getBoxDetails(boxIdentifier)->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	if (scenarioOutputIndex >= m_ScenarioOutputLinks.size())
	{
		m_ScenarioOutputLinks.resize(this->getOutputCount());
	}

	// Remove any existing outputs connected to the
	for (size_t outputLinkIndex = 0; outputLinkIndex < m_ScenarioOutputLinks.size(); outputLinkIndex++)
	{
		CIdentifier alreadyConnectedBoxIdentifier;
		uint32 alreadyConnectedBoxOutputIndex;
		this->getScenarioOutputLink(static_cast<uint32>(outputLinkIndex), alreadyConnectedBoxIdentifier, alreadyConnectedBoxOutputIndex);

		if (alreadyConnectedBoxIdentifier == boxIdentifier && alreadyConnectedBoxOutputIndex == boxOutputIndex)
		{
			this->removeScenarioOutputLink(static_cast<uint32>(outputLinkIndex), alreadyConnectedBoxIdentifier, alreadyConnectedBoxOutputIndex);
		}
	}

	m_ScenarioOutputLinks[scenarioOutputIndex] = std::make_pair(boxIdentifier, boxOutputIndex);

	return true;
}

bool CScenario::getScenarioInputLink(const uint32 scenarioInputIndex, CIdentifier& boxIdentifier, uint32& boxInputIndex) const
{
	OV_ERROR_UNLESS_KRF(
		scenarioInputIndex < this->getInputCount(),
		"Scenario input index = [" << scenarioInputIndex << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	if (scenarioInputIndex >= m_ScenarioInputLinks.size())
	{
		m_ScenarioInputLinks.resize(this->getInputCount());
	}

	boxIdentifier = m_ScenarioInputLinks[scenarioInputIndex].first;
	boxInputIndex = m_ScenarioInputLinks[scenarioInputIndex].second;

	return true;
}

bool CScenario::getScenarioOutputLink(const uint32 scenarioOutputIndex, CIdentifier& boxIdentifier, uint32& boxOutputIndex) const
{
	OV_ERROR_UNLESS_KRF(
		scenarioOutputIndex < this->getOutputCount(),
		"Scenario output index = [" << scenarioOutputIndex << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	if (scenarioOutputIndex >= m_ScenarioOutputLinks.size())
	{
		m_ScenarioOutputLinks.resize(this->getOutputCount());
	}

	boxIdentifier = m_ScenarioOutputLinks[scenarioOutputIndex].first;
	boxOutputIndex = m_ScenarioOutputLinks[scenarioOutputIndex].second;

	return true;
}

// Note: In current implementation only the scenarioInputIndex is necessary as it can only be connected to one input
// but to keep things simpler we give it all the info
bool CScenario::removeScenarioInputLink(const uint32 scenarioInputIndex, const CIdentifier& boxIdentifier, const uint32 boxInputIndex)
{
	OV_ERROR_UNLESS_KRF(
		scenarioInputIndex < this->getInputCount(),
		"Scenario Input index = [" << scenarioInputIndex << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRF(
		this->isBox(boxIdentifier),
		"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	OV_ERROR_UNLESS_KRF(
		boxInputIndex < this->getBoxDetails(boxIdentifier)->getInputCount(),
		"Box Input index = [" << boxInputIndex << "] is out of range (max index = [" << (this->getBoxDetails(boxIdentifier)->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	// This should not happen either
	if (scenarioInputIndex >= m_ScenarioInputLinks.size())
	{
		m_ScenarioInputLinks.resize(this->getInputCount());
	}

	m_ScenarioInputLinks[scenarioInputIndex] = std::make_pair(OV_UndefinedIdentifier, 0);
	return true;
}

// Note: In current implementation only the scenarioOutputIndex is necessary as it can only be connected to one Output
// but to keep things simpler we give it all the info
bool CScenario::removeScenarioOutputLink(const uint32 scenarioOutputIndex, const CIdentifier& boxIdentifier, const uint32 boxOutputIndex)
{
	OV_ERROR_UNLESS_KRF(
		scenarioOutputIndex < this->getOutputCount(),
		"Scenario output index = [" << scenarioOutputIndex << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRF(
		this->isBox(boxIdentifier),
		"Box [" << boxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	OV_ERROR_UNLESS_KRF(
		boxOutputIndex < this->getBoxDetails(boxIdentifier)->getOutputCount(),
		"Box output index = [" << boxOutputIndex << "] is out of range (max index = [" << (this->getBoxDetails(boxIdentifier)->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	// This should not happen either
	if (scenarioOutputIndex >= m_ScenarioOutputLinks.size())
	{
		m_ScenarioOutputLinks.resize(this->getOutputCount());
	}

	m_ScenarioOutputLinks[scenarioOutputIndex] = std::make_pair(OV_UndefinedIdentifier, 0);
	return true;
}

const ILink* CScenario::getLinkDetails(const CIdentifier& linkIdentifier) const
{
	const auto itLink = m_Links.find(linkIdentifier);

	OV_ERROR_UNLESS_KRN(
		itLink != m_Links.end(),
		"link [" << linkIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itLink->second;
}

ILink* CScenario::getLinkDetails(const CIdentifier& linkIdentifier)
{
	const auto itLink = m_Links.find(linkIdentifier);

	OV_ERROR_UNLESS_KRN(
		itLink != m_Links.end(),
		"Link [" << linkIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itLink->second;
}

bool CScenario::connect(
	CIdentifier& linkIdentifier,
	const CIdentifier& sourceBoxIdentifier,
	const uint32 sourceBoxOutputIndex,
	const CIdentifier& targetBoxIdentifier,
	const uint32 targetBoxInputIndex,
	const CIdentifier& suggestedLinkIdentifier)
{
	const auto itBox1 = m_Boxes.find(sourceBoxIdentifier);
	const auto itBox2 = m_Boxes.find(targetBoxIdentifier);

	OV_ERROR_UNLESS_KRF(
		itBox1 != m_Boxes.end(),
		"Source Box [" << sourceBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	OV_ERROR_UNLESS_KRF(
		itBox2 != m_Boxes.end(),
		"Target Box [" << targetBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	CBox* sourceBox = itBox1->second;
	CBox* targetBox = itBox2->second;

	OV_ERROR_UNLESS_KRF(
		targetBoxInputIndex < targetBox->getInputCount(),
		"Target box input index = [" << targetBoxInputIndex << "] is out of range (max index = [" << (targetBox->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRF(
		sourceBoxOutputIndex < sourceBox->getOutputCount(),
		"Source box output index = [" << sourceBoxOutputIndex << "] is out of range (max index = [" << (sourceBox->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	// Looks for any connected link to this box input and removes it
	auto itLink = m_Links.begin();
	while (itLink!=m_Links.end())
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
	link->setIdentifier(linkIdentifier);
	link->setSource(sourceBoxIdentifier, sourceBoxOutputIndex);
	link->setTarget(targetBoxIdentifier, targetBoxInputIndex);

	m_Links[link->getIdentifier()] = link;

	return true;
}

bool CScenario::disconnect(const CIdentifier& sourceBoxIdentifier, const uint32 sourceBoxOutputIndex, const CIdentifier& targetBoxIdentifier, const uint32 targetBoxInputIndex)
{
	// Looks for any link with the same signature
	for(auto itLink = m_Links.begin(); itLink != m_Links.end(); ++itLink)
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

	OV_ERROR_KRF(
		"Link is not part of the scenario",
		ErrorType::ResourceNotFound
	);
}

bool CScenario::disconnect(const CIdentifier& linkIdentifier)
{
	// Finds the link according to its identifier
	auto itLink = m_Links.find(linkIdentifier);

	OV_ERROR_UNLESS_KRF(
		itLink != m_Links.end(),
		"Link [" << linkIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

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

		for (uint32 settingIndex = 0;  settingIndex < box.second->getSettingCount(); settingIndex++)
		{
			CString settingName = "";
			CString settingValue = "";

			box.second->getSettingName(settingIndex, settingName);
			box.second->getSettingValue(settingIndex, settingValue);

			box.second->setSettingValue(settingIndex, this->getConfigurationManager().expandOnlyKeyword("var", settingValue));
			box.second->getSettingValue(settingIndex, settingValue);

			if(box.second->hasAttribute(OVD_AttributeId_SettingOverrideFilename))
			{
				settingValue = box.second->getAttributeValue(OVD_AttributeId_SettingOverrideFilename);
				box.second->setAttributeValue(OVD_AttributeId_SettingOverrideFilename, this->getConfigurationManager().expandOnlyKeyword("var", settingValue));
			}
		}

		this->getConfigurationManager().unregisterKeywordParser("var");

	}
	return true;
}

bool CScenario::isMetabox()
{
	// A scenario with inputs and/or outputs is a metabox
	if (this->getInputCount() + this->getOutputCount() > 0)
	{
		return true;
	}

	// TODO_JL: Find a way to check for other conditions as well

	return false;
}

//___________________________________________________________________//
//                                                                   //

bool CScenario::acceptVisitor(IObjectVisitor& objectVisitor)
{
	CObjectVisitorContext objectVisitorContext(getKernelContext());

	if (!objectVisitor.processBegin(objectVisitorContext, *this))
	{
		return false;
	}

	for (auto& box : m_Boxes)
	{
		if (!box.second->acceptVisitor(objectVisitor))
		{
			return false;
		}
	}

	for (auto& comment : m_Comments)
	{
		if(!comment.second->acceptVisitor(objectVisitor))
		{
			return false;
		}
	}

	for (auto& link : m_Links)
	{
		if (!link.second->acceptVisitor(objectVisitor))
		{
			return false;
		}
	}

	if(!objectVisitor.processEnd(objectVisitorContext, *this))
	{
		return false;
	}

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getUnusedIdentifier(const CIdentifier& suggestedIdentifier) const
{
	uint64 newIdentifier = (((uint64)rand())<<32)+((uint64)rand());
	if (suggestedIdentifier != OV_UndefinedIdentifier)
	{
		newIdentifier = suggestedIdentifier.toUInteger()-1;
	}

	CIdentifier result;
	map<CIdentifier, CBox*>::const_iterator itBox;
	map<CIdentifier, CComment*>::const_iterator itComment;
	map<CIdentifier, CLink*>::const_iterator itLink;
	do
	{
		newIdentifier++;
		result = CIdentifier(newIdentifier);
		itBox = m_Boxes.find(result);
		itComment = m_Comments.find(result);
		itLink = m_Links.find(result);
	}
	while (itBox != m_Boxes.end() || itComment != m_Comments.end() || itLink != m_Links.end() || result == OV_UndefinedIdentifier);
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

			for (uint32 settingIndex = 0; settingIndex < box.second->getSettingCount(); settingIndex++)
			{
				CString settingName = "";
				CString rawSettingValue = "";
				CIdentifier typeIdentifier;

				if (box.second->hasAttribute(OVD_AttributeId_SettingOverrideFilename))
				{
					return true;
				}
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

				if (this->getTypeManager().isEnumeration(typeIdentifier))
				{
					auto enumerationEntryValue = this->getTypeManager().getEnumerationEntryValueFromName(typeIdentifier, settingValue);
					auto enumerationEntryReversedName = this->getTypeManager().getEnumerationEntryNameFromValue(typeIdentifier, enumerationEntryValue);
					// We need to compare the reversed name of the enumerations because some enumeration values actually use max int
					// which is the same value as the guard value for incorrect stimulations
					OV_ERROR_UNLESS_KRF(
					            enumerationEntryValue != OV_IncorrectStimulation || enumerationEntryReversedName == settingValue,
					            "<" << box.second->getName() << "> The following value: [" << rawSettingValue << "] expanded as [" << settingValue << "] given as setting is not a valid [" << settingTypeName << "] value.",
					            ErrorType::BadValue);
				}

				OV_ERROR_UNLESS_KRF(
					::checkSettingValue(settingValue, typeIdentifier),
					"<" << box.second->getName() << "> The following value: ["<< rawSettingValue <<"] expanded as ["<< settingValue <<"] given as setting is not a valid [" << settingTypeName << "] value.",
					ErrorType::BadValue
				);
			}

			this->getConfigurationManager().unregisterKeywordParser("var");
		}
	}
	return true;
}


//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextNeedsUpdateBoxIdentifier(const CIdentifier& previousIdentifier) const
{
	return getNextTIdentifier<CBox, TTestTrue<CBox> >(m_BoxesWhichNeedUpdate, previousIdentifier, TTestTrue<CBox>());
}

bool CScenario::hasNeedsUpdateBox()
{
	return !m_BoxesWhichNeedUpdate.empty();
}

bool CScenario::checkNeedsUpdateBox()
{
	bool result = false;
	m_BoxesWhichNeedUpdate.clear();
	for (auto box : m_Boxes)
	{
		CIdentifier boxHashCode1;
		CIdentifier boxHashCode2;
		if (box.second->getAlgorithmClassIdentifier() == OVP_ClassId_BoxAlgorithm_Metabox)
		{
			CIdentifier metaboxId;
			metaboxId.fromString(box.second->getAttributeValue(OVP_AttributeId_Metabox_Identifier));
			boxHashCode1 = getKernelContext().getMetaboxManager().getMetaboxHash(metaboxId);
		}
		else
		{
			boxHashCode1 = this->getKernelContext().getPluginManager().getPluginObjectHashValue(box.second->getAlgorithmClassIdentifier());
		}

		boxHashCode2.fromString(box.second->getAttributeValue(OV_AttributeId_Box_InitialPrototypeHashValue));

		if(!(boxHashCode1 == OV_UndefinedIdentifier || boxHashCode1 == boxHashCode2))
		{
			result = true;
			m_BoxesWhichNeedUpdate.insert(box);
		}
	}

	return result;
}

template <class T, class TTest>
void getIdentifierList(
	const map<CIdentifier, T*>& elementMap,
	const TTest& testFunctor,
	CIdentifier** identifierList,
	size_t* size)
{
	*identifierList = new OpenViBE::CIdentifier[elementMap.size()];

	size_t index = 0;
	for (auto it = elementMap.begin(); it != elementMap.end(); it++)
	{
		if (testFunctor(it))
		{
			(*identifierList)[index++] = it->first;
		}
	}
	*size = index;
}

void CScenario::getBoxIdentifierList(OpenViBE::CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CBox, TTestTrue<CBox> >(m_Boxes, TTestTrue<CBox>(), identifierList, size);
}
void CScenario::getCommentIdentifierList(OpenViBE::CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CComment, TTestTrue<CComment> >(m_Comments, TTestTrue<CComment>(), identifierList, size);
}
void CScenario::getMetadataIdentifierList(OpenViBE::CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CMetadata, TTestTrue<CMetadata> >(m_Metadata, TTestTrue<CMetadata>(), identifierList, size);
}
void CScenario::getLinkIdentifierList(OpenViBE::CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CLink, TTestTrue<CLink> >(m_Links, TTestTrue<CLink>(), identifierList, size);
}
void CScenario::getLinkIdentifierFromBoxList(const OpenViBE::CIdentifier& boxIdentifier, OpenViBE::CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CLink, TTestEqSourceBox >(m_Links, TTestEqSourceBox(boxIdentifier), identifierList, size);
}
void CScenario::getLinkIdentifierFromBoxOutputList(const OpenViBE::CIdentifier& boxIdentifier, const uint32_t outputIndex, OpenViBE::CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CLink, TTestEqSourceBoxOutput >(m_Links, TTestEqSourceBoxOutput(boxIdentifier, outputIndex), identifierList, size);
}
void CScenario::getLinkIdentifierToBoxList(const OpenViBE::CIdentifier& boxIdentifier, OpenViBE::CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CLink, TTestEqTargetBox >(m_Links, TTestEqTargetBox(boxIdentifier), identifierList, size);
}
void CScenario::getLinkIdentifierToBoxInputList(const OpenViBE::CIdentifier& boxIdentifier, const uint32_t inputIndex, OpenViBE::CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CLink, TTestEqTargetBoxInput >(m_Links, TTestEqTargetBoxInput(boxIdentifier, inputIndex), identifierList, size);
}
void CScenario::getNeedsUpdateBoxIdentifierList(OpenViBE::CIdentifier** identifierList, size_t* size) const
{
	getIdentifierList<CBox, TTestTrue<CBox> >(m_BoxesWhichNeedUpdate, TTestTrue<CBox>(), identifierList, size);
}
 
void CScenario::releaseIdentifierList(OpenViBE::CIdentifier* identifierList) const
{
	delete[] identifierList;
}
