#pragma once

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
		CMemoryBuffer();
		/**
		 * \brief Copy constructor
		 *
		 * This constructor builds the internal implementation of this memory buffer and initializes it
		 * with the actual parameter of the constructor as a copy.
		 */
		CMemoryBuffer(const IMemoryBuffer& memoryBuffer);
		/**
		 * \brief Copy constructor
		 *
		 * This constructor builds the internal implementation of this memory buffer and initializes it
		 * with the actual parameter of the constructor as a copy.
		 */
		CMemoryBuffer(const uint8_t* pMemoryBuffer, size_t size);
		/**
		 * \brief Destructor
		 *
		 * The internal implementation is released.
		 */
		~CMemoryBuffer() override;

		//@}
		bool reserve(const size_t size) override;
		bool setSize(const size_t size, const bool discard) override;
		size_t getSize() const override;
		uint8_t* getDirectPointer() override;
		const uint8_t* getDirectPointer() const override;
		bool append(const uint8_t* buffer, const size_t size) override;
		bool append(const IMemoryBuffer& memoryBuffer) override;

		_IsDerivedFromClass_Final_(OpenViBE::IMemoryBuffer, OV_ClassId_MemoryBufferBridge)

	protected:

		IMemoryBuffer* m_impl = nullptr; //!< Internal implementation
	};
} // namespace OpenViBE
