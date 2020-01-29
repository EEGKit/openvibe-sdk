#include "ovpCBoxAlgorithmEBMLStreamSpy.h"

#include <iostream>
#include <fstream>
#include <algorithm>

#include <fs/Files.h>

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;
using namespace /*OpenViBE::*/Plugins;
using namespace OpenViBEPlugins;
using namespace OpenViBEPlugins::Tools;	//Ambiguous without OpenViBEPlugins
using namespace /*OpenViBE::*/Toolkit;
using namespace std;


bool CBoxAlgorithmEBMLStreamSpy::initialize()
{
	const IBox& boxContext = getStaticBoxContext();

	m_reader = createReader(*this);
	m_helper = EBML::createReaderHelper();

	bool expand;
	const CString fileName  = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 0);
	const uint64_t logLevel = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 1);

	if (boxContext.getSettingCount() > 2)
	{
		expand          = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 2);
		m_nExpandValues = FSettingValueAutoCast(*this->getBoxAlgorithmContext(), 3);
	}
	else
	{
		expand          = false;
		m_nExpandValues = 4;
	}

	m_logLevel = ELogLevel(logLevel);

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
			if ((type == "binary(long double)") || (type == "binary(double)") || (type == "binary(float)")
				|| (type == "binary(integer8)") || (type == "binary(integer16)") || (type == "binary(integer32)")
				|| (type == "binary(integer64)") || (type == "binary(uinteger8)") || (type == "binary(uinteger16)")
				|| (type == "binary(uinteger32)") || (type == "binary(uinteger64)")) { type = "binary"; }
		}

		// cout << "[" << identifier1 << "][" << identifier2 << "]" << endl;
		// printf("[EBML::CIdentifier(0x%08X,][0x%08X]\n", id1, id2);
		// cout << EBML::CIdentifier(id1, id2) << endl;

		m_names[EBML::CIdentifier(id1, id2)] = name;
		m_types[EBML::CIdentifier(id1, id2)] = type;
	}

	return true;
}

bool CBoxAlgorithmEBMLStreamSpy::uninitialize()
{
	m_helper->release();
	m_helper = nullptr;

	m_reader->release();
	m_reader = nullptr;

	return true;
}

bool CBoxAlgorithmEBMLStreamSpy::isMasterChild(const EBML::CIdentifier& identifier)
{
	const auto n = m_names.find(identifier);
	const auto t = m_types.find(identifier);
	if (n != m_names.end() && t != m_types.end()) { return (t->second == "master"); }
	return false;
}

void CBoxAlgorithmEBMLStreamSpy::openChild(const EBML::CIdentifier& identifier)
{
	const auto n = m_names.find(identifier);

	getLogManager() << m_logLevel;

	for (size_t i = 0; i <= m_nodes.size(); ++i) { getLogManager() << "  "; }

	getLogManager() << "Opened EBML node [id:" << identifier << "]-[name:" << (n != m_names.end() ? n->second : "unknown") << "]";

	if (isMasterChild(identifier)) { getLogManager() << "\n"; }

	m_nodes.push(identifier);
}

template <class T>
void CBoxAlgorithmEBMLStreamSpy::processBinaryBlock(const void* buffer, const size_t size)
{
	const size_t n = (size / sizeof(T));
	const T* buf   = static_cast<const T*>(buffer);
	for (size_t i = 0; i < std::min(m_nExpandValues, n); ++i) { getLogManager() << (i == 0 ? "" : " ") << buf[i]; }
	if (m_nExpandValues < n) { getLogManager() << " ..."; }
}

void CBoxAlgorithmEBMLStreamSpy::processChildData(const void* buffer, const size_t size)
{
	const auto t = m_types.find(m_nodes.top());

	if (t != m_types.end())
	{
		if (t->second == "uinteger") { getLogManager() << "-[type:" << t->second << "]-[value:" << m_helper->getUInt(buffer, size) << "]"; }
		else if (t->second == "integer") { getLogManager() << "-[type:" << t->second << "]-[value:" << m_helper->getInt(buffer, size) << "]"; }
		else if (t->second == "float") { getLogManager() << "-[type:" << t->second << "]-[value:" << m_helper->getDouble(buffer, size) << "]"; }
		else if (t->second == "string") { getLogManager() << "-[type:" << t->second << "]-[value:" << m_helper->getStr(buffer, size) << "]"; }
		else if (t->second == "binary") { getLogManager() << "-[type:" << t->second << "]-[bytes:" << size << "]"; }
		else if (t->second == "binary(double)")
		{
			getLogManager() << "-[type:" << t->second << "]-[values:";
			processBinaryBlock<double>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(float)")
		{
			getLogManager() << "-[type:" << t->second << "]-[values:";
			processBinaryBlock<float>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(integer8)")
		{
			getLogManager() << "-[type:" << t->second << "]-[values:";
			processBinaryBlock<int8_t>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(integer16)")
		{
			getLogManager() << "-[type:" << t->second << "]-[values:";
			processBinaryBlock<int16_t>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(integer32)")
		{
			getLogManager() << "-[type:" << t->second << "]-[values:";
			processBinaryBlock<int>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(integer64)")
		{
			getLogManager() << "-[type:" << t->second << "]-[values:";
			processBinaryBlock<int64_t>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(uinteger8)")
		{
			getLogManager() << "-[type:" << t->second << "]-[values:";
			processBinaryBlock<uint8_t>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(uinteger16)")
		{
			getLogManager() << "-[type:" << t->second << "]-[values:";
			processBinaryBlock<uint16_t>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(uinteger32)")
		{
			getLogManager() << "-[type:" << t->second << "]-[values:";
			processBinaryBlock<uint32_t>(buffer, size);
			getLogManager() << "]";
		}
		else if (t->second == "binary(uinteger64)")
		{
			getLogManager() << "-[type:" << t->second << "]-[values:";
			processBinaryBlock<uint64_t>(buffer, size);
			getLogManager() << "]";
		}
		else { getLogManager() << "-[type:unknown]-[bytes:" << size << "]"; }
	}
	getLogManager() << "\n";
}

bool CBoxAlgorithmEBMLStreamSpy::processInput(const size_t /*index*/)
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
	size_t size           = 0;
	const uint8_t* buffer = nullptr;

	getLogManager() << m_logLevel << "\n";

	for (size_t i = 0; i < staticBoxContext.getInputCount(); ++i)
	{
		if (boxContext.getInputChunkCount(i))
		{
			CString inputName;
			staticBoxContext.getInputName(i, inputName);

			CIdentifier inputType;
			staticBoxContext.getInputType(i, inputType);

			getLogManager() << m_logLevel << "For input " << inputName << " of type " << getTypeManager().getTypeName(inputType) << " :\n";

			for (size_t j = 0; j < boxContext.getInputChunkCount(i); ++j)
			{
				boxContext.getInputChunk(i, j, tStart, tEnd, size, buffer);
				boxContext.markInputAsDeprecated(i, j);

				getLogManager() << m_logLevel << "For chunk [id:" << j << "] at [time:" << CIdentifier(tStart) << "," << CIdentifier(tEnd)
						<< " / " << time64(tStart) << "," << time64(tEnd) << "]\n";

				m_reader->processData(buffer, size);
			}
		}
	}

	getLogManager() << m_logLevel << "\n";

	return true;
}
