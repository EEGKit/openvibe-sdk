#include "ovpCBoxAlgorithmVotingClassifier.h"

#include <system/ovCMemory.h>

#include <list>
#include <vector>
#include <string>
#include <algorithm>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;

using namespace OpenViBEPlugins;
using namespace Classification;

bool CBoxAlgorithmVotingClassifier::initialize()
{
	const IBox& boxContext = this->getStaticBoxContext();

	m_oClassificationChoiceEncoder.initialize(*this, 0);

	CIdentifier l_oTypeIdentifier;
	boxContext.getInputType(0, l_oTypeIdentifier);
	m_bMatrixBased = (l_oTypeIdentifier == OV_TypeId_StreamedMatrix);

	for (uint32_t i = 0; i < boxContext.getInputCount(); i++)
	{
		SInput& l_rInput = m_vClassificationResults[i];
		if (m_bMatrixBased)
		{
			OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmVotingClassifier>* l_pDecoder = new OpenViBEToolkit::TStreamedMatrixDecoder<CBoxAlgorithmVotingClassifier>();
			l_pDecoder->initialize(*this, i);
			l_rInput.m_pDecoder = l_pDecoder;

			l_rInput.op_pMatrix = l_pDecoder->getOutputMatrix();

			l_rInput.m_bTwoValueInput = false;
		}
		else
		{
			OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmVotingClassifier>* l_pDecoder = new OpenViBEToolkit::TStimulationDecoder<CBoxAlgorithmVotingClassifier>();
			l_pDecoder->initialize(*this, i);
			l_rInput.m_pDecoder = l_pDecoder;

			l_rInput.op_pStimulationSet = l_pDecoder->getOutputStimulationSet();

			l_rInput.m_bTwoValueInput = false;
		}
	}

	m_ui64NumberOfRepetitions  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	m_ui64TargetClassLabel     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);
	m_ui64NonTargetClassLabel  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
	m_ui64RejectClassLabel     = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	m_ui64ResultClassLabelBase = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 4);
	m_bChooseOneIfExAequo      = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 5);

	m_ui64LastTime = 0;

	m_oClassificationChoiceEncoder.encodeHeader();
	this->getDynamicBoxContext().markOutputAsReadyToSend(0, m_ui64LastTime, this->getPlayerContext().getCurrentTime());

	return true;
}

bool CBoxAlgorithmVotingClassifier::uninitialize()
{
	const size_t nInput = this->getStaticBoxContext().getInputCount();

	for (uint32_t i = 0; i < nInput; i++)
	{
		SInput& l_rInput = m_vClassificationResults[i];
		l_rInput.m_pDecoder->uninitialize();
		delete l_rInput.m_pDecoder;
	}

	m_oClassificationChoiceEncoder.uninitialize();

	return true;
}

bool CBoxAlgorithmVotingClassifier::processInput(const uint32_t index)
{
	this->getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();

	return true;
}

