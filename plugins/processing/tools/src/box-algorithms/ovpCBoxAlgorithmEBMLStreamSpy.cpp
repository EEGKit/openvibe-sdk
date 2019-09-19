#include "ovpCBoxAlgorithmEBMLStreamSpy.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include <fs/Files.h>

using namespace OpenViBE;
using namespace Kernel;
using namespace Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Tools;
using namespace OpenViBEToolkit;
using namespace std;

CBoxAlgorithmEBMLStreamSpy::CBoxAlgorithmEBMLStreamSpy() {}

void CBoxAlgorithmEBMLStreamSpy::release() { delete this; }

bool CBoxAlgorithmEBMLStreamSpy::initialize()
{
	const IBox& boxContext = getStaticBoxContext();

	m_pReader       = createReader(*this);
	m_pReaderHelper = EBML::createReaderHelper();

	bool expand;
	const CString fileName  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	const uint64_t logLevel = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	if (boxContext.getSettingCount() > 2)
	{
		expand                  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
		m_nExpandValues = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	}
	else
	{
		expand                  = false;
		m_nExpandValues = 4;
	}

	m_eLogLevel = ELogLevel(logLevel);

	ifstream file;
	FS::Files::openIFStream(file, fileName);
	while (file.good() && !file.eof())
	{
		uint32_t id1;
		uint32_t id2;
		string identifier1;
		string identifier2;
		string name;
		string type;

		file >> name;
		file >> identifier1;
		file >> identifier2;
		file >> type;

		sscanf(identifier1.c_str(), "EBML::CIdentifier(0x%08x", &id1);
		sscanf(identifier2.c_str(), "0x%08x)", &id2);

		if (!expand)
		{
			if ((type == "binary(long double)")
				|| (type == "binary(double)")
				|| (type == "binary(float)")
				|| (type == "binary(integer8)")
				|| (type == "binary(integer16)")
				|| (type == "binary(integer32)")
				|| (type == "binary(integer64)")
				|| (type == "binary(uinteger8)")
				|| (type == "binary(uinteger16)")
				|| (type == "binary(uinteger32)")
				|| (type == "binary(uinteger64)")) { type = "binary"; }
		}

		// cout << "[" << l_sIdentifier1 << "][" << l_sIdentifier2 << "]" << endl;
		// printf("[EBML::CIdentifier(0x%08X,][0x%08X]\n", l_ui32Identifier1, l_ui32Identifier2);
		// cout << EBML::CIdentifier(l_ui32Identifier1, l_ui32Identifier2) << endl;

		m_vName[EBML::CIdentifier(id1, id2)] = name;
		m_vType[EBML::CIdentifier(id1, id2)] = type;
	}

	return true;
}

bool CBoxAlgorithmEBMLStreamSpy::uninitialize()
{
	m_pReaderHelper->release();
	m_pReaderHelper = nullptr;

	m_pReader->release();
	m_pReader = nullptr;

	return true;
}

bool CBoxAlgorithmEBMLStreamSpy::isMasterChild(const EBML::CIdentifier& identifier)
{
	const auto n = m_vName.find(identifier);
	const auto t = m_vType.find(identifier);
	if (n != m_vName.end() && t != m_vType.end()) { return (t->second == "master"); }
	return false;
}

void CBoxAlgorithmEBMLStreamSpy::openChild(const EBML::CIdentifier& identifier)
{
	const auto n = m_vName.find(identifier);

	getLogManager() << m_eLogLevel;

	for (size_t i = 0; i <= m_vNodes.size(); i++) { getLogManager() << "  "; }

	getLogManager() << "Opened EBML node [id:" << CIdentifier(identifier) << "]-[name:" << CString(n != m_vName.end() ? n->second.c_str() : "unknown") << "]";

	if (isMasterChild(identifier)) { getLogManager() << "\n"; }

	m_vNodes.push(identifier);
}

template <class T>
void CBoxAlgorithmEBMLStreamSpy::processBinaryBlock(const void* buffer, const uint64_t size)
{
	const uint64_t n = (size / sizeof(T));
	const T* buf     = static_cast<const T*>(buffer);
	for (uint64_t i = 0; i < std::min(m_nExpandValues, n); i++) { getLogManager() << (i == 0 ? "" : " ") << buf[i]; }
	if (m_nExpandValues < n) { getLogManager() << " ..."; }
}

void CBoxAlgorithmEBMLStreamSpy::processChildData(const void* buffer, const uint64_t size)
{
	const auto t = m_vType.find(m_vNodes.top());

	if (t != m_vType.end())
	{
		if (t->second == "uinteger") getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[value:" << m_pReaderHelper->
									 getUIntegerFromChildData(buffer, size) << "]";
		else if (t->second == "integer") getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[value:" << m_pReaderHelper->
										 getSIntegerFromChildData(buffer, size) << "]";
		else if (t->second == "float") getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[value:" << m_pReaderHelper->
									   getFloatFromChildData(buffer, size) << "]";
		else if (t->second == "string") getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[value:" << m_pReaderHelper->
										getASCIIStringFromChildData(buffer, size) << "]";
		else if (t->second == "binary") getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[bytes:" << size << "]";
		else if (t->second == "binary(double)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<double>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(float)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<float>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(integer8)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<int8_t>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(integer16)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<int16_t>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(integer32)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<int>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(integer64)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<int64_t>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(uinteger8)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<uint8_t>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(uinteger16)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<uint16_t>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(uinteger32)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<uint32_t>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(uinteger64)")
		{
			getLogManager() << "-[type:" << CString(t->second.c_str()) << "]-[values:";
			processBinaryBlock<uint64_t>(buffer, size);
			getLogManager() << "]";
		}
		else { getLogManager() << "-[type:" << CString("unknown") << "]-[bytes:" << size << "]"; }
	}
	getLogManager() << "\n";
}

void CBoxAlgorithmEBMLStreamSpy::closeChild() { m_vNodes.pop(); }

bool CBoxAlgorithmEBMLStreamSpy::processInput(const uint32_t /*index*/)
{
	getBoxAlgorithmContext()->markAlgorithmAsReadyToProcess();
	return true;
}

bool CBoxAlgorithmEBMLStreamSpy::process()
{
	IBoxIO& boxContext           = getDynamicBoxContext();
	const IBox& staticBoxContext = getStaticBoxContext();

	uint64_t tStart       = 0;
	uint64_t tEnd         = 0;
	uint64_t size         = 0;
	const uint8_t* buffer = nullptr;

	getLogManager() << m_eLogLevel << "\n";

	for (uint32_t i = 0; i < staticBoxContext.getInputCount(); i++)
	{
		if (boxContext.getInputChunkCount(i))
		{
			CString inputName;
			staticBoxContext.getInputName(i, inputName);

			CIdentifier inputType;
			staticBoxContext.getInputType(i, inputType);

			getLogManager() << m_eLogLevel << "For input " << inputName << " of type " << getTypeManager().getTypeName(inputType) << " :\n";

			for (uint32_t j = 0; j < boxContext.getInputChunkCount(i); j++)
			{
				boxContext.getInputChunk(i, j, tStart, tEnd, size, buffer);
				boxContext.markInputAsDeprecated(i, j);

				getLogManager() << m_eLogLevel << "For chunk [id:" << j << "] at [time:" << CIdentifier(tStart) << "," << CIdentifier(tEnd)
						<< " / " << time64(tStart) << "," << time64(tEnd) << "]\n";

				m_pReader->processData(buffer, size);
			}
		}
	}

	getLogManager() << m_eLogLevel << "\n";

	return true;
}
