#pragma once

#include "../ovIConfigurable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IAlgorithmProto
		 * \brief Prototype interface for algorithm
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-11-21
		 * \ingroup Group_Algorithm
		 * \ingroup Group_Kernel
		 * \ingroup Group_Extend
		 * \sa OpenViBE::Plugins::IAlgorithm
		 * \sa OpenViBE::Plugins::IAlgorithmDesc
		 */
		class OV_API IAlgorithmProto : public IKernelObject
		{
		public:

			/**
			 * \brief Adds an input parameter
			 * \param inputParameterID [in] : the identifier for this parameter
			 * \param sInputName [in] : the name for this parameter
			 * \param eParameterType [in] : the type for this parameter
			 * \param subTypeID [in] : the optional sub type of this parameter (e.g. for enumerations)
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \sa IParameter
			 */
			virtual bool addInputParameter(const CIdentifier& inputParameterID, const CString& sInputName, EParameterType eParameterType,
										   const CIdentifier& subTypeID = OV_UndefinedIdentifier) = 0;
			/**
			 * \brief Adds an output parameter
			 * \param outputParameterID [in] : the identifier for this parameter
			 * \param sOutputName [in] : the name for this parameter
			 * \param eParameterType [in] : the type for this parameter
			 * \param subTypeID [in] : the optional sub type of this parameter (e.g. for enumerations)
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \sa IParameter
			 */
			virtual bool addOutputParameter(const CIdentifier& outputParameterID, const CString& sOutputName, EParameterType eParameterType,
											const CIdentifier& subTypeID = OV_UndefinedIdentifier) = 0;
			/**
			 * \brief Adds an input trigger
			 * \param inputTriggerID [in] : the identifier for this trigger
			 * \param rInputTriggerName [in] : the name for this trigger
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool addInputTrigger(const CIdentifier& inputTriggerID, const CString& rInputTriggerName) = 0;
			/**
			 * \brief Adds an output trigger
			 * \param outputTriggerID [in] : the identifier for this trigger
			 * \param rOutputTriggerName [in] : the name for this trigger
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool addOutputTrigger(const CIdentifier& outputTriggerID, const CString& rOutputTriggerName) = 0;

			_IsDerivedFromClass_(IKernelObject, OV_ClassId_Kernel_Algorithm_AlgorithmProto)
		};
	} // namespace Kernel
} // namespace OpenViBE
