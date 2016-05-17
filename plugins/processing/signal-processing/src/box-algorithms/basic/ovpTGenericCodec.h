/*
 * MENSIA TECHNOLOGIES CONFIDENTIAL
 * ________________________________
 *
 *  [2012] - [2013] Mensia Technologies SA
 *  Copyright, All Rights Reserved.
 *
 * NOTICE: All information contained herein is, and remains
 * the property of Mensia Technologies SA.
 * The intellectual and technical concepts contained
 * herein are proprietary to Mensia Technologies SA
 * and are covered copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Mensia Technologies SA.
 */

#ifndef __OpenViBEPlugins_TGenericCodec_H__
#define __OpenViBEPlugins_TGenericCodec_H__

//#include "ovp_common_defines.h"
#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>
#include <map>

namespace OpenViBEPlugins
{

// ______________________________________________________________________________________________________________________________________________________________________________
// ______________________________________________________________________________________________________________________________________________________________________________
//

	#define decoder_return_impl(what) \
		if(m_pSignalDecoder) \
		{ \
			return m_pSignalDecoder->what; \
		} \
		if(m_pSpectrumDecoder) \
		{ \
			return m_pSpectrumDecoder->what; \
		} \
		return m_pStreamedMatrixDecoder->what;

	template < class T >
	class TGenericDecoder
	{
	protected:

		void reset(void)
		{
			m_pStreamedMatrixDecoder=NULL;
			m_pSignalDecoder=NULL;
			m_pSpectrumDecoder=NULL;
		}

	public:

		TGenericDecoder(void)
		{
			this->reset();
		}

		TGenericDecoder<T>& operator=(OpenViBEToolkit::TStreamedMatrixDecoder < T >* pDecoder)
		{
			this->reset();
			m_pStreamedMatrixDecoder=pDecoder;
			return *this;
		}

		TGenericDecoder<T>& operator=(OpenViBEToolkit::TSignalDecoder < T >* pDecoder)
		{
			this->reset();
			m_pSignalDecoder=pDecoder;
			return *this;
		}

		TGenericDecoder<T>& operator=(OpenViBEToolkit::TSpectrumDecoder < T >* pDecoder)
		{
			this->reset();
			m_pSpectrumDecoder=pDecoder;
			return *this;
		}

