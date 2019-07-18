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
		class OV_API IScenarioManager : public IKernelObject
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
			virtual CIdentifier getNextScenarioIdentifier(
				const CIdentifier& rPreviousIdentifier) const = 0;

			virtual bool isScenario(const CIdentifier& scenarioIdentifier) const = 0;
			/**
			 * \brief Creates a new scenario
			 * \param rScenarioIdentifier [out] : the identifier of
			 *        the newly created scenario
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool createScenario(
				CIdentifier& rScenarioIdentifier) = 0;

			/**
			 * @brief Import a scenario from a memory buffer and insert it in the scenario manager
			 * @param[out] newScenarioIdentifier New identifier of the imported scenario
			 * @param inputMemoryBuffer Buffer to import the scenario from
			 * @param scenarioImporterAlgorithmIdentifier The importer algorithm to use
			 * @retval true In case of success
			 * @retval false In case of failure
			 */
			virtual bool importScenario(
				CIdentifier& newScenarioIdentifier,
				const IMemoryBuffer& inputMemoryBuffer,
				const CIdentifier& scenarioImporterAlgorithmIdentifier
			) = 0;

			/**
			 * @brief Import a scenario from a file and insert it in the scenario manager
			 * @param[out] newScenarioIdentifier New identifier of the imported scenario
			 * @param fileName File to import the scenario from
			 * @param scenarioImporterAlgorithmIdentifier The importer algorithm to use
			 * @retval true In case of success
			 * @retval false In case of failure
			 */
			virtual bool importScenarioFromFile(
				CIdentifier& newScenarioIdentifier,
				const CString& fileName,
				const CIdentifier& scenarioImporterAlgorithmIdentifier
			) = 0;

			/**
			 * @brief Export a scenario to a memory buffer
			 * @param[out] outputMemoryBuffer Buffer to be filled with the serialized scenario
			 * @param scenarioIdentifier Scenario to export
			 * @param scenarioExporterAlgorithmIdentifier Exporter to use
			 * @retval true In case of success
			 * @retval false In case of failure
			 */
			virtual bool exportScenario(
				IMemoryBuffer& outputMemoryBuffer,
				const CIdentifier& scenarioIdentifier,
				const CIdentifier& scenarioExporterAlgorithmIdentifier
			) const = 0;

			/**
			 * @brief Export a scenario to a file
			 * @param outputMemoryBuffer File to which export the scenario
			 * @param scenarioIdentifier Scenario to export
			 * @param scenarioExporterAlgorithmIdentifier Exporter to use
			 * @retval true In case of success
			 * @retval false In case of failure
			 */
			virtual bool exportScenarioToFile(
				const CString& fileName,
				const CIdentifier& scenarioIdentifier,
				const CIdentifier& scenarioExporterAlgorithmIdentifier
			) const = 0;

			/**
			 * \brief Releases an existing scenario
			 * \param rScenarioIdentifier [in] : the existing scenario identifier
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool releaseScenario(
				const CIdentifier& rScenarioIdentifier) = 0;
			/**
			 * \brief Gets details on a specific scenario
			 * \param rScenarioIdentifier [in] : the scenario identifier which details should be returned
			 * \return the corresponding scenario reference.
			 * \warning Calling this function with a bad identifier causes a crash
			 */
			virtual IScenario& getScenario(
				const CIdentifier& rScenarioIdentifier) = 0;


			/** @{
			 * @name Scenario Importers and Exporters
			 *
			 * Scenario importers and exporters permit the Kernel to load and save scenarios from and to files
			 * without having the client to explicitly specify the algorithm identifier for export.
			 *
			 * Both are represented by a data structure that associates a Context with one or more File Name Extensions,
			 * each of the Context/Extension pair has then an Algorithm Identifier associated. This enables the kernel
			 * to use a different importer for different uses.
			 *
			 * For example, in authoring mode one does not need to load the complete scenario, just the descriptor components.
			 *
			 * Importers and exporters can be registered directly by plugins declaring new algorithms.
			 */


			/**
			 * @brief Copies all scenario importers and exporters declarations from a scenarioManager to the current one.
			 * @param scenarioManager The manager to copy the importers from.
			 */
			virtual void cloneScenarioImportersAndExporters(const IScenarioManager& scenarioManager) = 0;

			virtual bool importScenarioFromFile(
				CIdentifier& newScenarioIdentifier,
				const CIdentifier& importContext,
				const CString& fileName) = 0;

			virtual bool registerScenarioImporter(
				const CIdentifier& importContext,
				const CString& fileNameExtension,
				const CIdentifier& scenarioImporterAlgorithmIdentifier
			) = 0;

			virtual bool unregisterScenarioImporter(
				const CIdentifier& importContext,
				const CString& fileNameExtension
			) = 0;

			virtual CIdentifier getNextScenarioImportContext(const CIdentifier& importContext) const = 0;
			virtual CString getNextScenarioImporter(const CIdentifier& importContext, const CString& fileNameExtension) const = 0;
			virtual CIdentifier getScenarioImporterAlgorithmIdentifier(const CIdentifier& importContext, const CString& fileNameExtension) const = 0;

			virtual bool exportScenarioToFile(
				const CIdentifier& exportContext,
				const CString& fileName,
				const CIdentifier& scenarioIdentifier
			) = 0;

			virtual bool registerScenarioExporter(
				const CIdentifier& exportContext,
				const CString& fileNameExtension,
				const CIdentifier& scenarioExporterAlgorithmIdentifier
			) = 0;

			virtual bool unregisterScenarioExporter(
				const CIdentifier& exportContext,
				const CString& fileNameExtension
			) = 0;

			virtual CIdentifier getNextScenarioExportContext(const CIdentifier& exportContext) const = 0;
			virtual CString getNextScenarioExporter(const CIdentifier& exportContext, const CString& fileNameExtension) const = 0;
			virtual CIdentifier getScenarioExporterAlgorithmIdentifier(const CIdentifier& exportContext, const CString& fileNameExtension) const = 0;

			/** @} */

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_ScenarioManager);
		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_IScenarioManager_H__
