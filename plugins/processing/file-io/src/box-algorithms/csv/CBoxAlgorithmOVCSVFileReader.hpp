/*********************************************************************
 * Software License Agreement (AGPL-3 License)
 *
 * \file CBoxAlgorithmOVCSVFileReader.hpp
 * \brief Classes of the box CSV File Reader
 * \author Victor Herlin (Mensia)
 * \version 1.1.0
 * \date Fri May 7 16:40:49 2021.
 *
 * \copyright (C) 2006-2021 INRIA
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

#include "../../ovp_defines.h"
#include <cstdio>
#include <memory>

#include <openvibe/ov_all.h>
#include <toolkit/ovtk_all.h>

#include "csv/ovICSV.h"

namespace OpenViBE {
namespace Plugins {
namespace FileIO {
class CBoxAlgorithmOVCSVFileReader final : public Toolkit::TBoxAlgorithm<IBoxAlgorithm>
{
public:

	CBoxAlgorithmOVCSVFileReader() : m_readerLib(CSV::createCSVHandler(), CSV::releaseCSVHandler) { }
	void release() override { delete this; }
	uint64_t getClockFrequency() override { return 128LL << 32; }
	bool initialize() override;
	bool uninitialize() override;
	bool processClock(Kernel::CMessageClock& msg) override;
	bool process() override;

	_IsDerivedFromClass_Final_(Toolkit::TBoxAlgorithm<IBoxAlgorithm>, OVP_ClassId_BoxAlgorithm_OVCSVFileReader)

private:
	bool processChunksAndStimulations();
	bool processStimulation(double startTime, double endTime);

	std::unique_ptr<CSV::ICSVHandler, decltype(&CSV::releaseCSVHandler)> m_readerLib;

	Toolkit::TGenericEncoder<CBoxAlgorithmOVCSVFileReader> m_algorithmEncoder;
	Toolkit::TStimulationEncoder<CBoxAlgorithmOVCSVFileReader> m_stimEncoder;

	std::deque<CSV::SMatrixChunk> m_savedChunks;
	std::deque<CSV::SStimulationChunk> m_savedStimulations;

	uint64_t m_lastStimulationDate = 0;

	CIdentifier m_typeID = CIdentifier::undefined();
	std::vector<std::string> m_channelNames;
	std::vector<size_t> m_dimSizes;
	size_t m_sampling         = 0;
	size_t m_nSamplePerBuffer = 0;

	bool m_isHeaderSent            = false;
	bool m_isStimulationHeaderSent = false;
	std::vector<double> m_frequencyAbscissa;
};

class CBoxAlgorithmOVCSVFileReaderListener final : public Toolkit::TBoxListener<IBoxListener>
{
public:
	bool onOutputTypeChanged(Kernel::IBox& box, const size_t index) override
	{
		CIdentifier typeID = CIdentifier::undefined();
		box.getOutputType(index, typeID);

		if (index == 0 && typeID == OV_TypeId_Stimulations)
		{
			OV_ERROR_UNLESS_KRF(box.setOutputType(index, OV_TypeId_Signal), "Failed to reset output type to signal", Kernel::ErrorType::Internal);
		}
		else if (index == 1 && typeID != OV_TypeId_Stimulations)
		{
			OV_ERROR_UNLESS_KRF(box.setOutputType(index, OV_TypeId_Stimulations), "Failed to reset output type to stimulations", Kernel::ErrorType::Internal);
		}
		else if (index > 1) { OV_ERROR_UNLESS_KRF(false, "The index of the output does not exist", Kernel::ErrorType::Internal); }

		return true;
	}

	_IsDerivedFromClass_Final_(Toolkit::TBoxListener<IBoxListener>, CIdentifier::undefined())
};

class CBoxAlgorithmOVCSVFileReaderDesc final : virtual public IBoxAlgorithmDesc
{
public:
	void release() override { }
	CString getName() const override { return CString("CSV File Reader"); }
	CString getAuthorName() const override { return CString("Victor Herlin"); }
	CString getAuthorCompanyName() const override { return CString("Mensia Technologies SA"); }
	CString getShortDescription() const override { return CString("Read signal in a CSV (text based) file"); }
	CString getDetailedDescription() const override { return CString(""); }
	CString getCategory() const override { return CString("File reading and writing/CSV"); }
	CString getVersion() const override { return CString("1.2"); }
	CString getSoftwareComponent() const override { return CString("openvibe-sdk"); }
	CString getAddedSoftwareVersion() const override { return CString("0.1.0"); }
	CString getUpdatedSoftwareVersion() const override { return CString("0.3.3"); }
	CIdentifier getCreatedClass() const override { return OVP_ClassId_BoxAlgorithm_OVCSVFileReader; }
	IPluginObject* create() override { return new CBoxAlgorithmOVCSVFileReader; }
	IBoxListener* createBoxListener() const override { return new CBoxAlgorithmOVCSVFileReaderListener; }
	void releaseBoxListener(IBoxListener* listener) const override { delete listener; }

	bool getBoxPrototype(Kernel::IBoxProto& BoxAlgorithmPrototype) const override
	{
		BoxAlgorithmPrototype.addOutput("Output stream", OV_TypeId_Signal);
		BoxAlgorithmPrototype.addOutput("Output stimulation", OV_TypeId_Stimulations);
		BoxAlgorithmPrototype.addSetting("Filename", OV_TypeId_Filename, "");

		BoxAlgorithmPrototype.addFlag(Kernel::BoxFlag_CanModifyOutput);

		BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Signal);
		BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Spectrum);
		BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_FeatureVector);
		BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_StreamedMatrix);
		BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_CovarianceMatrix);
		BoxAlgorithmPrototype.addOutputSupport(OV_TypeId_Stimulations);
		return true;
	}

	_IsDerivedFromClass_Final_(IBoxAlgorithmDesc, OVP_ClassId_BoxAlgorithm_OVCSVFileReaderDesc)
};
}  // namespace FileIO
}  // namespace Plugins
}  // namespace OpenViBE
