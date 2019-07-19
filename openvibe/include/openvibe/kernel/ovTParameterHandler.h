#pragma once

#include "ovIParameter.h"
#include "ovIConfigurable.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class TParameterHandler
		 * \brief Helper class in order to manipulate IParameter and IParameter values natively
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2007-11-21
		 * \ingroup Group_Kernel
		 * \ingroup Group_Helper
		 * \sa IParameter
		 * \sa EParameterType
		 *
		 * The purpose for this template is to help user code to manipulate
		 * IParameter abstract class and it's associated value. Client code
		 * should probably use this template better than IParameter directly.
		 * The template argument is the handled data type :
		 *  - for \c ParameterType_Integer : \c int64_t
		 *  - for \c ParameterType_UInteger : \c uint64_t
		 *  - for \c ParameterType_Boolean : \c bool
		 *  - for \c ParameterType_Float : \c double
		 *  - for \c ParameterType_String : \c OpenViBE::CString
		 *  - for \c ParameterType_Identifier : \c OpenViBE::CIdentifier
		 *  - for \c ParameterType_Matrix : \c OpenViBE::IMatrix*
		 *  - for \c ParameterType_StimulationSet : \c OpenViBE::IStimulationSet*
		 *  - for \c ParameterType_MemoryBuffer : \c OpenViBE::IMemoryBuffer*
		 *  - for \c ParameterType_Object : \c OpenViBE::IObject*
		 *  - for \c ParameterType_Pointer : \c uint8_t*
		 *
		 * The idea is to connect the handler to the corresponding IParameter object
		 * thanks to the \c initialize function. Then the provided interface translates
		 * handled object native operation to generic IParameter operations automatically.
		 * Moreover, the handler is able to provide the handled object's interface thanks
		 * to an automatic cast operator.
		 */
		template <typename T>
		class TParameterHandler
		{
		public:

			/** \name Construction */
			//@{

			/**
			 * \brief Default construction, does nothing
			 */
			TParameterHandler(void)
				: m_pParameter(NULL) { }

			/**
			 * \brief IParameter based construction
			 * \param pParameter [in] : the parameter to use for the initialization
			 *
			 * This constructor considers the parameter handler to be
			 * connected to the provided parameter. This is equivalent to
			 * using the default constructor and call the initialize function
			 * with the provided IParameter pointer.
			 */
			TParameterHandler(IParameter* pParameter)
				: m_pParameter(pParameter) { }

			//@}
			/** \name Initialization */
			//@{

			/**
			 * \brief Connects this parameter handler to a concrete IParameter pointer
			 * \param pParameter [in] : the concrete parameter to connect to (it can not be \c NULL )
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * Future calls to this handler will be redirected to the
			 * concrete parameter object.
			 */
			bool initialize(IParameter* pParameter)
			{
				if (m_pParameter) { return false; }

				if (!pParameter) { return false; }

				m_pParameter = pParameter;
				return true;
			}

			/**
			 * \brief Disconnects this parameter handler from its concrete IParameter pointer
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * This handler won't be usable until \c initialize is called successfully again.
			 */
			bool uninitialize(void)
			{
				if (!m_pParameter) { return false; }

				m_pParameter = NULL;
				return true;
			}

			/**
			 * \brief Checks whether this handler is connected to a concrete IParameter or not
			 * \return \e true if this handler is connected to a concrete IParameter, \e false if not
			 */
			bool exists(void) const { return m_pParameter != NULL; }

			//@{
			/** \name Transparent operators */
			//@{

			/**
			 * \brief Handled object type cast
			 * \return The handled object itself
			 *
			 * This cast operator allows the handler to be used as if it was the handled
			 * object itself. For example, an unsigned integer parameter handler is usable
			 * in arithmetic operations :
			 *
			 * \code
			 * // l_oParameter will directly be used as if it is an unsigned integer
			 * TParameterHandler<uint64_t> l_oParameter ( ... );
			 * uint64_t a= ...;
			 * uint64_t c=a*l_oParameter+l_oParameter;
			 * \endcode
			 */
			operator T(void) const
			{
				if (!m_pParameter) { throw; }
				T t;
				m_pParameter->getValue(&t);
				return t;
			}
			/**
			 * \brief Handled object type member access operator
			 * \return The handled object itself for access operation
			 *
			 * This operator allows the handler to be used as if it was the handled
			 * object itself. For example, a matrix parameter handler is usable
			 * as if it was a matrix pointer :
			 *
			 * \code
			 * // l_oParameter will directly be used as if it is a matrix pointer
			 * TParameterHandler<IMatrix*> l_oParameter ( ... );
			 * l_oParameter->setDimensionCount(2);
			 * \endcode
			 */
			T operator ->(void) const
			{
				if (!m_pParameter) { throw; }
				T t;
				m_pParameter->getValue(&t);
				return t;
			}
			/**
			 * \brief Affectation operator
			 * \return This handler itself
			 *
			 * This operator allows to affect handled object type values to the
			 * handled object itself. For example, an unsigned integer parameter
			 * handler can be affected this way :
			 *
			 * \code
			 * // the affectation of l_oParameter will directly go to its handled unsigned integer
			 * TParameterHandler<uint64_t> l_oParameter ( ... );
			 * l_oParameter = 1;
			 * \endcode
			 */
			TParameterHandler<T>& operator=(const T& t)
			{
				if (!m_pParameter) { throw; }
				m_pParameter->setValue(&t);
				return *this;
			}

			//@}
			/** \name Reference target management */
			//@{

			/**
			 * \brief Clears all reference targets
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			bool clearReferenceTarget(void)
			{
				return m_pParameter ? m_pParameter->clearReferenceTarget() : false;
			}
			/**
			 * \brief Sets a new reference target
			 * \param rParameterHandler [in] : a parameter handler of the same type as this parameter handler
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			bool setReferenceTarget(TParameterHandler<T>& rParameterHandler)
			{
				return m_pParameter && rParameterHandler.m_pParameter ? m_pParameter->setReferenceTarget(rParameterHandler.m_pParameter) : false;
			}
			/**
			 * \brief Sets a new reference target
			 * \param pParameter [in] : a parameter to use as reference target
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			bool setReferenceTarget(IParameter* pParameter)
			{
				return m_pParameter && pParameter ? m_pParameter->setReferenceTarget(pParameter) : false;
			}
			/**
			 * \brief Sets a new reference target
			 * \param t [in] : a value to use as reference target
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			bool setReferenceTarget(T& t)
			{
				return m_pParameter ? m_pParameter->setReferenceTarget(&t) : false;
			}

			//@}

		protected:

			IParameter* m_pParameter; //!< Handled parameter
		};
	};
};


