#include "ovpCChannelUnitsDecoder.h"

#include <system/ovCMemory.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace StreamCodecs;

// ________________________________________________________________________________________________________________
//

bool CChannelUnitsDecoder::initialize()
{
	CStreamedMatrixDecoder::initialize();

	op_bDynamic.initialize(getOutputParameter(OVP_Algorithm_ChannelUnitsStreamDecoder_OutputParameterId_Dynamic));

	return true;
}

bool CChannelUnitsDecoder::uninitialize()
{
	op_bDynamic.uninitialize();

	CStreamedMatrixDecoder::uninitialize();

	return true;
}

// ________________________________________________________________________________________________________________
//

bool CChannelUnitsDecoder::isMasterChild(const EBML::CIdentifier& identifier)
{
	if (identifier == OVTK_NodeId_Header_ChannelUnits) { return true; }
	if (identifier == OVTK_NodeId_Header_ChannelUnits_Dynamic) { return false; }
	return CStreamedMatrixDecoder::isMasterChild(identifier);
}

void CChannelUnitsDecoder::openChild(const EBML::CIdentifier& identifier)
{
	m_vNodes.push(identifier);

	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_ChannelUnits)
		|| (l_rTop == OVTK_NodeId_Header_ChannelUnits_Dynamic)
	) { }
	else { CStreamedMatrixDecoder::openChild(identifier); }
}

void CChannelUnitsDecoder::processChildData(const void* buffer, const uint64_t size)
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_ChannelUnits)
		|| (l_rTop == OVTK_NodeId_Header_ChannelUnits_Dynamic)
	)
	{
		if (l_rTop == OVTK_NodeId_Header_ChannelUnits_Dynamic) { op_bDynamic = (m_pEBMLReaderHelper->getUIntegerFromChildData(buffer, size) ? true : false); }

		//if(l_rTop==OVTK_NodeId_Header_ChannelUnits_MeasurementUnit_Unit)    op_pMeasurementUnits->getBuffer()[m_ui32UnitIdx*2  ]=m_pEBMLReaderHelper->getFloatFromChildData(buffer, size);
		//if(l_rTop==OVTK_NodeId_Header_ChannelUnits_MeasurementUnit_Factor)  op_pMeasurementUnits->getBuffer()[m_ui32UnitIdx*2+1]=m_pEBMLReaderHelper->getFloatFromChildData(buffer, size);
	}
	else { CStreamedMatrixDecoder::processChildData(buffer, size); }
}

void CChannelUnitsDecoder::closeChild()
{
	EBML::CIdentifier& l_rTop = m_vNodes.top();

	if ((l_rTop == OVTK_NodeId_Header_ChannelUnits)
		|| (l_rTop == OVTK_NodeId_Header_ChannelUnits_Dynamic)
	)
	{
		//if(l_rTop==OVTK_NodeId_Header_ChannelUnits_MeasurementUnit)
		//{
		//	m_ui32UnitIdx++;
		//}
	}
	else { CStreamedMatrixDecoder::closeChild(); }

	m_vNodes.pop();
}
