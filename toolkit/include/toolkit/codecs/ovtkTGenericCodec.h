/*********************************************************************
 * Software License Agreement (AGPL-3 License)
 *
 * OpenViBE SDK
 * Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
 * Copyright (C) Inria, 2015-2017,V1.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License version 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.
 * If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <map>

#include <openvibe/ov_all.h>

#include "../ovtk_base.h"
#include "decoders/ovtkTStreamedMatrixDecoder.h"
#include "decoders/ovtkTSignalDecoder.h"
#include "decoders/ovtkTSpectrumDecoder.h"
#include "decoders/ovtkTFeatureVectorDecoder.h"

#include "encoders/ovtkTStreamedMatrixEncoder.h"
#include "encoders/ovtkTSignalEncoder.h"
#include "encoders/ovtkTSpectrumEncoder.h"
#include "encoders/ovtkTFeatureVectorEncoder.h"

namespace OpenViBEToolkit
{

	// ______________________________________________________________________________________________________________________________________________________________________________
	// ______________________________________________________________________________________________________________________________________________________________________________
	//

#define decoder_return_impl(what) \
		if(m_pSignalDecoder) { return m_pSignalDecoder->what; } \
		if(m_pSpectrumDecoder) { return m_pSpectrumDecoder->what; } \
		if(m_pFeatureVectorDecoder) { return m_pFeatureVectorDecoder->what; } \
		return m_pStreamedMatrixDecoder->what;

	template <class T>
	class TGenericDecoder
	{
	protected:

		void reset()
		{
			m_pStreamedMatrixDecoder = nullptr;
			m_pSignalDecoder         = nullptr;
			m_pSpectrumDecoder       = nullptr;
			m_pFeatureVectorDecoder  = nullptr;
		}

	public:

		TGenericDecoder() : m_pStreamedMatrixDecoder(nullptr), m_pSignalDecoder(nullptr), m_pSpectrumDecoder(nullptr), m_pFeatureVectorDecoder(nullptr) { this->reset(); }

		~TGenericDecoder() { this->uninitialize(); }

		TGenericDecoder<T>& operator=(TStreamedMatrixDecoder<T>* pDecoder)
		{
			this->reset();
			m_pStreamedMatrixDecoder = pDecoder;
			return *this;
		}

		TGenericDecoder<T>& operator=(TSignalDecoder<T>* pDecoder)
		{
			this->reset();
			m_pSignalDecoder = pDecoder;
			return *this;
		}

		TGenericDecoder<T>& operator=(TSpectrumDecoder<T>* pDecoder)
		{
			this->reset();
			m_pSpectrumDecoder = pDecoder;
			return *this;
		}

		TGenericDecoder<T>& operator=(TFeatureVectorDecoder<T>* pDecoder)
		{
			this->reset();
			m_pFeatureVectorDecoder = pDecoder;
			return *this;
		}

		void uninitialize() { this->reset(); }

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*>& getOutputMatrix() { decoder_return_impl(getOutputMatrix()); }

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getOutputSamplingRate()
		{
			if (m_pSignalDecoder) { return m_pSignalDecoder->getOutputSamplingRate(); }
			return m_pSpectrumDecoder->getOutputSamplingRate();
		}

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*>& getOutputFrequencyAbcissa() { return m_pSpectrumDecoder->getOutputFrequencyAbscissa(); }

		bool decode(int, int)                   = delete;
		bool decode(unsigned int, unsigned int) = delete;

		bool decode(uint32_t ui32ChunkIndex, bool bMarkInputAsDeprecated = true) { decoder_return_impl(decode(ui32ChunkIndex, bMarkInputAsDeprecated)); }
		bool isHeaderReceived() { decoder_return_impl(isHeaderReceived()); }
		bool isBufferReceived() { decoder_return_impl(isBufferReceived()); }
		bool isEndReceived() { decoder_return_impl(isEndReceived()); }

	protected:

		TStreamedMatrixDecoder<T>* m_pStreamedMatrixDecoder = nullptr;
		TSignalDecoder<T>* m_pSignalDecoder = nullptr;
		TSpectrumDecoder<T>* m_pSpectrumDecoder = nullptr;
		TFeatureVectorDecoder<T>* m_pFeatureVectorDecoder = nullptr;
	};

	// ______________________________________________________________________________________________________________________________________________________________________________
	// ______________________________________________________________________________________________________________________________________________________________________________
	//

#define encoder_return_impl(what) \
		if(m_pSignalEncoder) { return m_pSignalEncoder->what; } \
		if(m_pSpectrumEncoder) { return m_pSpectrumEncoder->what; } \
		if(m_pFeatureVectorEncoder) { return m_pFeatureVectorEncoder->what; } \
		return m_pStreamedMatrixEncoder->what;

	template <class T>
	class TGenericEncoder
	{
	protected:

		void reset()
		{
			m_pStreamedMatrixEncoder = nullptr;
			m_pSignalEncoder         = nullptr;
			m_pSpectrumEncoder       = nullptr;
			m_pFeatureVectorEncoder  = nullptr;
		}

	public:

		TGenericEncoder() : m_pStreamedMatrixEncoder(nullptr), m_pSignalEncoder(nullptr), m_pSpectrumEncoder(nullptr), m_pFeatureVectorEncoder(nullptr) { this->reset(); }

		~TGenericEncoder() { this->uninitialize(); }

		TGenericEncoder<T>& operator=(TStreamedMatrixEncoder<T>* pEncoder)
		{
			this->reset();
			m_pStreamedMatrixEncoder = pEncoder;
			return *this;
		}

		TGenericEncoder<T>& operator=(TSignalEncoder<T>* pEncoder)
		{
			this->reset();
			m_pSignalEncoder = pEncoder;
			return *this;
		}

		TGenericEncoder<T>& operator=(TSpectrumEncoder<T>* pEncoder)
		{
			this->reset();
			m_pSpectrumEncoder = pEncoder;
			return *this;
		}

		TGenericEncoder<T>& operator=(TFeatureVectorEncoder<T>* pEncoder)
		{
			this->reset();
			m_pFeatureVectorEncoder = pEncoder;
			return *this;
		}

		void uninitialize() { this->reset(); }

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*>& getInputMatrix()
		{
			encoder_return_impl(getInputMatrix());
		}

		OpenViBE::Kernel::TParameterHandler<uint64_t>& getInputSamplingRate()
		{
			if (m_pSignalEncoder) { return m_pSignalEncoder->getInputSamplingRate(); }
			return m_pSpectrumEncoder->getInputSamplingRate();
		}

		OpenViBE::Kernel::TParameterHandler<OpenViBE::IMatrix*>& getInputFrequencyAbcissa() { return m_pSpectrumEncoder->getInputFrequencyAbscissa(); }
		bool encodeHeader() { encoder_return_impl(encodeHeader()); }
		bool encodeBuffer() { encoder_return_impl(encodeBuffer()); }
		bool encodeEnd() { encoder_return_impl(encodeEnd()); }

	protected:

		TStreamedMatrixEncoder<T>* m_pStreamedMatrixEncoder = nullptr;
		TSignalEncoder<T>* m_pSignalEncoder = nullptr;
		TSpectrumEncoder<T>* m_pSpectrumEncoder = nullptr;
		TFeatureVectorEncoder<T>* m_pFeatureVectorEncoder = nullptr;
	};

	// ______________________________________________________________________________________________________________________________________________________________________________
	// ______________________________________________________________________________________________________________________________________________________________________________
	//

	template <class T>
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

		explicit TGenericListener(const uint32_t typeFlag = Type_All)
		{
			if (typeFlag & Type_StreamedMatrix) m_vAllowedTypeIdentifier[OV_TypeId_StreamedMatrix] = true;
			if (typeFlag & Type_Signal) m_vAllowedTypeIdentifier[OV_TypeId_Signal] = true;
			if (typeFlag & Type_Spectrum) m_vAllowedTypeIdentifier[OV_TypeId_Spectrum] = true;
			if (typeFlag & Type_Covariance) m_vAllowedTypeIdentifier[OV_TypeId_CovarianceMatrix] = true;
		}

		bool isValidInputType(const OpenViBE::CIdentifier& rTypeIdentifier, uint32_t /*index*/)
		{
			return m_vAllowedTypeIdentifier[rTypeIdentifier];
			//return (rTypeIdentifier==OV_TypeId_Signal || rTypeIdentifier==OV_TypeId_Spectrum);
		}

		virtual bool onInputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index)
		{
			OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
			rBox.getInputType(index, l_oTypeIdentifier);
			if (this->isValidInputType(l_oTypeIdentifier, index))
			{
				rBox.setOutputType(index, l_oTypeIdentifier);
			}
			else
			{
				rBox.getOutputType(index, l_oTypeIdentifier);
				rBox.setInputType(index, l_oTypeIdentifier);
			}
			return true;
		}

		bool isValidOutputType(const OpenViBE::CIdentifier& rTypeIdentifier, uint32_t /*index*/)
		{
			return m_vAllowedTypeIdentifier[rTypeIdentifier];
			//return (rTypeIdentifier==OV_TypeId_Signal || rTypeIdentifier==OV_TypeId_Spectrum);
		}

		virtual bool onOutputTypeChanged(OpenViBE::Kernel::IBox& rBox, const uint32_t index)
		{
			OpenViBE::CIdentifier l_oTypeIdentifier = OV_UndefinedIdentifier;
			rBox.getOutputType(index, l_oTypeIdentifier);
			if (this->isValidOutputType(l_oTypeIdentifier, index))
			{
				rBox.setInputType(index, l_oTypeIdentifier);
			}
			else
			{
				rBox.getInputType(index, l_oTypeIdentifier);
				rBox.setOutputType(index, l_oTypeIdentifier);
			}
			return true;
		}

		_IsDerivedFromClass_Final_(T, OV_UndefinedIdentifier)

	private:

		std::map<OpenViBE::CIdentifier, bool> m_vAllowedTypeIdentifier;
	};
} // namespace OpenViBEToolkit
