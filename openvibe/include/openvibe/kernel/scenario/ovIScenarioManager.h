#ifndef __OpenViBE_Kernel_Scenario_IScenarioManager_H__
#define __OpenViBE_Kernel_Scenario_IScenarioManager_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	class IMemoryBuffer;

	namespace Kernel
	{
		class IScenario;

		/**
		 * \class IScenarioManager
		 * \author Yann Renard (IRISA/INRIA)
		 * \date 2006-10-05
		 * \brief The scenario manager
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 *
		 * This manager is responsible to organize and handle
		 * all the scenarios of the kernel.
		 */
		class OV_API IScenarioManager : public OpenViBE::Kernel::IKernelObject
		{
		public:

			/**
			 * \brief Gets next scenario identifier
			 * \param rPreviousIdentifier [in] : The identifier
			 *        for the preceeding scenario
			 * \return The identifier of the next scenario in case of success.
			 * \return \c OV_UndefinedIdentifier on error.
			 * \note Giving \c OV_UndefinedIdentifier as \c rPreviousIdentifier
			 *       will cause this function to return the first scenario
			 *       identifier.
			 */
			virtual OpenViBE::CIdentifier getNextScenarioIdentifier(
				const OpenViBE::CIdentifier& rPreviousIdentifier) const=0;

			virtual bool isScenario(const OpenViBE::CIdentifier& scenarioIdentifier) const = 0;
			/**
			 * \brief Creates a new scenario
			 * \param rScenarioIdentifier [out] : the identifier of
			 *        the newly created scenario
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean createScenario(
				OpenViBE::CIdentifier& rScenarioIdentifier)=0;

			virtual bool cloneScenarioImportersAndExporters(const IScenarioManager& scenarioManager) = 0;

			virtual bool importScenario(
			        OpenViBE::CIdentifier& newScenarioIdentifier,
			        const OpenViBE::IMemoryBuffer& inputMemoryBuffer,
			        const OpenViBE::CIdentifier& scenarioImporterAlgorithmIdentifier
			        ) = 0;

			virtual bool importScenarioFromFile(
			        OpenViBE::CIdentifier& newScenarioIdentifier,
			        const char* fileName,
			        const OpenViBE::CIdentifier& scenarioImporterAlgorithmIdentifier
			        ) = 0;

			virtual bool importScenarioFromFile(
			        OpenViBE::CIdentifier& newScenarioIdentifier,
			        const OpenViBE::CIdentifier& importContext,
			        const char* fileName) = 0;

			virtual bool registerScenarioImporter(
			        const OpenViBE::CIdentifier& importContext,
			        const char* fileNameExtension,
			        const OpenViBE::CIdentifier& scenarioImporterAlgorithmIdentifier
			        ) = 0;

			virtual bool unregisterScenarioImporter(
			        const OpenViBE::CIdentifier& importContext,
			        const char* fileNameExtension
			        ) = 0;

			virtual OpenViBE::uint32 getRegisteredScenarioImportContextsCount() const = 0;
			virtual OpenViBE::uint32 getRegisteredScenarioImportersCount(const OpenViBE::CIdentifier& importContext) const = 0;

			virtual bool getRegisteredScenarioImportContextDetails(
			        OpenViBE::uint32 index,
			        OpenViBE::CIdentifier& importContext
			        ) const = 0;
			virtual bool getRegisteredScenarioImporterDetails(
			        const OpenViBE::CIdentifier& importContext,
			        OpenViBE::uint32 index,
			        const char** fileNameExtension,
			        OpenViBE::CIdentifier& scenarioImporterAlgorithmIdentifier) const = 0;


			virtual bool exportScenario(
			        OpenViBE::IMemoryBuffer& outputMemoryBuffer,
			        const OpenViBE::CIdentifier& scenarioIdentifier,
			        const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier
			        ) const = 0;

			virtual bool exportScenarioToFile(
			        const char* fileName,
			        const OpenViBE::CIdentifier& scenarioIdentifier,
			        const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier
			        ) const = 0;

			virtual bool exportScenarioToFile(
			        const OpenViBE::CIdentifier& exportContext,
			        const char* fileName,
			        const OpenViBE::CIdentifier& scenarioIdentifier
			        ) = 0;

			virtual bool registerScenarioExporter(
			        const OpenViBE::CIdentifier& exportContext,
			        const char* fileNameExtension,
			        const OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier
			        ) = 0;

			virtual bool unregisterScenarioExporter(
			        const OpenViBE::CIdentifier& exportContext,
			        const char* fileNameExtension
			        ) = 0;

			virtual OpenViBE::uint32 getRegisteredScenarioExportContextsCount() const = 0;
			virtual OpenViBE::uint32 getRegisteredScenarioExportersCount(const OpenViBE::CIdentifier& exportContext) const = 0;

			virtual bool getRegisteredScenarioExportContextDetails(
			        OpenViBE::uint32 index,
			        OpenViBE::CIdentifier& exportContext
			        ) const = 0;
			virtual bool getRegisteredScenarioExporterDetails(
			        const OpenViBE::CIdentifier& exportContext,
			        OpenViBE::uint32 index,
			        const char** fileNameExtension,
			        OpenViBE::CIdentifier& scenarioExporterAlgorithmIdentifier) const = 0;
			/**
			 * \brief Releases an existing scenario
			 * \param rScenarioIdentifier [in] : the existing scenario identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual OpenViBE::boolean releaseScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier)=0;
			/**
			 * \brief Gets details on a specific scenario
			 * \param rScenarioIdentifier [in] : the scenario identifier which details should be returned
			 * \return the corresponding scenario reference.
			 * \warning Calling this function with a bad identifier causes a crash
			 */
			virtual OpenViBE::Kernel::IScenario& getScenario(
				const OpenViBE::CIdentifier& rScenarioIdentifier)=0;

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_ScenarioManager);
		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_IScenarioManager_H__
