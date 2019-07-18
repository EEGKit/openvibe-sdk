#include "ovkCBuffer.h"

#include <system/ovCMemory.h>

using namespace OpenViBE;
using namespace Kernel;

CBuffer::CBuffer(const CBuffer& rBuffer)
{
	this->setSize(rBuffer.getSize(), true);
	System::Memory::copy(this->getDirectPointer(), rBuffer.getDirectPointer(), rBuffer.getSize());
}

CBuffer& CBuffer::operator=(
	const CBuffer& rBuffer)
{
	this->setSize(rBuffer.getSize(), true);
	System::Memory::copy(this->getDirectPointer(), rBuffer.getDirectPointer(), rBuffer.getSize());
	return *this;
}
