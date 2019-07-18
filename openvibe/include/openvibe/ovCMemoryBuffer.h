#ifndef __OpenViBE_CMemoryBuffer_H__
#define __OpenViBE_CMemoryBuffer_H__

#include "ovIMemoryBuffer.h"

namespace OpenViBE
{
	/**
	 * \class CMemoryBuffer
	 * \author Yann Renard (INRIA/IRISA)
	 * \date 2007-11-21
	 * \brief Basic standalone OpenViBE memory buffer implementation
	 * \ingroup Group_Base
	 *
	 * This class offers a basic standalone impementation of the OpenViBE::IMemoryBuffer
	 * interface. This class can be directly instanciated and used.
	 *
	 * Instances of this class use an internal implementation of the OpenViBE::IMemoryBuffer
	 * interface and redirect their calls to this implementation.
	 */

	class OV_API CMemoryBuffer : public IMemoryBuffer
	{
	public:

		/** \name Constructors */
		//@{

		/**
		 * \brief Default constructor
		 *
		 * This constructor builds the internal implementation of this memory buffer.
		 */
		CMemoryBuffer(void);
		/**
		 * \brief Copy constructor
		 *
		 * This constructor builds the internal implementation of this memory buffer and initializes it
		 * with the actual parameter of the constructor as a copy.
		 */
		CMemoryBuffer(const IMemoryBuffer& rMemoryBuffer);
		/**
		 * \brief Copy constructor
		 *
		 * This constructor builds the internal implementation of this memory buffer and initializes it
		 * with the actual parameter of the constructor as a copy.
		 */
		CMemoryBuffer(const uint8_t* pMemoryBuffer, uint64_t ui64BufferSize);
		/**
		 * \brief Destructor
		 *
		 * The internal implementation is released.
		 */
		virtual ~CMemoryBuffer(void);

		//@}

		virtual bool reserve(
			uint64_t ui64Size);
		virtual bool setSize(
			uint64_t ui64Size,
			bool bDiscard);
		virtual uint64_t getSize(void) const;
		virtual uint8_t* getDirectPointer(void);
		virtual const uint8_t* getDirectPointer(void) const;
		virtual bool append(
			const uint8_t* pBuffer,
			uint64_t ui64BufferSize);
		virtual bool append(
			const IMemoryBuffer& rMemoryBuffer);

		_IsDerivedFromClass_Final_(OpenViBE::IMemoryBuffer, OV_ClassId_MemoryBufferBridge);

	protected:

		IMemoryBuffer* m_pMemoryBufferImpl; //!< Internal implementation
	};
};

#endif // __OpenViBE_CMemoryBuffer_H__
