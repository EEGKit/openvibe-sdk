#include "ovkCBuffer.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;

CBuffer::CBuffer(const CBuffer& buffer)
{
	this->CMemoryBuffer::setSize(buffer.getSize(), true);
	memcpy(this->CMemoryBuffer::getDirectPointer(), buffer.getDirectPointer(), buffer.getSize());
}

CBuffer& CBuffer::operator=(const CBuffer& buffer)
{
	this->CMemoryBuffer::setSize(buffer.getSize(), true);
	memcpy(this->CMemoryBuffer::getDirectPointer(), buffer.getDirectPointer(), buffer.getSize());
	return *this;
}