		void uninitialize(void)
		{
			delete m_pStreamedMatrixDecoder;
			delete m_pSignalDecoder;
			delete m_pSpectrumDecoder;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* >& getOutputMatrix()
		{
			decoder_return_impl(getOutputMatrix());
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getOutputSamplingRate()
		{
			return m_pSignalDecoder->getOutputSamplingRate();
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* >& getOutputMinMaxFrequencyBands()
		{
			return m_pSpectrumDecoder->getOutputMinMaxFrequencyBands();
		}

		OpenViBE::boolean decode(OpenViBE::uint32 ui32ChunkIndex, OpenViBE::boolean bMarkInputAsDeprecated=true)
		{
			decoder_return_impl(decode(ui32ChunkIndex, bMarkInputAsDeprecated));
		}

		OpenViBE::boolean isHeaderReceived(void)
		{
			decoder_return_impl(isHeaderReceived());
		}

		OpenViBE::boolean isBufferReceived(void)
		{
			decoder_return_impl(isBufferReceived());
		}

		OpenViBE::boolean isEndReceived(void)
		{
			decoder_return_impl(isEndReceived());
		}

	protected:

		OpenViBEToolkit::TStreamedMatrixDecoder < T >* m_pStreamedMatrixDecoder;
		OpenViBEToolkit::TSignalDecoder < T >* m_pSignalDecoder;
		OpenViBEToolkit::TSpectrumDecoder < T >* m_pSpectrumDecoder;
	};

// ______________________________________________________________________________________________________________________________________________________________________________
// ______________________________________________________________________________________________________________________________________________________________________________
//

	#define encoder_return_impl(what) \
		if(m_pSignalEncoder) \
		{ \
			return m_pSignalEncoder->what; \
		} \
		if(m_pSpectrumEncoder) \
		{ \
			return m_pSpectrumEncoder->what; \
		} \
		return m_pStreamedMatrixEncoder->what;

	template < class T >
	class TGenericEncoder
	{
	protected:

		void reset(void)
		{
			m_pStreamedMatrixEncoder=NULL;
			m_pSignalEncoder=NULL;
			m_pSpectrumEncoder=NULL;
		}

	public:

		TGenericEncoder(void)
		{
			this->reset();
		}

		TGenericEncoder<T>& operator=(OpenViBEToolkit::TStreamedMatrixEncoder < T >* pEncoder)
		{
			this->reset();
			m_pStreamedMatrixEncoder=pEncoder;
			return *this;
		}

		TGenericEncoder<T>& operator=(OpenViBEToolkit::TSignalEncoder < T >* pEncoder)
		{
			this->reset();
			m_pSignalEncoder=pEncoder;
			return *this;
		}

		TGenericEncoder<T>& operator=(OpenViBEToolkit::TSpectrumEncoder < T >* pEncoder)
		{
			this->reset();
			m_pSpectrumEncoder=pEncoder;
			return *this;
		}

		void uninitialize(void)
		{
			delete m_pStreamedMatrixEncoder;
			delete m_pSignalEncoder;
			delete m_pSpectrumEncoder;
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* >& getInputMatrix()
		{
			encoder_return_impl(getInputMatrix());
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::uint64 >& getInputSamplingRate()
		{
			return m_pSignalEncoder->getInputSamplingRate();
		}

		OpenViBE::Kernel::TParameterHandler < OpenViBE::IMatrix* >& getInputMinMaxFrequencyBands()
		{
			return m_pSpectrumEncoder->getInputMinMaxFrequencyBands();
		}

		OpenViBE::boolean encodeHeader()
		{
			encoder_return_impl(encodeHeader());
		}

		OpenViBE::boolean encodeBuffer()
		{
			encoder_return_impl(encodeBuffer());
		}

		OpenViBE::boolean encodeEnd()
		{
			encoder_return_impl(encodeEnd());
		}

	protected:

		OpenViBEToolkit::TStreamedMatrixEncoder < T >* m_pStreamedMatrixEncoder;
		OpenViBEToolkit::TSignalEncoder < T >* m_pSignalEncoder;
		OpenViBEToolkit::TSpectrumEncoder < T >* m_pSpectrumEncoder;
	};

// ______________________________________________________________________________________________________________________________________________________________________________
// ______________________________________________________________________________________________________________________________________________________________________________
//

	template < class T >
	class TGenericListener : public T
	{
	public:

		typedef enum
		{
			Type_None = 0x00000000,
			Type_StreamedMatrix = 0x01,
			Type_Signal = 0x02,
			Type_Spectrum = 0x04,
			Type_Covariance = 0x08,
			Type_All = 0xffffffff,
		} EType;

		TGenericListener(const OpenViBE::uint32 ui32ValidTypeFlag=Type_All)
		{
			if(ui32ValidTypeFlag & Type_StreamedMatrix) m_vAllowedTypeIdentifier[OV_TypeId_StreamedMatrix] = true;
			if(ui32ValidTypeFlag & Type_Signal) m_vAllowedTypeIdentifier[OV_TypeId_Signal] = true;
			if(ui32ValidTypeFlag & Type_Spectrum) m_vAllowedTypeIdentifier[OV_TypeId_Spectrum] = true;
			if(ui32ValidTypeFlag & Type_Covariance) m_vAllowedTypeIdentifier[OV_TypeId_CovarianceMatrix] = true;
		}

		OpenViBE::boolean isValidInputType(const OpenViBE::CIdentifier& rTypeIdentifier, OpenViBE::uint32 ui32Index)
		{
			return m_vAllowedTypeIdentifier[rTypeIdentifier];
//			return (rTypeIdentifier==OV_TypeId_Signal || rTypeIdentifier==OV_TypeId_Spectrum);
		}

		virtual OpenViBE::boolean onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
		{
			OpenViBE::CIdentifier l_oTypeIdentifier;
			rBox.getInputType(ui32Index, l_oTypeIdentifier);
			if(this->isValidInputType(l_oTypeIdentifier, ui32Index))
			{
				rBox.setOutputType(ui32Index, l_oTypeIdentifier);
			}
			else
			{
				rBox.getOutputType(ui32Index, l_oTypeIdentifier);
				rBox.setInputType(ui32Index, l_oTypeIdentifier);
			}
			return true;
		}

		OpenViBE::boolean isValidOutputType(const OpenViBE::CIdentifier& rTypeIdentifier, OpenViBE::uint32 ui32Index)
		{
			return m_vAllowedTypeIdentifier[rTypeIdentifier];
//			return (rTypeIdentifier==OV_TypeId_Signal || rTypeIdentifier==OV_TypeId_Spectrum);
		}

		virtual OpenViBE::boolean onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const OpenViBE::uint32 ui32Index)
		{
			OpenViBE::CIdentifier l_oTypeIdentifier;
			rBox.getOutputType(ui32Index, l_oTypeIdentifier);
			if(this->isValidOutputType(l_oTypeIdentifier, ui32Index))
			{
				rBox.setInputType(ui32Index, l_oTypeIdentifier);
			}
			else
			{
				rBox.getInputType(ui32Index, l_oTypeIdentifier);
				rBox.setOutputType(ui32Index, l_oTypeIdentifier);
			}
			return true;
		}

		_IsDerivedFromClass_Final_(T, OV_UndefinedIdentifier);

	private:

		std::map < OpenViBE::CIdentifier, OpenViBE::boolean > m_vAllowedTypeIdentifier;
	};
};

#endif // __OpenViBEPlugins_TGenericCodec_H__
