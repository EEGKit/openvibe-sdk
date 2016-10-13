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
		boolean operator()(typename map<CIdentifier, T*>::const_iterator it) const { return true; }
	};

	struct TTestEqSourceBox
	{
		explicit TTestEqSourceBox(const CIdentifier& rId) : m_rId(rId) { }
		boolean operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getSourceBoxIdentifier()==m_rId; }
		const CIdentifier& m_rId;
	};

	struct TTestEqSourceBoxOutput
	{
		TTestEqSourceBoxOutput(const CIdentifier& rId, uint32 ui32Id) : m_rId(rId), m_ui32Id(ui32Id) { }
		boolean operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getSourceBoxIdentifier()==m_rId && it->second->getSourceBoxOutputIndex()==m_ui32Id; }
		const CIdentifier& m_rId;
		uint32 m_ui32Id;
	};

	struct TTestEqTargetBox
	{
		explicit TTestEqTargetBox(const CIdentifier& rId) : m_rId(rId) { }
		boolean operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getTargetBoxIdentifier()==m_rId; }
		const CIdentifier& m_rId;
	};

	struct TTestEqTargetBoxInput
	{
		TTestEqTargetBoxInput(const CIdentifier& rId, uint32 ui32Id) : m_rId(rId), m_ui32Id(ui32Id) { }
		boolean operator()(map<CIdentifier, CLink*>::const_iterator it) const { return it->second->getTargetBoxIdentifier()==m_rId && it->second->getTargetBoxInputIndex()==m_ui32Id; }
		const CIdentifier& m_rId;
		uint32 m_ui32Id;
	};

	template <class T, class TTest>
	CIdentifier getNextTIdentifier(
		const map<CIdentifier, T*>& vMap,
		const CIdentifier& rPreviousIdentifier,
		const TTest& rTest)
	{
		typename map<CIdentifier, T*>::const_iterator it;

		if(rPreviousIdentifier==OV_UndefinedIdentifier)
		{
			it=vMap.begin();
		}
		else
		{
			it=vMap.find(rPreviousIdentifier);
			if(it==vMap.end())
			{
				return OV_UndefinedIdentifier;
			}
			++it;
		}

		while(it!=vMap.end())
		{
			if(rTest(it))
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

CScenario::CScenario(const IKernelContext& rKernelContext, const CIdentifier& rIdentifier)
	:TBox< IScenario > (rKernelContext)
    ,m_firstMetadataIdentifier(OV_UndefinedIdentifier)
{
	// Some operations on boxes manipulate the owner scenario, for example removing inputs
	// by default we set the scenario as owning itself to avoid segfaults
	this->setOwnerScenario(this);
	this->m_oIdentifier = rIdentifier;
}

CScenario::~CScenario(void)
{
	this->clear();
}

//___________________________________________________________________//
//                                                                   //

boolean CScenario::clear(void)
{
	this->getLogManager() << LogLevel_Debug << "Clearing scenario\n";

	// Clears boxes
	map<CIdentifier, CBox*>::iterator itBox;
	for(itBox=m_vBox.begin(); itBox!=m_vBox.end(); ++itBox)
	{
		delete itBox->second;
	}
	m_vBox.clear();

	// Clears comments
	map<CIdentifier, CComment*>::iterator itComment;
	for(itComment=m_vComment.begin(); itComment!=m_vComment.end(); ++itComment)
	{
		delete itComment->second;
	}
	m_vComment.clear();

	// Clears metadata
	for (auto it = m_metadata.begin(); it != m_metadata.end(); ++it)
	{
		delete it->second;
	}
	m_metadata.clear();

	// Clears links
	map<CIdentifier, CLink*>::iterator itLink;
	for(itLink=m_vLink.begin(); itLink!=m_vLink.end(); ++itLink)
	{
		delete itLink->second;
	}
	m_vLink.clear();

	// Remove all settings
	while (this->getSettingCount())
	{
		this->removeSetting(0);
	}

	// Remove all inputs
	while (this->getInputCount())
	{
		this->removeScenarioInput(0);
	}

	// Remove all outputs
	while (this->getOutputCount())
	{
		this->removeScenarioOutput(0);
	}

	// Clears attributes
	this->removeAllAttributes();

	return true;
}

boolean CScenario::removeScenarioInput(const uint32 ui32InputIndex)
{
	OV_ERROR_UNLESS_KRF(
		ui32InputIndex < this->getInputCount(),
		"Input index = [" << ui32InputIndex << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	// Remove the link within the scenario to this input
	if (ui32InputIndex < m_vScenarioInputLink.size())
	{
		m_vScenarioInputLink.erase(m_vScenarioInputLink.begin() + ui32InputIndex);
	}
	this->removeInput(ui32InputIndex);

	return true;
}

boolean CScenario::removeScenarioOutput(const uint32 ui32OutputIndex)
{
	OV_ERROR_UNLESS_KRF(
		ui32OutputIndex < this->getOutputCount(),
		"Output index = [" << ui32OutputIndex << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	// Remove the link within the scenario to this output
	if (ui32OutputIndex < m_vScenarioOutputLink.size())
	{
		m_vScenarioOutputLink.erase(m_vScenarioOutputLink.begin() + ui32OutputIndex);
	}
	this->removeOutput(ui32OutputIndex);

	return true;
}

boolean CScenario::merge(const IScenario& rScenario, IScenarioMergeCallback* pScenarioMergeCallback, bool bMergeSettings, bool bShouldPreserveIdentifiers)
{
	// Prepares copy
	map < CIdentifier, CIdentifier > l_vIdMapping;

	// Copies boxes
	CIdentifier l_oBoxIdentifier;
	while((l_oBoxIdentifier = rScenario.getNextBoxIdentifier(l_oBoxIdentifier)) != OV_UndefinedIdentifier)
	{
		CIdentifier l_oNewIdentifier;
		const IBox* l_pBox = rScenario.getBoxDetails(l_oBoxIdentifier);
		CIdentifier suggestedNewIdentifier = bShouldPreserveIdentifiers ? l_pBox->getIdentifier() : OV_UndefinedIdentifier;
		this->addBox(l_oNewIdentifier, *l_pBox, suggestedNewIdentifier);
		l_vIdMapping[l_oBoxIdentifier] = l_oNewIdentifier;

		if (pScenarioMergeCallback)
		{
			pScenarioMergeCallback->process(l_oBoxIdentifier, l_oNewIdentifier);
		}
	}

	// Copies links
	CIdentifier l_oLinkIdentifier;
	while((l_oLinkIdentifier = rScenario.getNextLinkIdentifier(l_oLinkIdentifier)) != OV_UndefinedIdentifier)
	{
		CIdentifier l_oNewIdentifier;
		const ILink* l_pLink = rScenario.getLinkDetails(l_oLinkIdentifier);
		this->connect(l_oNewIdentifier,
		              l_vIdMapping[l_pLink->getSourceBoxIdentifier()],
		        l_pLink->getSourceBoxOutputIndex(),
		        l_vIdMapping[l_pLink->getTargetBoxIdentifier()],
		        l_pLink->getTargetBoxInputIndex(),
		        OV_UndefinedIdentifier);

		if (pScenarioMergeCallback)
		{
			pScenarioMergeCallback->process(l_oLinkIdentifier, l_oNewIdentifier);
		}
	}

	// Copy comments

	// Copy metadata
	CIdentifier oldMetadataIdentifier;
	while ((oldMetadataIdentifier = rScenario.getNextMetadataIdentifier(oldMetadataIdentifier)) != OV_UndefinedIdentifier)
	{
		CIdentifier newIdentifier;
		const IMetadata* oldMetadata = rScenario.getMetadataDetails(oldMetadataIdentifier);
		CIdentifier suggestedNewIdentifier = bShouldPreserveIdentifiers ? oldMetadataIdentifier : OV_UndefinedIdentifier;
		this->addMetadata(newIdentifier, suggestedNewIdentifier);
		IMetadata* newMetadata = this->getMetadataDetails(newIdentifier);
		newMetadata->initializeFromExistingMetadata(*oldMetadata);
	}

	// Copy settings if requested

	uint32 l_ui32PreviousSettingCount = this->getSettingCount();

	if (bMergeSettings)
	{
		for (uint32 l_ui32SettingIndex = 0; l_ui32SettingIndex < rScenario.getSettingCount(); l_ui32SettingIndex++)
		{
			CIdentifier l_oSettingTypeIdentifier;
			CString l_sSettingName;
			CString l_sDefaultValue;
			CString l_sValue;
			bool l_bModifiable;
			rScenario.getSettingType(l_ui32SettingIndex, l_oSettingTypeIdentifier);
			rScenario.getSettingName(l_ui32SettingIndex, l_sSettingName);
			rScenario.getSettingDefaultValue(l_ui32SettingIndex, l_sDefaultValue);
			rScenario.getSettingValue(l_ui32SettingIndex, l_sValue);
			rScenario.getSettingMod(l_ui32SettingIndex, l_bModifiable);

			this->addSetting(l_sSettingName, l_oSettingTypeIdentifier, l_sDefaultValue, -1, l_bModifiable);
			this->setSettingValue(l_ui32PreviousSettingCount + l_ui32SettingIndex, l_sValue);

		}

		// In this case we also merge the attributes
		CIdentifier l_oAttributeIdentifier;
		while ((l_oAttributeIdentifier = rScenario.getNextAttributeIdentifier(l_oAttributeIdentifier)) != OV_UndefinedIdentifier)
		{
			CString l_sAttributeValue = rScenario.getAttributeValue(l_oAttributeIdentifier);
			this->addAttribute(l_oAttributeIdentifier, l_sAttributeValue);
		}
	}

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextBoxIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<CBox, TTestTrue<CBox> >(m_vBox, rPreviousIdentifier, TTestTrue<CBox>());
}

const IBox* CScenario::getBoxDetails(
	const CIdentifier& rBoxIdentifier) const
{
//	this->getLogManager() << LogLevel_Debug << "Getting const box details from scenario\n";

	map<CIdentifier, CBox*>::const_iterator itBox;
	itBox=m_vBox.find(rBoxIdentifier);

	OV_ERROR_UNLESS_KRN(
		itBox != m_vBox.end(),
		"Box [" << rBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itBox->second;
}

boolean CScenario::isBox(
	const CIdentifier& rIdentifier) const
{
	map<CIdentifier, CBox*>::const_iterator itBox;
	itBox=m_vBox.find(rIdentifier);
	return itBox!=m_vBox.end();
}

IBox* CScenario::getBoxDetails(
	const CIdentifier& rBoxIdentifier)
{
//	this->getLogManager() << LogLevel_Debug << "Getting box details from scenario\n";

	map<CIdentifier, CBox*>::const_iterator itBox;
	itBox=m_vBox.find(rBoxIdentifier);

	OV_ERROR_UNLESS_KRN(
		itBox != m_vBox.end(),
		"Box [" << rBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itBox->second;
}

boolean CScenario::addBox(
	CIdentifier& rBoxIdentifier,
	const CIdentifier& rSuggestedBoxIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Adding new empty box in scenario\n";

	rBoxIdentifier=getUnusedIdentifier(rSuggestedBoxIdentifier);
	CBox* l_pBox=new CBox(this->getKernelContext());
	l_pBox->setOwnerScenario(this);
	l_pBox->setIdentifier(rBoxIdentifier);

	m_vBox[rBoxIdentifier]=l_pBox;
	return true;
}

boolean CScenario::addBox(
	CIdentifier& rBoxIdentifier,
	const IBox& rBox,
	const CIdentifier& rSuggestedBoxIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Adding a new box in the scenario based on an existing one\n";

	if(!addBox(rBoxIdentifier, rSuggestedBoxIdentifier))
	{
		// error is handled in addBox
		return false;
	}

	IBox* l_pBox=getBoxDetails(rBoxIdentifier);
	if(!l_pBox)
	{
		// error is handled in getBoxDetails
		return false;
	}

	return l_pBox->initializeFromExistingBox(rBox);
}

boolean CScenario::addBox(
	CIdentifier& rBoxIdentifier,
	const CIdentifier& rBoxAlgorithmIdentifier,
	const CIdentifier& rSuggestedBoxIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Adding new box in scenario\n";

	if(!addBox(rBoxIdentifier, rSuggestedBoxIdentifier))
	{
		// error is handled in addBox
		return false;
	}

	IBox* l_pBox=getBoxDetails(rBoxIdentifier);
	if(!l_pBox)
	{
		// error is handled in getBoxDetails
		return false;
	}

	return l_pBox->initializeFromAlgorithmClassIdentifier(rBoxAlgorithmIdentifier);
}

boolean CScenario::addBox(CIdentifier& rBoxIdentifier, const IBoxAlgorithmDesc& rBoxAlgorithmDesc, const CIdentifier& rSuggestedBoxIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Adding new box in scenario based on BoxAlgorithmDesc\n";

	if(!addBox(rBoxIdentifier, rSuggestedBoxIdentifier))
	{
		// error is handled in addBox
		return false;
	}

	IBox* l_pBox = getBoxDetails(rBoxIdentifier);
	if(!l_pBox)
	{
		// error is handled in getBoxDetails
		return false;
	}

	return dynamic_cast<CBox*>(l_pBox)->initializeFromBoxAlgorithmDesc(rBoxAlgorithmDesc);

}

boolean CScenario::removeBox(
	const CIdentifier& rBoxIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Removing box from scenario\n";

	// Finds the box according to its identifier
	map<CIdentifier, CBox*>::iterator itBox;
	itBox=m_vBox.find(rBoxIdentifier);

	OV_ERROR_UNLESS_KRF(
		itBox != m_vBox.end(),
		"Box [" << rBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	this->getLogManager() << LogLevel_Debug << "Found the box !\n";

	// Found the box,
	// now find all the links that are used by this box
	map<CIdentifier, CLink*>::iterator itLink=m_vLink.begin();
	while(itLink!=m_vLink.end())
	{
		map<CIdentifier, CLink*>::iterator itLinkCurrent=itLink;
		++itLink;

		if(itLinkCurrent->second->getSourceBoxIdentifier()==rBoxIdentifier || itLinkCurrent->second->getTargetBoxIdentifier()==rBoxIdentifier)
		{
			this->getLogManager() << LogLevel_Debug << "Found a link to this box - it will be deleted !\n";

			// Deletes this link
			delete itLinkCurrent->second;

			// Removes link from the link list
			m_vLink.erase(itLinkCurrent);
		}
	}

	// Deletes the box itself
	delete itBox->second;

	// Removes box from the box list
	m_vBox.erase(itBox);

	this->getLogManager() << LogLevel_Debug << "The box has been removed\n";

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextCommentIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<CComment, TTestTrue<CComment> >(m_vComment, rPreviousIdentifier, TTestTrue<CComment>());
}

const IComment* CScenario::getCommentDetails(
	const CIdentifier& rCommentIdentifier) const
{
	this->getLogManager() << LogLevel_Debug << "Getting const comment details from scenario\n";

	map<CIdentifier, CComment*>::const_iterator itComment;
	itComment=m_vComment.find(rCommentIdentifier);

	OV_ERROR_UNLESS_KRN(
		itComment != m_vComment.end(),
		"Comment [" << rCommentIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itComment->second;
}

boolean CScenario::isComment(
	const CIdentifier& rIdentifier) const
{
	map<CIdentifier, CComment*>::const_iterator itComment;
	itComment=m_vComment.find(rIdentifier);
	return itComment!=m_vComment.end();
}

IComment* CScenario::getCommentDetails(
	const CIdentifier& rCommentIdentifier)
{
	// this->getLogManager() << LogLevel_Debug << "Getting comment details from scenario\n";

	map<CIdentifier, CComment*>::const_iterator itComment;
	itComment=m_vComment.find(rCommentIdentifier);

	OV_ERROR_UNLESS_KRN(
		itComment != m_vComment.end(),
		"Comment [" << rCommentIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itComment->second;
}

boolean CScenario::addComment(
	CIdentifier& rCommentIdentifier,
	const CIdentifier& rSuggestedCommentIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Adding new empty comment in scenario\n";

	rCommentIdentifier=getUnusedIdentifier(rSuggestedCommentIdentifier);
	CComment* l_pComment=new CComment(this->getKernelContext(), *this);
	l_pComment->setIdentifier(rCommentIdentifier);

	m_vComment[rCommentIdentifier]=l_pComment;
	return true;
}

boolean CScenario::addComment(
	CIdentifier& rCommentIdentifier,
	const IComment& rComment,
	const CIdentifier& rSuggestedCommentIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Adding a new comment in the scenario based on an existing one\n";

	if(!addComment(rCommentIdentifier, rSuggestedCommentIdentifier))
	{
		// error is handled in addComment above
		return false;
	}

	IComment* l_pComment=getCommentDetails(rCommentIdentifier);
	if(!l_pComment)
	{
		// error is handled in getCommentDetails
		return false;
	}

	return l_pComment->initializeFromExistingComment(rComment);
}

boolean CScenario::removeComment(
	const CIdentifier& rCommentIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Removing comment from scenario\n";

	// Finds the comment according to its identifier
	map<CIdentifier, CComment*>::iterator itComment;
	itComment=m_vComment.find(rCommentIdentifier);

	OV_ERROR_UNLESS_KRF(
		itComment != m_vComment.end(),
		"Comment [" << rCommentIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	this->getLogManager() << LogLevel_Debug << "Found the comment !\n";

	// Deletes the comment itself
	delete itComment->second;

	// Removes comment from the comment list
	m_vComment.erase(itComment);

	this->getLogManager() << LogLevel_Debug << "The comment has been removed\n";

	return true;
}

// Metadata
CIdentifier CScenario::getNextMetadataIdentifier(const CIdentifier& previousIdentifier) const
{
	if (previousIdentifier == OV_UndefinedIdentifier)
	{
		return m_firstMetadataIdentifier;
	}

	if (m_metadata.count(previousIdentifier) == 0)
	{
		return OV_UndefinedIdentifier;
	}

	return m_nextMetadataIdentifier.at(previousIdentifier);
}

const IMetadata* CScenario::getMetadataDetails(const CIdentifier& metadataIdentifier) const
{
	auto itMetadata = m_metadata.find(metadataIdentifier);

	OV_ERROR_UNLESS_KRN(
		itMetadata != m_metadata.end(),
		"Metadata [" << metadataIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itMetadata->second;
}

IMetadata* CScenario::getMetadataDetails(const CIdentifier& metadataIdentifier)
{
	auto itMetadata = m_metadata.find(metadataIdentifier);

	OV_ERROR_UNLESS_KRN(
		itMetadata != m_metadata.end(),
		"Metadata [" << metadataIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itMetadata->second;
}

bool CScenario::isMetadata(const CIdentifier& identifier) const
{
	return m_metadata.count(identifier) > 0;
}

bool CScenario::addMetadata(CIdentifier& metadataIdentifier, const CIdentifier& suggestedMetadataIdentifier)
{
	metadataIdentifier = getUnusedIdentifier(suggestedMetadataIdentifier);
	CMetadata* metadata = new CMetadata(this->getKernelContext(), *this);
	metadata->setIdentifier(metadataIdentifier);

	m_nextMetadataIdentifier[metadataIdentifier] = m_firstMetadataIdentifier;
	m_firstMetadataIdentifier = metadataIdentifier;
	m_metadata[metadataIdentifier] = metadata;
	return true;
}

bool CScenario::removeMetadata(const CIdentifier& metadataIdentifier)
{
	// Finds the comment according to its identifier
	auto itMetadata = m_metadata.find(metadataIdentifier);

	OV_ERROR_UNLESS_KRF(
		itMetadata != m_metadata.end(),
		"Comment [" << metadataIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	// Deletes the comment itself
	delete itMetadata->second;

	// Removes comment from the comment list
	m_metadata.erase(itMetadata);

	if (metadataIdentifier == m_firstMetadataIdentifier)
	{
		m_firstMetadataIdentifier = m_nextMetadataIdentifier[m_firstMetadataIdentifier];
		m_nextMetadataIdentifier.erase(metadataIdentifier);
	}
	else
	{
		auto previousIdentifier = std::find_if( m_nextMetadataIdentifier.begin(), m_nextMetadataIdentifier.end(), [metadataIdentifier](const std::pair<CIdentifier, CIdentifier>& v) {
			return v.second == metadataIdentifier;
		});

		m_nextMetadataIdentifier[previousIdentifier->first] = m_nextMetadataIdentifier[metadataIdentifier];
		m_nextMetadataIdentifier.erase(metadataIdentifier);
	}

	return true;
}
//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextLinkIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<CLink, TTestTrue<CLink> >(m_vLink, rPreviousIdentifier, TTestTrue<CLink>());
}

CIdentifier CScenario::getNextLinkIdentifierFromBox(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier) const
{
	return getNextTIdentifier<CLink, TTestEqSourceBox>(m_vLink, rPreviousIdentifier, TTestEqSourceBox(rBoxIdentifier));
}

CIdentifier CScenario::getNextLinkIdentifierFromBoxOutput(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier,
	const uint32 ui32OutputIndex) const
{
	return getNextTIdentifier<CLink, TTestEqSourceBoxOutput>(m_vLink, rPreviousIdentifier, TTestEqSourceBoxOutput(rBoxIdentifier, ui32OutputIndex));
}

CIdentifier CScenario::getNextLinkIdentifierToBox(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier) const
{
	return getNextTIdentifier<CLink, TTestEqTargetBox>(m_vLink, rPreviousIdentifier, TTestEqTargetBox(rBoxIdentifier));
}

CIdentifier CScenario::getNextLinkIdentifierToBoxInput(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier,
	const uint32 ui32InputInex) const
{

	return getNextTIdentifier<CLink, TTestEqTargetBoxInput>(m_vLink, rPreviousIdentifier, TTestEqTargetBoxInput(rBoxIdentifier, ui32InputInex));
}

boolean CScenario::isLink(
	const CIdentifier& rIdentifier) const
{
	map<CIdentifier, CLink*>::const_iterator itLink;
	itLink=m_vLink.find(rIdentifier);
	return itLink!=m_vLink.end();
}

boolean CScenario::setHasIO(const boolean bHasIO)
{
	m_bHasIO = bHasIO;
	return true;
}

boolean CScenario::hasIO() const
{
	return m_bHasIO;
}

boolean CScenario::setScenarioInputLink(const uint32 ui32ScenarioInputIndex, const CIdentifier& rBoxIdentifier, const uint32 ui32BoxInputIndex)
{
	OV_ERROR_UNLESS_KRF(
		ui32ScenarioInputIndex < this->getInputCount(),
		"Scenario Input index = [" << ui32ScenarioInputIndex << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRF(
		this->isBox(rBoxIdentifier),
		"Box [" << rBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	OV_ERROR_UNLESS_KRF(
		ui32BoxInputIndex < this->getBoxDetails(rBoxIdentifier)->getInputCount(),
		"Box Input index = [" << ui32BoxInputIndex << "] is out of range (max index = [" << (this->getBoxDetails(rBoxIdentifier)->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	if (ui32ScenarioInputIndex >= m_vScenarioInputLink.size())
	{
		m_vScenarioInputLink.resize(this->getInputCount());
	}

	// Remove any existing inputs connected to the
	for (size_t uiInputLinkIndex = 0; uiInputLinkIndex < m_vScenarioInputLink.size(); uiInputLinkIndex++)
	{
		CIdentifier l_oAlreadyConnectedBoxIdentifier;
		uint32 l_ui32AlreadyConnectedBoxInputIndex;
		this->getScenarioInputLink(uiInputLinkIndex, l_oAlreadyConnectedBoxIdentifier, l_ui32AlreadyConnectedBoxInputIndex);

		if (l_oAlreadyConnectedBoxIdentifier == rBoxIdentifier && l_ui32AlreadyConnectedBoxInputIndex == ui32BoxInputIndex)
		{
			this->removeScenarioInputLink(uiInputLinkIndex, l_oAlreadyConnectedBoxIdentifier, l_ui32AlreadyConnectedBoxInputIndex);
		}
	}

	// Remove any existing link to this input
	CIdentifier l_oLinkIdentifier;
	while((l_oLinkIdentifier=this->getNextLinkIdentifierToBoxInput(l_oLinkIdentifier, rBoxIdentifier, ui32BoxInputIndex))!=OV_UndefinedIdentifier)
	{
		this->disconnect(l_oLinkIdentifier);
	}

	m_vScenarioInputLink[ui32ScenarioInputIndex] = std::make_pair(rBoxIdentifier, ui32BoxInputIndex);
	return true;
}

boolean CScenario::setScenarioOutputLink(const uint32 ui32ScenarioOutputIndex, const CIdentifier& rBoxIdentifier, const uint32 ui32BoxOutputIndex)
{
	OV_ERROR_UNLESS_KRF(
		ui32ScenarioOutputIndex < this->getOutputCount(),
		"Scenario output index = [" << ui32ScenarioOutputIndex << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRF(
		this->isBox(rBoxIdentifier),
		"Box [" << rBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	OV_ERROR_UNLESS_KRF(
		ui32BoxOutputIndex < this->getBoxDetails(rBoxIdentifier)->getOutputCount(),
		"Box output index = [" << ui32BoxOutputIndex << "] is out of range (max index = [" << (this->getBoxDetails(rBoxIdentifier)->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	if (ui32ScenarioOutputIndex >= m_vScenarioOutputLink.size())
	{
		m_vScenarioOutputLink.resize(this->getOutputCount());
	}

	// Remove any existing outputs connected to the
	for (size_t uiOutputLinkIndex = 0; uiOutputLinkIndex < m_vScenarioOutputLink.size(); uiOutputLinkIndex++)
	{
		CIdentifier l_oAlreadyConnectedBoxIdentifier;
		uint32 l_ui32AlreadyConnectedBoxOutputIndex;
		this->getScenarioOutputLink(uiOutputLinkIndex, l_oAlreadyConnectedBoxIdentifier, l_ui32AlreadyConnectedBoxOutputIndex);

		if (l_oAlreadyConnectedBoxIdentifier == rBoxIdentifier && l_ui32AlreadyConnectedBoxOutputIndex == ui32BoxOutputIndex)
		{
			this->removeScenarioOutputLink(uiOutputLinkIndex, l_oAlreadyConnectedBoxIdentifier, l_ui32AlreadyConnectedBoxOutputIndex);
		}
	}

	m_vScenarioOutputLink[ui32ScenarioOutputIndex] = std::make_pair(rBoxIdentifier, ui32BoxOutputIndex);

	return true;
}

boolean CScenario::getScenarioInputLink(const uint32 ui32ScenarioInputIndex, CIdentifier& rBoxIdentifier, uint32& rBoxInputIndex) const
{
	OV_ERROR_UNLESS_KRF(
		ui32ScenarioInputIndex < this->getInputCount(),
		"Scenario input index = [" << ui32ScenarioInputIndex << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	if (ui32ScenarioInputIndex >= m_vScenarioInputLink.size())
	{
		m_vScenarioInputLink.resize(this->getInputCount());
	}

	rBoxIdentifier = m_vScenarioInputLink[ui32ScenarioInputIndex].first;
	rBoxInputIndex = m_vScenarioInputLink[ui32ScenarioInputIndex].second;

	return true;
}

boolean CScenario::getScenarioOutputLink(const uint32 ui32ScenarioOutputIndex, CIdentifier& rBoxIdentifier, uint32& rBoxOutputIndex) const
{
	OV_ERROR_UNLESS_KRF(
		ui32ScenarioOutputIndex < this->getOutputCount(),
		"Scenario output index = [" << ui32ScenarioOutputIndex << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	if (ui32ScenarioOutputIndex >= m_vScenarioOutputLink.size())
	{
		m_vScenarioOutputLink.resize(this->getOutputCount());
	}

	rBoxIdentifier = m_vScenarioOutputLink[ui32ScenarioOutputIndex].first;
	rBoxOutputIndex = m_vScenarioOutputLink[ui32ScenarioOutputIndex].second;

	return true;
}

// Note: In current implementation only the scenarioInputIndex is necessary as it can only be connected to one input
// but to keep things simpler we give it all the info
boolean CScenario::removeScenarioInputLink(const uint32 ui32ScenarioInputIndex, const CIdentifier& rBoxIdentifier, const uint32 ui32BoxInputIndex)
{
	OV_ERROR_UNLESS_KRF(
		ui32ScenarioInputIndex < this->getInputCount(),
		"Scenario Input index = [" << ui32ScenarioInputIndex << "] is out of range (max index = [" << (this->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRF(
		this->isBox(rBoxIdentifier),
		"Box [" << rBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	OV_ERROR_UNLESS_KRF(
		ui32BoxInputIndex < this->getBoxDetails(rBoxIdentifier)->getInputCount(),
		"Box Input index = [" << ui32BoxInputIndex << "] is out of range (max index = [" << (this->getBoxDetails(rBoxIdentifier)->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	// This should not happen either
	if (ui32ScenarioInputIndex >= m_vScenarioInputLink.size())
	{
		m_vScenarioInputLink.resize(this->getInputCount());
	}

	m_vScenarioInputLink[ui32ScenarioInputIndex] = std::make_pair(OV_UndefinedIdentifier, 0);
	return true;
}

// Note: In current implementation only the scenarioOutputIndex is necessary as it can only be connected to one Output
// but to keep things simpler we give it all the info
boolean CScenario::removeScenarioOutputLink(const uint32 ui32ScenarioOutputIndex, const CIdentifier& rBoxIdentifier, const uint32 ui32BoxOutputIndex)
{
	OV_ERROR_UNLESS_KRF(
		ui32ScenarioOutputIndex < this->getOutputCount(),
		"Scenario output index = [" << ui32ScenarioOutputIndex << "] is out of range (max index = [" << (this->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRF(
		this->isBox(rBoxIdentifier),
		"Box [" << rBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	OV_ERROR_UNLESS_KRF(
		ui32BoxOutputIndex < this->getBoxDetails(rBoxIdentifier)->getOutputCount(),
		"Box output index = [" << ui32BoxOutputIndex << "] is out of range (max index = [" << (this->getBoxDetails(rBoxIdentifier)->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	// This should not happen either
	if (ui32ScenarioOutputIndex >= m_vScenarioOutputLink.size())
	{
		m_vScenarioOutputLink.resize(this->getOutputCount());
	}

	m_vScenarioOutputLink[ui32ScenarioOutputIndex] = std::make_pair(OV_UndefinedIdentifier, 0);
	return true;
}

const ILink* CScenario::getLinkDetails(
	const CIdentifier& rLinkIdentifier) const
{
//	this->getLogManager() << LogLevel_Debug << "Retrieving const link details from scenario\n";

	map<CIdentifier, CLink*>::const_iterator itLink;
	itLink=m_vLink.find(rLinkIdentifier);

	OV_ERROR_UNLESS_KRN(
		itLink != m_vLink.end(),
		"link [" << rLinkIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itLink->second;
}

ILink* CScenario::getLinkDetails(
	const CIdentifier& rLinkIdentifier)
{
	// this->getLogManager() << LogLevel_Debug << "Retrieving link details from scenario\n";

	map<CIdentifier, CLink*>::const_iterator itLink;
	itLink=m_vLink.find(rLinkIdentifier);

	OV_ERROR_UNLESS_KRN(
		itLink != m_vLink.end(),
		"Link [" << rLinkIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	return itLink->second;
}

boolean CScenario::connect(
	CIdentifier& rLinkIdentifier,
	const CIdentifier& rSourceBoxIdentifier,
	const uint32 ui32SourceBoxOutputIndex,
	const CIdentifier& rTargetBoxIdentifier,
	const uint32 ui32TargetBoxInputIndex,
	const CIdentifier& rSuggestedLinkIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Connecting boxes\n";

	map<CIdentifier, CBox*>::const_iterator itBox1;
	map<CIdentifier, CBox*>::const_iterator itBox2;
	itBox1=m_vBox.find(rSourceBoxIdentifier);
	itBox2=m_vBox.find(rTargetBoxIdentifier);

	OV_ERROR_UNLESS_KRF(
		itBox1 != m_vBox.end(),
		"Source Box [" << rSourceBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	OV_ERROR_UNLESS_KRF(
		itBox2 != m_vBox.end(),
		"Target Box [" << rTargetBoxIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	CBox* l_pSourceBox=itBox1->second;
	CBox* l_pTargetBox=itBox2->second;

	OV_ERROR_UNLESS_KRF(
		ui32TargetBoxInputIndex < l_pTargetBox->getInputCount(),
		"Target box input index = [" << ui32TargetBoxInputIndex << "] is out of range (max index = [" << (l_pTargetBox->getInputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	OV_ERROR_UNLESS_KRF(
		ui32SourceBoxOutputIndex < l_pSourceBox->getOutputCount(),
		"Source box output index = [" << ui32SourceBoxOutputIndex << "] is out of range (max index = [" << (l_pSourceBox->getOutputCount() - 1) << "])",
		ErrorType::OutOfBound
	);

	// Looks for any connected link to this box input and removes it
	map<CIdentifier, CLink*>::iterator itLink=m_vLink.begin();
	while(itLink!=m_vLink.end())
	{
		map<CIdentifier, CLink*>::iterator itLinkCurrent=itLink;
		++itLink;

		CLink* l_pLink=itLinkCurrent->second;
		if(l_pLink)
		{
			if(l_pLink->getTargetBoxIdentifier()==rTargetBoxIdentifier && l_pLink->getTargetBoxInputIndex()==ui32TargetBoxInputIndex)
			{
				delete l_pLink;
				m_vLink.erase(itLinkCurrent);
			}
		}
	}

	rLinkIdentifier=getUnusedIdentifier(rSuggestedLinkIdentifier);

	CLink* l_pLink=new CLink(this->getKernelContext(), *this);
	l_pLink->setIdentifier(rLinkIdentifier);
	l_pLink->setSource(rSourceBoxIdentifier, ui32SourceBoxOutputIndex);
	l_pLink->setTarget(rTargetBoxIdentifier, ui32TargetBoxInputIndex);

	m_vLink[l_pLink->getIdentifier()]=l_pLink;

	return true;
}

boolean CScenario::disconnect(
	const CIdentifier& rSourceBoxIdentifier,
	const uint32 ui32SourceBoxOutputIndex,
	const CIdentifier& rTargetBoxIdentifier,
	const uint32 ui32TargetBoxInputIndex)
{
	// Looks for any link with the same signature
	map<CIdentifier, CLink*>::iterator itLink;
	for(itLink=m_vLink.begin(); itLink!=m_vLink.end(); ++itLink)
	{
		CLink* l_pLink=itLink->second;
		if(l_pLink)
		{
			if(l_pLink->getTargetBoxIdentifier()==rTargetBoxIdentifier && l_pLink->getTargetBoxInputIndex()==ui32TargetBoxInputIndex)
			{
				if(l_pLink->getSourceBoxIdentifier()==rSourceBoxIdentifier && l_pLink->getSourceBoxOutputIndex()==ui32SourceBoxOutputIndex)
				{
					// Found a link, so removes it
					delete l_pLink;
					m_vLink.erase(itLink);

					this->getLogManager() << LogLevel_Debug << "Link removed\n";
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

boolean CScenario::disconnect(
	const CIdentifier& rLinkIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Disconnecting boxes\n";

	// Finds the link according to its identifier
	map<CIdentifier, CLink*>::iterator itLink;
	itLink=m_vLink.find(rLinkIdentifier);

	OV_ERROR_UNLESS_KRF(
		itLink != m_vLink.end(),
		"Link [" << rLinkIdentifier.toString() << "] is not part of the scenario",
		ErrorType::ResourceNotFound
	);

	this->getLogManager() << LogLevel_Debug << "Found the link !\n";

	// Deletes the link itself
	delete itLink->second;

	// Removes link from the link list
	m_vLink.erase(itLink);

	this->getLogManager() << LogLevel_Debug << "Link removed\n";

	return true;
}

//___________________________________________________________________//
//                                                                   //

boolean CScenario::applyLocalSettings()
{
	for (auto l_pBox : m_vBox)
	{
		// Expand all the variables inside the newly created scenario by replacing only the $var variables
		CScenarioSettingKeywordParserCallback l_pScenarioSettingKeywordParserCallback(*this);
		this->getConfigurationManager().registerKeywordParser("var", l_pScenarioSettingKeywordParserCallback);

		for (uint32 l_ui32SettingIndex = 0;  l_ui32SettingIndex < l_pBox.second->getSettingCount(); l_ui32SettingIndex++)
		{
			CString l_sSettingName = "";
			CString l_sSettingValue = "";

			l_pBox.second->getSettingName(l_ui32SettingIndex, l_sSettingName);
			l_pBox.second->getSettingValue(l_ui32SettingIndex, l_sSettingValue);

			l_pBox.second->setSettingValue(l_ui32SettingIndex, this->getConfigurationManager().expandOnlyKeyword("var", l_sSettingValue));
			l_pBox.second->getSettingValue(l_ui32SettingIndex, l_sSettingValue);

			if(l_pBox.second->hasAttribute(OVD_AttributeId_SettingOverrideFilename))
			{
				l_sSettingValue = l_pBox.second->getAttributeValue(OVD_AttributeId_SettingOverrideFilename);
				l_pBox.second->setAttributeValue(OVD_AttributeId_SettingOverrideFilename, this->getConfigurationManager().expandOnlyKeyword("var", l_sSettingValue));
			}
		}

		this->getConfigurationManager().unregisterKeywordParser("var");

	}
	return true;
}

boolean CScenario::isMetabox()
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

boolean CScenario::acceptVisitor(
	IObjectVisitor& rObjectVisitor)
{
	CObjectVisitorContext l_oObjectVisitorContext(getKernelContext());

	if(!rObjectVisitor.processBegin(l_oObjectVisitorContext, *this))
	{
		return false;
	}

	map<CIdentifier, CBox*>::iterator i;
	for(i=m_vBox.begin(); i!=m_vBox.end(); ++i)
	{
		if(!i->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	map<CIdentifier, CComment*>::iterator j;
	for(j=m_vComment.begin(); j!=m_vComment.end(); ++j)
	{
		if(!j->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	map<CIdentifier, CLink*>::iterator k;
	for(k=m_vLink.begin(); k!=m_vLink.end(); ++k)
	{
		if(!k->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	if(!rObjectVisitor.processEnd(l_oObjectVisitorContext, *this))
	{
		return false;
	}

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getUnusedIdentifier(const CIdentifier& rSuggestedIdentifier) const
{
	uint64 l_ui64Identifier = (((uint64)rand())<<32)+((uint64)rand());
	if(rSuggestedIdentifier != OV_UndefinedIdentifier)
	{
		l_ui64Identifier = rSuggestedIdentifier.toUInteger()-1;
	}

	CIdentifier l_oResult;
	map<CIdentifier, CBox*>::const_iterator l_itBox;
	map<CIdentifier, CComment*>::const_iterator l_itComment;
	map<CIdentifier, CLink*>::const_iterator l_itLink;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		l_itBox=m_vBox.find(l_oResult);
		l_itComment=m_vComment.find(l_oResult);
		l_itLink=m_vLink.find(l_oResult);
	}
	while(l_itBox!=m_vBox.end() || l_itComment!= m_vComment.end() || l_itLink!= m_vLink.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;

}

boolean CScenario::checkSettings(IConfigurationManager* pConfig)
{
	// Use this once we stop using archeological compilers
	for (auto l_pBox : m_vBox)
	{

		this->applyLocalSettings();
		// Expand all the variables inside the newly created scenario by replacing only the $var variables
		CScenarioSettingKeywordParserCallback l_pScenarioSettingKeywordParserCallback(*this);
		this->getConfigurationManager().registerKeywordParser("var", l_pScenarioSettingKeywordParserCallback);

		for( uint32 l_ui32SettingIndex = 0; l_ui32SettingIndex < l_pBox.second->getSettingCount(); l_ui32SettingIndex++ )
		{
			CString l_sSettingName = "";
			CString l_sRawSettingValue = "";
			CIdentifier l_oTypeIdentifier;

			if(l_pBox.second->hasAttribute(OVD_AttributeId_SettingOverrideFilename))
			{
				return true;
			}
			l_pBox.second->getSettingName(l_ui32SettingIndex, l_sSettingName);
			l_pBox.second->getSettingValue(l_ui32SettingIndex, l_sRawSettingValue);
			l_pBox.second->getSettingType(l_ui32SettingIndex, l_oTypeIdentifier);
			CString l_sSettingValue = l_sRawSettingValue;
			if(pConfig != NULL)
			{
				l_sSettingValue = pConfig->expand(l_sSettingValue);
			}
			else
			{
				l_sSettingValue = this->getConfigurationManager().expandOnlyKeyword("var", l_sSettingValue);
			}

			OV_ERROR_UNLESS_KRF(
				::checkSettingValue(l_sSettingValue, l_oTypeIdentifier),
				"<" << l_pBox.second->getName() << "> The following value: ["<< l_sRawSettingValue <<"] expanded as ["<< l_sSettingValue <<"] given as setting is not valid.",
				ErrorType::BadValue
			);
		}

		this->getConfigurationManager().unregisterKeywordParser("var");

	}
	return true;
}


//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextNeedsUpdateBoxIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<CBox, TTestTrue<CBox> >(m_vNeedsUpdatesBoxes, rPreviousIdentifier, TTestTrue<CBox>());
}

boolean CScenario::hasNeedsUpdateBox()
{
	return !m_vNeedsUpdatesBoxes.empty();
}

boolean CScenario::checkNeedsUpdateBox()
{
	boolean l_bResult = false;
	m_vNeedsUpdatesBoxes.clear();
	for (auto l_pBox : m_vBox)
	{
		CIdentifier l_oBoxHashCode1;
		CIdentifier l_oBoxHashCode2;
		if (l_pBox.second->getAlgorithmClassIdentifier() == OVP_ClassId_BoxAlgorithm_Metabox)
		{
			CString l_sMetaboxIdentifier = l_pBox.second->getAttributeValue(OVP_AttributeId_Metabox_Scenario);
			l_oBoxHashCode1.fromString(this->getKernelContext().getConfigurationManager().lookUpConfigurationTokenValue(CString("Metabox_Scenario_Hash_For_") + l_sMetaboxIdentifier));
		}
		else
		{
			l_oBoxHashCode1 = this->getKernelContext().getPluginManager().getPluginObjectHashValue(l_pBox.second->getAlgorithmClassIdentifier());
		}

		l_oBoxHashCode2.fromString(l_pBox.second->getAttributeValue(OV_AttributeId_Box_InitialPrototypeHashValue));

		if(!(l_oBoxHashCode1 == OV_UndefinedIdentifier || l_oBoxHashCode1 == l_oBoxHashCode2))
		{
			l_bResult = true;
			m_vNeedsUpdatesBoxes.insert(l_pBox);
		}
	}

	return l_bResult;
}
