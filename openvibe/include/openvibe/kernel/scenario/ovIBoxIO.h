#ifndef __OpenViBE_Kernel_Scenario_IBoxIO_H__
#define __OpenViBE_Kernel_Scenario_IBoxIO_H__

#include "../ovIKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class IBoxIO
		 * \author Yann Renard (INRIA/IRISA)
		 * \date 2006-06-21
		 * \brief Main OpenViBE box interface to communicate with the kernel
		 * \ingroup Group_Scenario
		 * \ingroup Group_Kernel
		 *
		 * This context allows the object which uses it to check
		 * inputs states, read them, write outputs while being
		 * in a dynamic context (used by player).
		 */
		class OV_API IBoxIO : public IKernelObject
		{
		public:

			/** \name Accessing the input chunks informations */
			//@{

			/**
			 * \brief Gets the number of pending chunks for an input
			 * \param ui32InputIndex [in] : the input to work on.
			 * \return The number of pending chunks for an input
			 */
			virtual uint32_t getInputChunkCount(
				uint32_t ui32InputIndex) const = 0;
			/**
			 * \brief Gets an input chunk and its time validity.
			 * \param ui32InputIndex [in] : the index of the desired input.
			 * \param ui32ChunkIndex [in] : the index of the desired
			 *        chunk in this input.
			 * \param rStartTime [out] : the time which the chunk starts at
			 * \param rEndTime [out] : the time which the chunk ends at
			 * \param rChunkSize [out] : the chunk buffer size in bytes
			 * \param rpChunkBuffer [out] : the chunk data itself
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \warning When returning \e false, none of the value
			 *          \c rStartTime, \c rEndTime, \c rChunkSize nor
			 *          \c rpChunkBuffer are defined.
			 * \warning The chunks are ordered like they arrived
			 *          to the box, this means chunk 0 arrived
			 *          before chunk 1, that arrived before
			 *          chunk 2 and so on...
			 * \note Both time value are given in fixed point 32:32 seconds
			 * \sa getInputChunkCount
			 * \sa markInputAsDeprecated
			 */
			virtual bool getInputChunk(
				uint32_t ui32InputIndex,
				uint32_t ui32ChunkIndex,
				uint64_t& rStartTime,
				uint64_t& rEndTime,
				uint64_t& rChunkSize,
				const uint8_t*& rpChunkBuffer) const = 0;
			/**
			 * \brief Gets an input chunk.
			 * \param ui32InputIndex [in] : the index of the desired input.
			 * \param ui32ChunkIndex [in] : the index of the desired
			 *        chunk in this input.
			 * \return The memory buffer for the specified chunk.
			 * \return \c NULL in case of error.
			 * \warning The chunks are ordered like they arrived
			 *          to the box, this means chunk 0 arrived
			 *          before chunk 1, that arrived before
			 *          chunk 2 and so on...
			 * \sa getInputChunkCount
			 * \sa markInputAsDeprecated
			 */
			virtual const IMemoryBuffer* getInputChunk(
				uint32_t ui32InputIndex,
				uint32_t ui32ChunkIndex) const = 0;
			/**
			 * \brief Gets an input chunk start time.
			 * \param ui32InputIndex [in] : the index of the desired input.
			 * \param ui32ChunkIndex [in] : the index of the desired
			 *        chunk in this input.
			 * \return The input chunk start time.
			 * \return \c 0 is returned in case of error.
			 * \note Both time value are given in fixed point 32:32 seconds
			 * \sa getInputChunkCount
			 * \sa markInputAsDeprecated
			 */
			virtual uint64_t getInputChunkStartTime(
				uint32_t ui32InputIndex,
				uint32_t ui32ChunkIndex) const = 0;
			/**
			 * \brief Gets an input chunk end time.
			 * \param ui32InputIndex [in] : the index of the desired input.
			 * \param ui32ChunkIndex [in] : the index of the desired
			 *        chunk in this input.
			 * \return The input chunk end time.
			 * \return \c 0 is returned in case of error.
			 * \note Both time value are given in fixed point 32:32 seconds
			 * \sa getInputChunkCount
			 * \sa markInputAsDeprecated
			 */
			virtual uint64_t getInputChunkEndTime(
				uint32_t ui32InputIndex,
				uint32_t ui32ChunkIndex) const = 0;
			/**
			 * \brief Marks an input chunk as deprecated
			 * \param ui32InputIndex [in] : the index of the desired input.
			 * \param ui32ChunkIndex [in] : the index of the chunk to mark.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \warning The chunks are ordered like they arrived
			 *          to the box, this means chunk 0 arrived
			 *          before chunk 1, that arrived before
			 *          chunk 2 and so on...
			 *
			 * This function discards a chunk when it's been read
			 * and processed. This allows the kernel to know a chunk
			 * has been treated successfully and will not be used
			 * any more so it can be removed from memory.
			 *
			 * \sa getInputChunkCount
			 * \sa releaseChunk
			 */
			virtual bool markInputAsDeprecated(
				uint32_t ui32InputIndex,
				uint32_t ui32ChunkIndex) = 0;

			//@}
			/** \name Accessing the output chunks information */
			//@{

			/**
			 * \brief Gets the output chunk size
			 * \param ui32OutputIndex [in] : the index of the output to work on
			 * \return The output chunk size in bytes
			 */
			virtual uint64_t getOutputChunkSize(
				uint32_t ui32OutputIndex) const = 0;
			/**
			 * \brief Sets an output chunk size
			 * \param ui32OutputIndex [in] : the index of the output to work on
			 * \param ui64Size [in] : the new size of the output chunk
			 * \param bDiscard [in] : tells if existing buffer should be discarded or not
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 */
			virtual bool setOutputChunkSize(
				uint32_t ui32OutputIndex,
				uint64_t ui64Size,
				bool bDiscard = true) = 0;
			/**
			 * \brief Gets a pointer to the current output chunk buffer
			 * \param ui32OutputIndex [in] : the index of the output to work on
			 * \return A pointer to the current output chunk buffer
			 * \warning The returned pointer may change if
			 *          the caller resizes the chunk buffer
			 *          using \c setChunkSize !
			 */
			virtual uint8_t* getOutputChunkBuffer(
				uint32_t ui32OutputIndex) = 0;
			/**
			 * \brief Appends data to the output chunk
			 * \param ui32OutputIndex [in] : the index of the output to work on
			 * \param pBuffer [in] : the buffer to append to the current buffer
			 * \param ui64BufferSize [in] : the size of the appended buffer
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 * \warning This function resizes the chunk size
			 *          calling \c setOutputChunkSize and then
			 *          appends data doing a copy.
			 */
			virtual bool appendOutputChunkData(
				uint32_t ui32OutputIndex,
				const uint8_t* pBuffer,
				uint64_t ui64BufferSize) = 0;
			/**
			 * \brief Gets an output chunk.
			 * \param ui32OutputIndex [in] : the index of the desired output.
			 * \return The output chunk memory buffer for the specified output.
			 * \return \c NULL in case of error.
			 * \sa markOutputAsReadyToSend
			 */
			virtual IMemoryBuffer* getOutputChunk(
				uint32_t ui32OutputIndex) = 0;
			/**
			 * \brief Marks output buffer as 'ready to send'
			 * \param ui32OutputIndex [in] : the index of the output to work on
			 * \param ui64StartTime [in] : the start time for
			 *        the related buffer.
			 * \param ui64EndTime [in] : the end time for the
			 *        related buffer.
			 * \return \e true in case of success.
			 * \return \e false in case of error.
			 *
			 * The output chunk should first be filled. For
			 * that, one will have to get a reference on it
			 * thanks to the getChunkBuffer or the
			 * appendOutputChunkData methods ! The
			 * player will then know the buffer can be sent.
			 *
			 * When called, the function flushes the output
			 * chunk. Thus, after the call, the chunk size
			 * is turned to 0 and the output is ready for
			 * a new chunk when necessary...
			 *
			 * \note Both time value are given in fixed point 32:32 seconds
			 * \sa getChunk
			 */
			virtual bool markOutputAsReadyToSend(
				uint32_t ui32OutputIndex,
				uint64_t ui64StartTime,
				uint64_t ui64EndTime) = 0;

			//@}

			_IsDerivedFromClass_(OpenViBE::Kernel::IKernelObject, OV_ClassId_Kernel_Scenario_BoxIO)
		};
	};
};

#endif // __OpenViBE_Kernel_Scenario_IBoxIO_H__
