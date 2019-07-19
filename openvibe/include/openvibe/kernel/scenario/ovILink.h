#pragma once

#include "ovIAttributable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class ILink
		 * \author Yann Renard (IRISA/INRIA)
		 * \date 2006-08-16
		 * \brief Link information between OpenViBE box
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 *
		 * This class collects informations between OpenViBE
		 * boxes : box identifiers and input / output indices.
		 */
		class OV_API ILink : public IAttributable
		{
		public:

			/** \name Identification */
			//@{

			/**
			 * \brief Initialize link from existing link by copying members
			 * \param link  the existing link
			 * @return true
			 */
			virtual bool InitializeFromExistingLink(const ILink& link) = 0;
			
			/**
			 * \brief Changes this link's identifier
			 * \param rIdentifier [in] : The new identifier
			 *        this link should have
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setIdentifier(
				const CIdentifier& rIdentifier) = 0;
			/**
			 * \brief Gets the identifier of this link
			 * \return The identifier of this link.
			 */
			virtual CIdentifier getIdentifier(void) const = 0;

			//@}
			/** \name Source / Target management */
			//@{

			/**
			 * \brief Sets the source of this link
			 * \param rBoxIdentifier [in] : The identifier of the
			 *        source box
			 * \param ui32BoxOutputIndex [in] : The index of the
			 *        output to use on the source box
			 * \param rBoxOutputIdentifier [in] : The identifier of the
			 *        output to use on the source box
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setSource(
				const CIdentifier& rBoxIdentifier,
				uint32_t ui32BoxOutputIndex,
				CIdentifier rBoxOutputIdentifier) = 0;
			/**
			 * \brief Sets the target of this link
			 * \param rBoxIdentifier [in] : The identifier of the
			 *        target box
			 * \param ui32BoxInputIndex [in] : The index of the
			 *        input to use on the target box
			 * \param rBoxInputIdentifier [in] : The identifier of the
			 *        input to use on the target box
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setTarget(
				const CIdentifier& rBoxIdentifier,
				uint32_t ui32BoxInputIndex,
				CIdentifier rBoxInputIdentifier) = 0;
			/**
			 * \brief Gets the source information for this link
			 * \param rBoxIdentifier [out] : The identifier of
			 *        of the source box
			 * \param ui32BoxOutputIndex [out] : The output index
			 *        of the source box
			 * \param ui32BoxOutputIdentifier [out] : The output identifier
			 *        of the target box
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool getSource(
				CIdentifier& rBoxIdentifier,
				uint32_t& ui32BoxOutputIndex,
				CIdentifier& rBoxOutputIdentifier) const = 0;
			/**
			 * \brief Gets the source box identifier for this link
			 * \return the source box identifier for thit link
			 */
			virtual CIdentifier getSourceBoxIdentifier(void) const = 0;
			/**
			 * \brief Gets the source box output index for this link
			 * \return the source box output index for this link
			 */
			virtual uint32_t getSourceBoxOutputIndex(void) const = 0;
			/**
			 * \brief Gets the source box output index for this link
			 * \return the source box output index for this link
			 */
			virtual CIdentifier getSourceBoxOutputIdentifier(void) const = 0;
			/**
			 * \brief Gets the target information for this link
			 * \param rTargetBoxIdentifier [out] : The identifier
			 *        of the target box
			 * \param ui32BoxInputIndex [out] : The input index
			 *        of the target box
			 * \param ui32BoxInputIdentifier [out] : The input identifier
			 *        of the target box
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool getTarget(
				CIdentifier& rTargetBoxIdentifier,
				uint32_t& ui32BoxInputIndex,
				CIdentifier& rBoxInputIdentifier) const = 0;
			/**
			 * \brief Gets the target box identifier for this link
			 * \return the target box identifier for this link
			 */
			virtual CIdentifier getTargetBoxIdentifier(void) const = 0;
			/**
			 * \brief Gets the target box input index for this link
			 * \return the target box input index for this link
			 */
			virtual uint32_t getTargetBoxInputIndex(void) const = 0;			
			/**
			 * \brief Gets the target box input identifier for this link
			 * \return the target box input identifier for this link
			 */
			virtual CIdentifier getTargetBoxInputIdentifier(void) const = 0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Scenario_Link)
		};
	};
};


