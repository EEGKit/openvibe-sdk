#include "ovkCScenario.h"
#include "ovkCScenarioExporterContext.h"
#include "ovkCScenarioImporterContext.h"
#include "ovkCScenarioSettingKeywordParserCallback.h"

#include "ovkTBox.hpp"
#include "ovkCComment.h"
#include "ovkCLink.h"
#include "ovkCMessageLink.h"
#include "ovkCProcessingUnit.h"

#include "../ovkCObjectVisitorContext.h"

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
		TTestEqSourceBox(const CIdentifier& rId) : m_rId(rId) { }
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
		TTestEqTargetBox(const CIdentifier& rId) : m_rId(rId) { }
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

	struct TTestEqProcessingUnitIdentifier
	{
		TTestEqProcessingUnitIdentifier(const CIdentifier& rId) : m_rId(rId) { }
		boolean operator()(map<CIdentifier, CBox*>::const_iterator it) const { return it->second->getProcessingUnitIdentifier()==m_rId; }
		const CIdentifier& m_rId;
	};
//{
	//factorize??
	struct TTestMessageEqSourceBox
	{
		TTestMessageEqSourceBox(const CIdentifier& rId) : m_rId(rId) { }
		boolean operator()(map<CIdentifier, CMessageLink*>::const_iterator it) const { return it->second->getSourceBoxIdentifier()==m_rId; }
		const CIdentifier& m_rId;
	};

	struct TTestMessageEqSourceBoxOutput
	{
		TTestMessageEqSourceBoxOutput(const CIdentifier& rId, uint32 ui32Id) : m_rId(rId), m_ui32Id(ui32Id) { }
		boolean operator()(map<CIdentifier, CMessageLink*>::const_iterator it) const { return it->second->getSourceBoxIdentifier()==m_rId && it->second->getSourceBoxOutputIndex()==m_ui32Id; }
		const CIdentifier& m_rId;
		uint32 m_ui32Id;
	};

	struct TTestMessageEqTargetBox
	{
		TTestMessageEqTargetBox(const CIdentifier& rId) : m_rId(rId) { }
		boolean operator()(map<CIdentifier, CMessageLink*>::const_iterator it) const { return it->second->getTargetBoxIdentifier()==m_rId; }
		const CIdentifier& m_rId;
	};

	struct TTestMessageEqTargetBoxInput
	{
		TTestMessageEqTargetBoxInput(const CIdentifier& rId, uint32 ui32Id) : m_rId(rId), m_ui32Id(ui32Id) { }
		boolean operator()(map<CIdentifier, CMessageLink*>::const_iterator it) const { return it->second->getTargetBoxIdentifier()==m_rId && it->second->getTargetBoxInputIndex()==m_ui32Id; }
		const CIdentifier& m_rId;
		uint32 m_ui32Id;
	};
//}

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
			it++;
		}

		while(it!=vMap.end())
		{
			if(rTest(it))
			{
				return it->first;
			}
			it++;
		}

		return OV_UndefinedIdentifier;
	}
};

//___________________________________________________________________//
//                                                                   //

CScenario::CScenario(const IKernelContext& rKernelContext, const CIdentifier& rIdentifier)
	:TBox< IScenario > (rKernelContext)
	,m_oIdentifier(rIdentifier)
{
	// Some operations on boxes manipulate the owner scenario, for example removing inputs
	// by default we set the scenario as owning itself to avoid segfaults
	this->setOwnerScenario(this);
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
	for(itBox=m_vBox.begin(); itBox!=m_vBox.end(); itBox++)
	{
		delete itBox->second;
	}
	m_vBox.clear();

	// Clears comments
	map<CIdentifier, CComment*>::iterator itComment;
	for(itComment=m_vComment.begin(); itComment!=m_vComment.end(); itComment++)
	{
		delete itComment->second;
	}
	m_vComment.clear();

	// Clears links
	map<CIdentifier, CLink*>::iterator itLink;
	for(itLink=m_vLink.begin(); itLink!=m_vLink.end(); itLink++)
	{
		delete itLink->second;
	}
	m_vLink.clear();

	// Clears message links
	map<CIdentifier, CMessageLink*>::iterator itMessageLink;
	for(itMessageLink=m_vMessageLink.begin(); itMessageLink!=m_vMessageLink.end(); itMessageLink++)
	{
		delete itMessageLink->second;
	}
	m_vMessageLink.clear();

	// Clears processing units
	map<CIdentifier, CProcessingUnit*>::iterator itProcessingUnit;
	for(itProcessingUnit=m_vProcessingUnit.begin(); itProcessingUnit!=m_vProcessingUnit.end(); itProcessingUnit++)
	{
		delete itProcessingUnit->second;
	}
	m_vProcessingUnit.clear();

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
	if (ui32InputIndex >= this->getInputCount())
	{
		return false;
	}

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
	if (ui32OutputIndex >= this->getOutputCount())
	{
		return false;
	}

	// Remove the link within the scenario to this output
	if (ui32OutputIndex < m_vScenarioOutputLink.size())
	{
		m_vScenarioOutputLink.erase(m_vScenarioOutputLink.begin() + ui32OutputIndex);
	}
	this->removeOutput(ui32OutputIndex);

	return true;
}

