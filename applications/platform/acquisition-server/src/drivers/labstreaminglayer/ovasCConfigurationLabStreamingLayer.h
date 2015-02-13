#ifndef __OpenViBE_AcquisitionServer_CConfigurationLabStreamingLayer_H__
#define __OpenViBE_AcquisitionServer_CConfigurationLabStreamingLayer_H__

#include "../ovasCConfigurationBuilder.h"
#include "ovasIDriver.h"
#include "ovasIHeader.h"

#include <gtk/gtk.h>
#include <lsl_cpp.h>

namespace OpenViBEAcquisitionServer
{
	/**
	 * \class CConfigurationLabStreamingLayer
	 * \author Jussi T. Lindgren / Inria
	 * \date Wed Oct 15 09:41:18 2014
	 * \brief The CConfigurationLabStreamingLayer handles the configuration dialog specific to the LabStreamingLayer (LSL) device.
	 *
	 * \sa CDriverLabStreamingLayer
	 */
	class CConfigurationLabStreamingLayer : public OpenViBEAcquisitionServer::CConfigurationBuilder
	{
	public:

		CConfigurationLabStreamingLayer(OpenViBEAcquisitionServer::IDriverContext& rDriverContext, const char* sGtkBuilderFileName, 
			OpenViBEAcquisitionServer::IHeader& rHeader,
			OpenViBE::CString& rSignalStream,
			OpenViBE::CString& rMarkerStream);

		virtual OpenViBE::boolean preConfigure(void);
		virtual OpenViBE::boolean postConfigure(void);

	protected:

		OpenViBEAcquisitionServer::IDriverContext& m_rDriverContext;

	private:

		OpenViBEAcquisitionServer::IHeader& m_rHeader;

		OpenViBE::CString& m_rSignalStream;
		OpenViBE::CString& m_rMarkerStream;

	};
};

#endif // __OpenViBE_AcquisitionServer_CConfigurationLabStreamingLayer_H__
