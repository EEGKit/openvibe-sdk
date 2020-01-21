#include "ovkCBuffer.h"

#include <system/ovCMemory.h>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;

CBuffer::CBuffer(const CBuffer& buffer)
{
	this->CMemoryBuffer::setSize(buffer.getSize(), true);
	System::Memory::copy(this->CMemoryBuffer::getDirectPointer(), buffer.getDirectPointer(), buffer.getSize());
}

CBuffer& CBuffer::operator=(const CBuffer& buffer)
{
	this->CMemoryBuffer::setSize(buffer.getSize(), true);
	System::Memory::copy(this->CMemoryBuffer::getDirectPointer(), buffer.getDirectPointer(), buffer.getSize());
	return *this;
}