boolean CScenario::merge(const IScenario& rScenario, IScenarioMergeCallback* pScenarioMergeCallback, boolean bMergeSettings)
{
	// Prepares copy
	map < CIdentifier, CIdentifier > l_vIdMapping;

	// Copies boxes
	CIdentifier l_oBoxIdentifier;
	while((l_oBoxIdentifier = rScenario.getNextBoxIdentifier(l_oBoxIdentifier)) != OV_UndefinedIdentifier)
	{
		CIdentifier l_oNewIdentifier;
		const IBox* l_pBox = rScenario.getBoxDetails(l_oBoxIdentifier);
		this->addBox(l_oNewIdentifier, *l_pBox, OV_UndefinedIdentifier);
		l_vIdMapping[l_oBoxIdentifier] = l_oNewIdentifier;

		if (pScenarioMergeCallback)
		{
			pScenarioMergeCallback->process(l_oBoxIdentifier, l_oNewIdentifier);
		}

		// Updates visualisation manager
		// CIdentifier l_oBoxAlgorithmIdentifier = l_pBox->getAlgorithmClassIdentifier();
		// const IPluginObjectDesc* l_pPOD = this->getPluginManager().getPluginObjectDescCreating(l_oBoxAlgorithmIdentifier);
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

	// Get the whole visualisation tree from the original scenario and push it into the current one

	// First we are going to determine the order in which we can insert elements into the visualisation tree
	// then we will insert them into the original tree as top levels

	// Collect ids of all widgets in old scenario
	std::vector<CIdentifier> l_vOldWidgetIdentifier;
	CIdentifier l_oCurrentWidgetIdentifier;
	while ((rScenario.getVisualisationTreeDetails().getNextVisualisationWidgetIdentifier(l_oCurrentWidgetIdentifier)))
	{
		l_vOldWidgetIdentifier.push_back(l_oCurrentWidgetIdentifier);
	}

	std::vector<CIdentifier> l_vOrderedWidget;

	// Now reorder them in a way that parents are always before their children
	while (!l_vOldWidgetIdentifier.empty())
	{
		// Go through all non-ordered widgets, if the widgets has no parent or its parent is already
		// in the list, we add it to the list.
		for (auto l_itOldWidget = l_vOldWidgetIdentifier.begin(); l_itOldWidget != l_vOldWidgetIdentifier.end();)
		{
			CIdentifier l_oWidgetParentIdentifier = rScenario.getVisualisationTreeDetails().getVisualisationWidget(*l_itOldWidget)->getParentIdentifier();

			bool l_bIsParentInList = std::find(l_vOrderedWidget.begin(), l_vOrderedWidget.end(), l_oWidgetParentIdentifier) != l_vOrderedWidget.end();

			if (l_oWidgetParentIdentifier == OV_UndefinedIdentifier || l_bIsParentInList)
			{
				l_vOrderedWidget.push_back(*l_itOldWidget);
				l_itOldWidget = l_vOldWidgetIdentifier.erase(l_itOldWidget);
			}
			else
			{
				++l_itOldWidget;
			}
		}
	}


	// Now go through the tree again and replace the parent identifiers and indexes by the new identifiers
//	for (auto l_oWidget : l_vOrderedWidget) // better
	for (auto l_itWidget = l_vOrderedWidget.cbegin(); l_itWidget != l_vOrderedWidget.cend(); l_itWidget++ ) // old
	{
		const CIdentifier& l_oWidget = *l_itWidget;

		IVisualisationWidget* l_pVisualisationWidget = rScenario.getVisualisationTreeDetails().getVisualisationWidget(l_oWidget);

		// Find out the index at which the widget was in the parent
		CIdentifier l_oParentIdentifier = l_pVisualisationWidget->getParentIdentifier();
		uint32 l_ui32WidgetIndexWithinParent = 0;
		if (l_oParentIdentifier != OV_UndefinedIdentifier)
		{
			rScenario.getVisualisationTreeDetails().getVisualisationWidget(l_oParentIdentifier)->getChildIndex(l_oWidget, l_ui32WidgetIndexWithinParent);
		}

		CIdentifier l_oNewVisualisationWidgetIdentifier;
		this->getVisualisationTreeDetails().addVisualisationWidget(
		            l_oNewVisualisationWidgetIdentifier,
		            l_pVisualisationWidget->getName(),
		            l_pVisualisationWidget->getType(),
		            // This works because we are sure that all parents already exist within the tree
		            l_vIdMapping[l_oParentIdentifier],
		            l_ui32WidgetIndexWithinParent,
		            l_vIdMapping[l_pVisualisationWidget->getBoxIdentifier()],
		            l_pVisualisationWidget->getNbChildren(),
		            OV_UndefinedIdentifier
		            );

		l_vIdMapping[l_oWidget] = l_oNewVisualisationWidgetIdentifier;

		// Copy the widget attributes
		CIdentifier l_oWidgetAttributeIdentifier = OV_UndefinedIdentifier;
		while ((l_oWidgetAttributeIdentifier = l_pVisualisationWidget->getNextAttributeIdentifier(l_oWidgetAttributeIdentifier)) != OV_UndefinedIdentifier)
		{
			CString l_sAttributeValue = l_pVisualisationWidget->getAttributeValue(l_oWidgetAttributeIdentifier);
			this->getVisualisationTreeDetails().getVisualisationWidget(l_oNewVisualisationWidgetIdentifier)->addAttribute(l_oWidgetAttributeIdentifier, l_sAttributeValue);
		}

	}

	// Copy visualisation tree attributes
	CIdentifier l_oVisualisationTreeAttributeIdentifier = OV_UndefinedIdentifier;
	while ((l_oVisualisationTreeAttributeIdentifier = rScenario.getVisualisationTreeDetails().getNextAttributeIdentifier(l_oVisualisationTreeAttributeIdentifier)) != OV_UndefinedIdentifier)
	{
		CString l_sAttributeValue = rScenario.getVisualisationTreeDetails().getAttributeValue(l_oVisualisationTreeAttributeIdentifier);
		this->getVisualisationTreeDetails().addAttribute(l_oVisualisationTreeAttributeIdentifier, l_sAttributeValue);
	}

	// Copies message links
	// TODO_JL copy message links here

	// Copy comments

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

boolean CScenario::setVisualisationTreeIdentifier(const CIdentifier& rVisualisationTreeIdentifier)
{
	m_oVisualisationTreeIdentifier=rVisualisationTreeIdentifier;
	return true;
}

CIdentifier CScenario::getVisualisationTreeIdentifier(void) const
{
	return m_oVisualisationTreeIdentifier;
}

const IVisualisationTree& CScenario::getVisualisationTreeDetails() const
{
	return getKernelContext().getVisualisationManager().getVisualisationTree(m_oVisualisationTreeIdentifier);
}

IVisualisationTree& CScenario::getVisualisationTreeDetails()
{
	return getKernelContext().getVisualisationManager().getVisualisationTree(m_oVisualisationTreeIdentifier);
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextBoxIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<CBox, TTestTrue<CBox> >(m_vBox, rPreviousIdentifier, TTestTrue<CBox>());
}

CIdentifier CScenario::getNextBoxIdentifierOnProcessingUnit(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rProcessingUnitIdentifier) const
{
	return getNextTIdentifier<CBox, TTestEqProcessingUnitIdentifier >(m_vBox, rPreviousIdentifier, TTestEqProcessingUnitIdentifier(rProcessingUnitIdentifier));
}

const IBox* CScenario::getBoxDetails(
	const CIdentifier& rBoxIdentifier) const
{
//	this->getLogManager() << LogLevel_Debug << "Getting const box details from scenario\n";

	map<CIdentifier, CBox*>::const_iterator itBox;
	itBox=m_vBox.find(rBoxIdentifier);
	if(itBox==m_vBox.end())
	{
		this->getLogManager() << LogLevel_Warning << "The box does not exist\n";
		return NULL;
	}
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
	if(itBox==m_vBox.end())
	{
		this->getLogManager() << LogLevel_Warning << "The box does not exist\n";
		return NULL;
	}
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
		return false;
	}

	IBox* l_pBox=getBoxDetails(rBoxIdentifier);
	if(!l_pBox)
	{
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
		return false;
	}

	IBox* l_pBox=getBoxDetails(rBoxIdentifier);
	if(!l_pBox)
	{
		return false;
	}

	return l_pBox->initializeFromAlgorithmClassIdentifier(rBoxAlgorithmIdentifier);
}

boolean CScenario::addBox(CIdentifier& rBoxIdentifier, const IBoxAlgorithmDesc& rBoxAlgorithmDesc, const CIdentifier& rSuggestedBoxIdentifier)
{
	this->getLogManager() << LogLevel_Trace << "Adding new box in scenario based on BoxAlgorithmDesc\n";

	if(!addBox(rBoxIdentifier, rSuggestedBoxIdentifier))
	{
		return false;
	}

	IBox* l_pBox = getBoxDetails(rBoxIdentifier);
	if(!l_pBox)
	{
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
	if(itBox==m_vBox.end())
	{
		// The box does not exist !
		this->getLogManager() << LogLevel_Warning << "The box does not exist\n";
		return false;
	}

	this->getLogManager() << LogLevel_Debug << "Found the box !\n";

	// Found the box,
	// now find all the links that are used by this box
	map<CIdentifier, CLink*>::iterator itLink=m_vLink.begin();
	while(itLink!=m_vLink.end())
	{
		map<CIdentifier, CLink*>::iterator itLinkCurrent=itLink;
		itLink++;

		if(itLinkCurrent->second->getSourceBoxIdentifier()==rBoxIdentifier || itLinkCurrent->second->getTargetBoxIdentifier()==rBoxIdentifier)
		{
			this->getLogManager() << LogLevel_Debug << "Found a link to this box - it will be deleted !\n";

			// Deletes this link
			delete itLinkCurrent->second;

			// Removes link from the link list
			m_vLink.erase(itLinkCurrent);
		}
	}

	// same thing with message links
	map<CIdentifier, CMessageLink*>::iterator itMessageLink=m_vMessageLink.begin();
	while(itMessageLink!=m_vMessageLink.end())
	{
		map<CIdentifier, CMessageLink*>::iterator itMessageLinkCurrent=itMessageLink;
		itMessageLink++;

		if(itMessageLinkCurrent->second->getSourceBoxIdentifier()==rBoxIdentifier || itMessageLinkCurrent->second->getTargetBoxIdentifier()==rBoxIdentifier)
		{
			this->getLogManager() << LogLevel_Debug << "Found a Message link to this box - it will be deleted !\n";

			// Deletes this link
			delete itMessageLinkCurrent->second;

			// Removes link from the link list
			m_vMessageLink.erase(itMessageLinkCurrent);
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
	if(itComment==m_vComment.end())
	{
		this->getLogManager() << LogLevel_Warning << "The comment does not exist\n";
		return NULL;
	}
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
	if(itComment==m_vComment.end())
	{
		this->getLogManager() << LogLevel_Warning << "The comment does not exist\n";
		return NULL;
	}
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
		return false;
	}

	IComment* l_pComment=getCommentDetails(rCommentIdentifier);
	if(!l_pComment)
	{
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
	if(itComment==m_vComment.end())
	{
		// The comment does not exist !
		this->getLogManager() << LogLevel_Warning << "The comment does not exist\n";
		return false;
	}

	this->getLogManager() << LogLevel_Debug << "Found the comment !\n";

	// Deletes the comment itself
	delete itComment->second;

	// Removes comment from the comment list
	m_vComment.erase(itComment);

	this->getLogManager() << LogLevel_Debug << "The comment has been removed\n";

	return true;
}

//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextLinkIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<CLink, TTestTrue<CLink> >(m_vLink, rPreviousIdentifier, TTestTrue<CLink>());
}

CIdentifier CScenario::getNextMessageLinkIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<CMessageLink, TTestTrue<CMessageLink> >(m_vMessageLink, rPreviousIdentifier, TTestTrue<CMessageLink>());
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
	if (ui32ScenarioInputIndex >= this->getInputCount() || !this->isBox(rBoxIdentifier) || ui32BoxInputIndex >= this->getBoxDetails(rBoxIdentifier)->getInputCount())
	{
		return false;
	}
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
	if (ui32ScenarioOutputIndex >= this->getOutputCount() ||
	        !this->isBox(rBoxIdentifier) ||
	        ui32BoxOutputIndex >= this->getBoxDetails(rBoxIdentifier)->getOutputCount())
	{
		return false;
	}
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
	if (ui32ScenarioInputIndex >= this->getInputCount())
	{
		return false;
	}

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
	if (ui32ScenarioOutputIndex >= this->getOutputCount())
	{
		return false;
	}

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
	if (ui32ScenarioInputIndex >= this->getInputCount() ||
	        !this->isBox(rBoxIdentifier) ||
	        ui32BoxInputIndex >= this->getBoxDetails(rBoxIdentifier)->getInputCount())
	{
		// this really should not happen
		return false;
	}

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
	if (ui32ScenarioOutputIndex >= this->getOutputCount() ||
	        !this->isBox(rBoxIdentifier) ||
	        ui32BoxOutputIndex >= this->getBoxDetails(rBoxIdentifier)->getOutputCount())
	{
		// this really should not happen
		return false;
	}

	// This should not happen either
	if (ui32ScenarioOutputIndex >= m_vScenarioOutputLink.size())
	{
		m_vScenarioOutputLink.resize(this->getOutputCount());
	}

	m_vScenarioOutputLink[ui32ScenarioOutputIndex] = std::make_pair(OV_UndefinedIdentifier, 0);
	return true;
}
//*
CIdentifier CScenario::getNextMessageLinkIdentifierFromBox(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier) const
{
	return getNextTIdentifier<CMessageLink, TTestMessageEqSourceBox>(m_vMessageLink, rPreviousIdentifier, TTestMessageEqSourceBox(rBoxIdentifier));
}

CIdentifier CScenario::getNextMessageLinkIdentifierFromBoxOutput(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier,
	const uint32 ui32OutputIndex) const
{
	return getNextTIdentifier<CMessageLink, TTestMessageEqSourceBoxOutput>(m_vMessageLink, rPreviousIdentifier, TTestMessageEqSourceBoxOutput(rBoxIdentifier, ui32OutputIndex));
}

CIdentifier CScenario::getNextMessageLinkIdentifierToBox(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier) const
{
	return getNextTIdentifier<CMessageLink, TTestMessageEqTargetBox>(m_vMessageLink, rPreviousIdentifier, TTestMessageEqTargetBox(rBoxIdentifier));
}

CIdentifier CScenario::getNextMessageLinkIdentifierToBoxInput(
	const CIdentifier& rPreviousIdentifier,
	const CIdentifier& rBoxIdentifier,
	const uint32 ui32InputInex) const
{

	return getNextTIdentifier<CMessageLink, TTestMessageEqTargetBoxInput>(m_vMessageLink, rPreviousIdentifier, TTestMessageEqTargetBoxInput(rBoxIdentifier, ui32InputInex));
}

boolean CScenario::isMessageLink(
	const CIdentifier& rIdentifier) const
{
	map<CIdentifier, CMessageLink*>::const_iterator itMessageLink;
	itMessageLink=m_vMessageLink.find(rIdentifier);
	return itMessageLink!=m_vMessageLink.end();
}
//*/

const ILink* CScenario::getLinkDetails(
	const CIdentifier& rLinkIdentifier) const
{
//	this->getLogManager() << LogLevel_Debug << "Retrieving const link details from scenario\n";

	map<CIdentifier, CLink*>::const_iterator itLink;
	itLink=m_vLink.find(rLinkIdentifier);
	if(itLink==m_vLink.end())
	{
		this->getLogManager() << LogLevel_Warning << "The link does not exist\n";
		return NULL;
	}
	return itLink->second;
}

ILink* CScenario::getLinkDetails(
	const CIdentifier& rLinkIdentifier)
{
	// this->getLogManager() << LogLevel_Debug << "Retrieving link details from scenario\n";

	map<CIdentifier, CLink*>::const_iterator itLink;
	itLink=m_vLink.find(rLinkIdentifier);
	if(itLink==m_vLink.end())
	{
		this->getLogManager() << LogLevel_Warning << "The link does not exist\n";
		return NULL;
	}
	return itLink->second;
}

const ILink* CScenario::getMessageLinkDetails(
	const CIdentifier& rLinkIdentifier) const
{
	this->getLogManager() << LogLevel_Debug << "Retrieving message link details from scenario\n";

	map<CIdentifier, CMessageLink*>::const_iterator itLink;
	itLink=m_vMessageLink.find(rLinkIdentifier);
	if(itLink==m_vMessageLink.end())
	{
		this->getLogManager() << LogLevel_Warning << "The message link does not exist\n";
		return NULL;
	}
	return itLink->second;
}

ILink* CScenario::getMessageLinkDetails(
	const CIdentifier& rLinkIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Retrieving message link details from scenario\n";

	map<CIdentifier, CMessageLink*>::const_iterator itLink;
	itLink=m_vMessageLink.find(rLinkIdentifier);
	if(itLink==m_vMessageLink.end())
	{
		this->getLogManager() << LogLevel_Warning << "The message link does not exist\n";
		return NULL;
	}
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
	if(itBox1==m_vBox.end() || itBox2==m_vBox.end())
	{
		this->getLogManager() << LogLevel_Warning << "At least one of the boxes does not exist\n";
		return false;
	}
	CBox* l_pSourceBox=itBox1->second;
	CBox* l_pTargetBox=itBox2->second;
	if(ui32SourceBoxOutputIndex >= l_pSourceBox->getOutputCount())
	{
		this->getLogManager() << LogLevel_Warning << "Wrong output index\n";
		return false;
	}
	if(ui32TargetBoxInputIndex >= l_pTargetBox->getInputCount())
	{
		this->getLogManager() << LogLevel_Warning << "Wrong input index\n";
		return false;
	}

	// Looks for any connected link to this box input and removes it
	map<CIdentifier, CLink*>::iterator itLink=m_vLink.begin();
	while(itLink!=m_vLink.end())
	{
		map<CIdentifier, CLink*>::iterator itLinkCurrent=itLink;
		itLink++;

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


boolean CScenario::connectMessage(
	CIdentifier& rLinkIdentifier,
	const CIdentifier& rSourceBoxIdentifier,
	const uint32 ui32SourceBoxOutputIndex,
	const CIdentifier& rTargetBoxIdentifier,
	const uint32 ui32TargetBoxInputIndex,
	const CIdentifier& rSuggestedMessageLinkIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "(Message) Connecting boxes\n";

	map<CIdentifier, CBox*>::const_iterator itBox1;
	map<CIdentifier, CBox*>::const_iterator itBox2;
	map<CIdentifier, CBox*>::const_iterator mit;
	itBox1=m_vBox.find(rSourceBoxIdentifier);
	itBox2=m_vBox.find(rTargetBoxIdentifier);
	if(itBox1==m_vBox.end() || itBox2==m_vBox.end())
	{
		for(mit=m_vBox.begin(); mit!=m_vBox.end(); mit++)
		{
			this->getLogManager() << LogLevel_Warning << mit->first << mit->second->getName() << "\n";
		}
		this->getLogManager() << LogLevel_Warning << "(Message) At least one of the boxes does not exist\n";
		/*
		if (itBox1==m_vBox.end())
		{
			this->getLogManager() << LogLevel_Warning << "Source missing\n";
		}
		if(itBox2==m_vBox.end())
		{
			this->getLogManager() << LogLevel_Warning << "Target missing" << rTargetBoxIdentifier << "\n";
		}
		//*/
		return false;
	}
	CBox* l_pSourceBox=itBox1->second;
	CBox* l_pTargetBox=itBox2->second;
	if(ui32SourceBoxOutputIndex >= l_pSourceBox->getMessageOutputCount())
	{
		this->getLogManager() << LogLevel_Warning << "Wrong message output index\n";
		return false;
	}
	if(ui32TargetBoxInputIndex >= l_pTargetBox->getMessageInputCount())
	{
		this->getLogManager() << LogLevel_Warning << "Wrong message input index\n";
		return false;
	}

	// Looks for any connected link to this box input and removes it
	map<CIdentifier, CMessageLink*>::iterator itLink=m_vMessageLink.begin();
	while(itLink!=m_vMessageLink.end())
	{
		map<CIdentifier, CMessageLink*>::iterator itLinkCurrent=itLink;
		itLink++;

		CMessageLink* l_pLink=itLinkCurrent->second;
		if(l_pLink)
		{
			if(l_pLink->getTargetBoxIdentifier()==rTargetBoxIdentifier && l_pLink->getTargetBoxInputIndex()==ui32TargetBoxInputIndex)
			{
				delete l_pLink;
				m_vMessageLink.erase(itLinkCurrent);
			}
		}
	}

	rLinkIdentifier=getUnusedIdentifier(rSuggestedMessageLinkIdentifier);

	CMessageLink* l_pLink=new CMessageLink(this->getKernelContext(), *this);
	l_pLink->setIdentifier(rLinkIdentifier);
	l_pLink->setSource(rSourceBoxIdentifier, ui32SourceBoxOutputIndex);
	l_pLink->setTarget(rTargetBoxIdentifier, ui32TargetBoxInputIndex);

	m_vMessageLink[l_pLink->getIdentifier()]=l_pLink;

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
	for(itLink=m_vLink.begin(); itLink!=m_vLink.end(); itLink++)
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

	this->getLogManager() << LogLevel_Warning << "The link does not exist\n";
	return false;
}

boolean CScenario::disconnect(
	const CIdentifier& rLinkIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Disconnecting boxes\n";

	// Finds the link according to its identifier
	map<CIdentifier, CLink*>::iterator itLink;
	itLink=m_vLink.find(rLinkIdentifier);
	if(itLink==m_vLink.end())
	{
		// The link does not exist !
		this->getLogManager() << LogLevel_Warning << "The link does not exist\n";
		return false;
	}

	this->getLogManager() << LogLevel_Debug << "Found the link !\n";

	// Deletes the link itself
	delete itLink->second;

	// Removes link from the link list
	m_vLink.erase(itLink);

	this->getLogManager() << LogLevel_Debug << "Link removed\n";

	return true;
}

///disconnectmessage

boolean CScenario::disconnectMessage(
	const CIdentifier& rSourceBoxIdentifier,
	const uint32 ui32SourceBoxOutputIndex,
	const CIdentifier& rTargetBoxIdentifier,
	const uint32 ui32TargetBoxInputIndex)
{
	// Looks for any link with the same signature
	map<CIdentifier, CMessageLink*>::iterator itLink;
	for(itLink=m_vMessageLink.begin(); itLink!=m_vMessageLink.end(); itLink++)
	{
		CMessageLink* l_pLink=itLink->second;
		if(l_pLink)
		{
			if(l_pLink->getTargetBoxIdentifier()==rTargetBoxIdentifier && l_pLink->getTargetBoxInputIndex()==ui32TargetBoxInputIndex)
			{
				if(l_pLink->getSourceBoxIdentifier()==rSourceBoxIdentifier && l_pLink->getSourceBoxOutputIndex()==ui32SourceBoxOutputIndex)
				{
					// Found a link, so removes it
					delete l_pLink;
					m_vMessageLink.erase(itLink);

					this->getLogManager() << LogLevel_Debug << "Message Link removed\n";
					return true;
				}
			}
		}
	}

	this->getLogManager() << LogLevel_Warning << "The Message link does not exist\n";
	return false;
}

boolean CScenario::disconnectMessage(
	const CIdentifier& rLinkIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "(Message) Disconnecting boxes\n";

	// Finds the link according to its identifier
	map<CIdentifier, CMessageLink*>::iterator itLink;
	itLink=m_vMessageLink.find(rLinkIdentifier);
	if(itLink==m_vMessageLink.end())
	{
		// The link does not exist !
		this->getLogManager() << LogLevel_Warning << "The Message link does not exist\n";
		return false;
	}

	this->getLogManager() << LogLevel_Debug << "Found the Message link !\n";

	// Deletes the link itself
	delete itLink->second;

	// Removes link from the link list
	m_vMessageLink.erase(itLink);

	this->getLogManager() << LogLevel_Debug << "Message Link removed\n";

	return true;
}

//


//___________________________________________________________________//
//                                                                   //

CIdentifier CScenario::getNextProcessingUnitIdentifier(
	const CIdentifier& rPreviousIdentifier) const
{
	return getNextTIdentifier<CProcessingUnit, TTestTrue<CProcessingUnit> >(m_vProcessingUnit, rPreviousIdentifier, TTestTrue<CProcessingUnit>());
}

boolean CScenario::isProcessingUnit(
	const CIdentifier& rIdentifier) const
{
	map<CIdentifier, CProcessingUnit*>::const_iterator itProcessingUnit=m_vProcessingUnit.find(rIdentifier);
	return itProcessingUnit!=m_vProcessingUnit.end()?true:false;
}

const IProcessingUnit* CScenario::getProcessingUnitDetails(
	const CIdentifier& rProcessingUnitIdentifier) const
{
	this->getLogManager() << LogLevel_Debug << "Getting const processing unit details from scenario\n";

	map<CIdentifier, CProcessingUnit*>::const_iterator itProcessingUnit;
	itProcessingUnit=m_vProcessingUnit.find(rProcessingUnitIdentifier);
	if(itProcessingUnit==m_vProcessingUnit.end())
	{
		this->getLogManager() << LogLevel_Warning << "The processing unit does not exist\n";
		return NULL;
	}
	return itProcessingUnit->second;
}

IProcessingUnit* CScenario::getProcessingUnitDetails(
	const CIdentifier& rProcessingUnitIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Getting processing unit details from scenario\n";

	map<CIdentifier, CProcessingUnit*>::const_iterator itProcessingUnit;
	itProcessingUnit=m_vProcessingUnit.find(rProcessingUnitIdentifier);
	if(itProcessingUnit==m_vProcessingUnit.end())
	{
		this->getLogManager() << LogLevel_Warning << "The processing unit does not exist\n";
		return NULL;
	}
	return itProcessingUnit->second;
}

boolean CScenario::addProcessingUnit(
	CIdentifier& rProcessingUnitIdentifier,
	const CIdentifier& rSuggestedProcessingUnitIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Adding new processing unit in scenario\n";

	CProcessingUnit* l_pProcessingUnit=new CProcessingUnit(getKernelContext());
	rProcessingUnitIdentifier=getUnusedIdentifier(rSuggestedProcessingUnitIdentifier);
	l_pProcessingUnit->setIdentifier(rProcessingUnitIdentifier);

	m_vProcessingUnit[rProcessingUnitIdentifier]=l_pProcessingUnit;
	return true;
}

boolean CScenario::removeProcessingUnit(
	const CIdentifier& rProcessingUnitIdentifier)
{
	this->getLogManager() << LogLevel_Debug << "Removing processing unit from scenario\n";

	// Finds the processing unit according to its identifier
	map<CIdentifier, CProcessingUnit*>::iterator itProcessingUnit;
	itProcessingUnit=m_vProcessingUnit.find(rProcessingUnitIdentifier);
	if(itProcessingUnit==m_vProcessingUnit.end())
	{
		// The processing unit does not exist !
		this->getLogManager() << LogLevel_Warning << "The processing unit does not exist\n";
		return false;
	}

	// Found the processing unit,
	// now unaffect all the boxes that are using this processing unit
	map<CIdentifier, CBox*>::const_iterator itBox;
	for(itBox=m_vBox.begin(); itBox!=m_vBox.end(); )
	{
		if(itBox->second->getProcessingUnitIdentifier() == rProcessingUnitIdentifier)
		{
			this->getLogManager() << LogLevel_Debug << "The box " << itBox->second->getIdentifier() << " is affected to this processing unit - it will now be affected to undefined processing unit\n";

			// Affects to unknown identifier
			itBox->second->setProcessingUnitIdentifier(OV_UndefinedIdentifier);
		}
		itBox++;
	}

	// Deletes the processing unit itself
	delete itProcessingUnit->second;

	// Removes processing unit from the processing unit list
	m_vProcessingUnit.erase(itProcessingUnit);
	return true;
}

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
	for(i=m_vBox.begin(); i!=m_vBox.end(); i++)
	{
		if(!i->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	map<CIdentifier, CComment*>::iterator j;
	for(j=m_vComment.begin(); j!=m_vComment.end(); j++)
	{
		if(!j->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	map<CIdentifier, CLink*>::iterator k;
	for(k=m_vLink.begin(); k!=m_vLink.end(); k++)
	{
		if(!k->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	map<CIdentifier, CProcessingUnit*>::iterator l;
	for(l=m_vProcessingUnit.begin(); l!=m_vProcessingUnit.end(); l++)
	{
		if(!l->second->acceptVisitor(rObjectVisitor))
		{
			return false;
		}
	}

	if(!getKernelContext().getVisualisationManager().getVisualisationTree(m_oVisualisationTreeIdentifier).acceptVisitor(rObjectVisitor))
	{
		return false;
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
	map<CIdentifier, CMessageLink*>::const_iterator l_itMessageLink;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		l_itBox=m_vBox.find(l_oResult);
		l_itComment=m_vComment.find(l_oResult);
		l_itLink=m_vLink.find(l_oResult);
		l_itMessageLink=m_vMessageLink.find(l_oResult);
	}
	while(l_itBox!=m_vBox.end() || l_itComment!= m_vComment.end() || l_itLink!= m_vLink.end() || l_itMessageLink!= m_vMessageLink.end() || l_oResult==OV_UndefinedIdentifier);
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

			if(l_pBox.second->hasAttribute(OVD_AttributeId_SettingOverrideFilename))
			{
				return true;
			}
			l_pBox.second->getSettingName(l_ui32SettingIndex, l_sSettingName);
			l_pBox.second->getSettingValue(l_ui32SettingIndex, l_sRawSettingValue);
			CString l_sSettingValue = l_sRawSettingValue;
			if(pConfig != NULL)
			{
				l_sSettingValue = pConfig->expand(l_sSettingValue);
			}
			else
			{
				l_sSettingValue = this->getConfigurationManager().expandOnlyKeyword("var", l_sSettingValue);
			}
			if( !l_pBox.second->evaluateSettingValue(l_ui32SettingIndex, l_sSettingValue) )
			{
				this->getLogManager() << OpenViBE::Kernel::LogLevel_ImportantWarning << "<" << l_pBox.second->getName() << "> The following value: ["<< l_sRawSettingValue 
					<<"] expanded as ["<< l_sSettingValue <<"] given as setting is not a numeric value.\n";
				return false;
			}
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

		if(!(l_oBoxHashCode1 == OV_UndefinedIdentifier || (l_oBoxHashCode1 != OV_UndefinedIdentifier && l_oBoxHashCode1 == l_oBoxHashCode2)))
		{
			l_bResult = true;
			m_vNeedsUpdatesBoxes.insert(l_pBox);
		}
	}
	
	return l_bResult;
}