bool CBoxAlgorithmVotingClassifier::process()
{
	IBoxIO& boxContext    = this->getDynamicBoxContext();
	const size_t nInput = this->getStaticBoxContext().getInputCount();

	bool l_bCanChoose = true;

	for (size_t i = 0; i < nInput; i++)
	{
		SInput& l_rInput = m_vClassificationResults[i];
		for (size_t j = 0; j < boxContext.getInputChunkCount(i); j++)
		{
			l_rInput.m_pDecoder->decode(j);

			if (l_rInput.m_pDecoder->isHeaderReceived())
			{
				if (m_bMatrixBased)
				{
					if (l_rInput.op_pMatrix->getBufferElementCount() != 1)
					{
						OV_ERROR_UNLESS_KRF(l_rInput.op_pMatrix->getBufferElementCount() == 2,
											"Invalid input matrix with [" << l_rInput.op_pMatrix->getBufferElementCount() << "] (expected values must be 1 or 2)",
											OpenViBE::Kernel::ErrorType::BadInput);

						this->getLogManager() << LogLevel_Debug << "Input got two dimensions, the value use for the vote will be the difference between the two values\n";
						l_rInput.m_bTwoValueInput = true;
					}
				}
			}
			if (l_rInput.m_pDecoder->isBufferReceived())
			{
				if (m_bMatrixBased)
				{
					double value;
					if (l_rInput.m_bTwoValueInput)
					{
						value = l_rInput.op_pMatrix->getBuffer()[1] - l_rInput.op_pMatrix->getBuffer()[0];
					}
					else
					{
						value = l_rInput.op_pMatrix->getBuffer()[0];
					}
					l_rInput.m_vScore.push_back(std::pair<double, uint64_t>(-value, boxContext.getInputChunkEndTime(i, j)));
				}
				else
				{
					for (uint32_t k = 0; k < l_rInput.op_pStimulationSet->getStimulationCount(); k++)
					{
						const uint64_t stimulationId = l_rInput.op_pStimulationSet->getStimulationIdentifier(k);
						if (stimulationId == m_ui64TargetClassLabel || stimulationId == m_ui64NonTargetClassLabel || stimulationId == m_ui64RejectClassLabel)
						{
							l_rInput.m_vScore.push_back(std::pair<double, uint64_t>(stimulationId == m_ui64TargetClassLabel ? 1 : 0, l_rInput.op_pStimulationSet->getStimulationDate(k)));
						}
					}
				}
			}
			if (l_rInput.m_pDecoder->isEndReceived())
			{
				m_oClassificationChoiceEncoder.encodeEnd();
				boxContext.markOutputAsReadyToSend(0, m_ui64LastTime, this->getPlayerContext().getCurrentTime());
			}
		}

		if (l_rInput.m_vScore.size() < m_ui64NumberOfRepetitions)
		{
			l_bCanChoose = false;
		}
	}

	if (l_bCanChoose)
	{
		double resultScore         = -1E100;
		uint64_t resultClassLabel = m_ui64RejectClassLabel;
		uint64_t time             = 0;

		std::map<uint32_t, double> score;
		for (size_t i = 0; i < nInput; i++)
		{
			SInput& input = m_vClassificationResults[i];
			score[i]      = 0;
			for (size_t j = 0; j < m_ui64NumberOfRepetitions; j++)
			{
				score[i] += input.m_vScore[j].first;
			}

			if (score[i] > resultScore)
			{
				resultScore       = score[i];
				resultClassLabel = m_ui64ResultClassLabelBase + i;
				time             = input.m_vScore[size_t(m_ui64NumberOfRepetitions - 1)].second;
			}
			else if (score[i] == resultScore)
			{
				if (!m_bChooseOneIfExAequo)
				{
					resultScore       = score[i];
					resultClassLabel = m_ui64RejectClassLabel;
					time             = input.m_vScore[size_t(m_ui64NumberOfRepetitions - 1)].second;
				}
			}

			input.m_vScore.erase(input.m_vScore.begin(), input.m_vScore.begin() + int(m_ui64NumberOfRepetitions));

			this->getLogManager() << LogLevel_Debug << "Input " << i << " got score " << score[i] << "\n";
		}

		if (resultClassLabel != m_ui64RejectClassLabel)
		{
			this->getLogManager() << LogLevel_Debug << "Chosen " << this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, resultClassLabel) << " with score " << resultScore << "\n";
		}
		else
		{
			this->getLogManager() << LogLevel_Debug << "Chosen rejection " << this->getTypeManager().getEnumerationEntryNameFromValue(OV_TypeId_Stimulation, resultClassLabel) << "\n";
		}
		m_oClassificationChoiceEncoder.getInputStimulationSet()->clear();
		m_oClassificationChoiceEncoder.getInputStimulationSet()->appendStimulation(resultClassLabel, time, 0);

		m_oClassificationChoiceEncoder.encodeBuffer();
		boxContext.markOutputAsReadyToSend(0, m_ui64LastTime, time);
		m_ui64LastTime = time;
	}

	return true;
}
