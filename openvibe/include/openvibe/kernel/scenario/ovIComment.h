#pragma once

#include "ovIAttributable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IComment
		 * \author Yann Renard INRIA
		 * \date 2010-04-27
		 * \brief A comment class for scenarios
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 *
		 * This interface can be used in order to fully describe an
		 * OpenViBE comment in order to help the understanding of a
		 * given scenario.
		 */
		class OV_API IComment : public IAttributable
		{
		public:

			/** \name Comment naming and identification */
			//@{

			/**
			 * \brief Gets the identifier of this comment
			 * \return The identifier of this OpenViBE comment.
			 */
			virtual CIdentifier getIdentifier(void) const = 0;
			/**
			 * \brief Gets the display name of this comment
			 * \return The name of this OpenViBE comment.
			 */
			virtual CString getText(void) const = 0;
			/**
			 * \brief Changes the identifier of this comment
			 * \param rIdentifier [in] : The new identifier
			 *        this comment should take.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setIdentifier(
				const CIdentifier& rIdentifier) = 0;
			/**
			 * \brief Changes the text of this comment
			 * \param sText [in] : The text this comment should contain
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setText(
				const CString& sText) = 0;

			//@}
			/** \name Initialisation from prototypes etc... */
			//@{

			/**
			 * \brief Initializes the comment from an already existing comment
			 * \param rExisitingComment [in] : The existing box.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Resets the comment and initializes its text/attributes
			 * according to the existing comment.
			 */
			virtual bool initializeFromExistingComment(
				const IComment& rExisitingComment) = 0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IAttributable, OV_ClassId_Kernel_Scenario_Comment)
		};

		typedef IComment IStaticCommentContext;
	};
};


