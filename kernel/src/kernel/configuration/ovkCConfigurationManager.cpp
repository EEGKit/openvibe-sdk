#include "ovkCConfigurationManager.h"

#include <openvibe/kernel/configuration/ovIConfigurationKeywordExpandCallback.h>

#include <fs/IEntryEnumerator.h>
#include <fs/Files.h>

#include <system/ovCTime.h>
#include <system/ovCMath.h>

#include <stack>
#include <string>
#include <fstream>

#include <string>
#include <algorithm>
#include <functional>
#include <cctype>

#include <ctime>

#include <cstdlib>

#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
 #include <unistd.h> // for getpid
#elif defined TARGET_OS_Windows
 #include <windows.h> // for GetCurrentProcessId
#else
#endif

using namespace OpenViBE;
using namespace OpenViBE::Kernel;
using namespace OpenViBE::Plugins;

namespace
{
	// because std::tolower has multiple signatures,
	// it can not be easily used in std::transform
	// this workaround is taken from http://www.gcek.net/ref/books/sw/cpp/ticppv2/
	template <class charT>
	charT to_lower(charT c)
	{
		return std::tolower(c);
	}
};

namespace OpenViBE
{
	namespace Kernel
	{
		class CConfigurationManagerEntryEnumeratorCallBack : public FS::IEntryEnumeratorCallBack
		{
		public:

			CConfigurationManagerEntryEnumeratorCallBack(ILogManager& rLogManager, IConfigurationManager& rConfigurationManger, IErrorManager& rErrorManager)
				:m_rLogManager(rLogManager),
				 m_rConfigurationManager(rConfigurationManger),
				 m_rErrorManager(rErrorManager)
			{
			}

			static std::string reduce(const std::string& sValue)
			{
				if(sValue.length()==0)
				{
					return "";
				}

				std::string::size_type i=0;
				std::string::size_type j=sValue.length()-1;

				while(i<sValue.length() && (sValue[i]=='\t' || sValue[i]==' ')) i++;
				while(j>=i              && (sValue[j]=='\t' || sValue[j]==' ')) j--;

				return sValue.substr(i, j-i+1);
			}

			virtual FS::boolean callback(
				FS::IEntryEnumerator::IEntry& rEntry,
				FS::IEntryEnumerator::IAttributes& rAttributes)
			{
				std::ifstream l_oFile;
				FS::Files::openIFStream(l_oFile, rEntry.getName());

				OV_ERROR_UNLESS(
					l_oFile.good(),
					"Could not open file " << CString(rEntry.getName()),
					ErrorType::ResourceNotFound,
					false,
					m_rErrorManager,
					m_rLogManager
				);

				m_rLogManager << LogLevel_Trace << "Processing configuration file " << CString(rEntry.getName()) << "\n";

				do
				{
					std::string l_sLine;
					std::string l_sLinePart;
					std::string l_sLineTokenName;
					std::string l_sLineTokenValue;
					std::string::size_type eq;

					while(!l_oFile.eof() && (l_sLine.length()==0 || l_sLine[l_sLine.length()-1]=='\\'))
					{
						while(l_sLine.length()!=0 && l_sLine[l_sLine.length()-1]=='\\')
						{
							l_sLine.resize(l_sLine.length()-1); // removes ending backslashes
						}

						std::getline(l_oFile, l_sLinePart, '\n');

						l_sLine+=reduce(l_sLinePart);
					}

					if (l_sLine.size() > 0)
					{
						switch(l_sLine[0])
						{
							case '\0': // empty line
								break;
							case '#':
								m_rLogManager << LogLevel_Debug << "Ignored comment in configuration file " << CString(rEntry.getName()) << " : " << CString(l_sLine.c_str()) << "\n";
								break;
							default :
								OV_ERROR_UNLESS(
									(eq=l_sLine.find("=")) != std::string::npos,
									"Invalid syntax in configuration file " << CString(rEntry.getName()) << " : " << CString(l_sLine.c_str()),
									ErrorType::BadFileParsing,
									false,
									m_rErrorManager,
									m_rLogManager
								);
								std::string l_sTokenName(reduce(l_sLine.substr(0, eq)));
								std::string l_sTokenValue(reduce(l_sLine.substr(eq+1, l_sLine.length()-eq)));
								if(l_sTokenName=="Include")
								{
									CString l_sWildcard=m_rConfigurationManager.expand(l_sTokenValue.c_str());
									m_rLogManager << LogLevel_Trace << "Including configuration file " << l_sWildcard << "...\n";
									m_rConfigurationManager.addConfigurationFromFile(l_sWildcard);
									m_rLogManager << LogLevel_Trace << "Including configuration file " << l_sWildcard << " done...\n";
								}
								else
								{
									CIdentifier l_oTokenIdentifier=m_rConfigurationManager.lookUpConfigurationTokenIdentifier(l_sTokenName.c_str());
									if(l_oTokenIdentifier==OV_UndefinedIdentifier)
									{
										m_rLogManager << LogLevel_Trace << "Adding configuration token " << CString(l_sTokenName.c_str()) << " : " << CString(l_sTokenValue.c_str()) << "\n";
										m_rConfigurationManager.createConfigurationToken(l_sTokenName.c_str(), l_sTokenValue.c_str());
									}
									else
									{
										m_rLogManager << LogLevel_Trace << "Changing configuration token " << CString(l_sTokenName.c_str()) << " to " << CString(l_sTokenValue.c_str()) << "\n";

										// warning if base token are overwritten here
										OV_WARNING_UNLESS(
											l_sTokenName != "Path_UserData" &&
											l_sTokenName != "Path_Log" &&
											l_sTokenName != "Path_Tmp" &&
											l_sTokenName != "Path_Lib" &&
											l_sTokenName != "Path_Bin" &&
											l_sTokenName != "OperatingSystem" &&
											l_sTokenName != "Kernel_PluginsPatternMacOS" &&
											l_sTokenName != "Kernel_PluginsPatternLinux" &&
											l_sTokenName != "Kernel_PluginsPatternWindows" &&
											l_sTokenName != "Kernel_Plugins" &&
											l_sTokenName != "Kernel_PluginsPatternLinux",
											"Overwriting critical token " << l_sTokenName.c_str(),
											m_rLogManager
										);

										m_rConfigurationManager.setConfigurationTokenValue(l_oTokenIdentifier, l_sTokenValue.c_str());
									}
								}
								break;
						}
					}
#if 0 // Might not be necessary as the no-new-line at the end of file case is now handled properly
					else
					{
						OV_ERROR_UNLESS(
							l_sLine == "",
							"Unexpected end of file in configuration file " << CString(rEntry.getName()),
							ErrorType::BadFileParsing,
							false,
							m_rErrorManager,
							m_rLogManager
						);
					}
#endif
				}
				while(!l_oFile.eof());

				m_rLogManager << LogLevel_Trace << "Processing configuration file " << CString(rEntry.getName()) << " finished\n";

				return true;
			}

		protected:

			ILogManager& m_rLogManager;
			IConfigurationManager& m_rConfigurationManager;
			IErrorManager& m_rErrorManager;
		};
	};
};

//#define boolean OpenViBE::boolean

CConfigurationManager::CConfigurationManager(const IKernelContext& rKernelContext, IConfigurationManager* pParentConfigurationManager)
	:TKernelObject<IConfigurationManager>(rKernelContext)
	,m_pParentConfigurationManager(pParentConfigurationManager)
{
	m_ui32Index=0;
	m_ui32StartTime=System::Time::getTime();
}

void CConfigurationManager::clear(void)
{
	m_vConfigurationToken.clear();
}

OpenViBE::boolean CConfigurationManager::addConfigurationFromFile(
	const CString& rFileNameWildCard)
{
	this->getLogManager() << LogLevel_Trace << "Adding configuration file(s) [" << rFileNameWildCard << "]\n";


	boolean l_bResult;
	CConfigurationManagerEntryEnumeratorCallBack l_rCB(getKernelContext().getLogManager(), *this, getKernelContext().getErrorManager());
	FS::IEntryEnumerator* l_pEntryEnumerator=FS::createEntryEnumerator(l_rCB);
	l_bResult=l_pEntryEnumerator->enumerate(rFileNameWildCard);
	l_pEntryEnumerator->release();
	return l_bResult;
}

// ----------------------------------------------------------------------------------------------------------------------------
//

CIdentifier CConfigurationManager::createConfigurationToken(
	const CString& rConfigurationTokenName,
	const CString& rConfigurationTokenValue)
{
	OV_ERROR_UNLESS_KRF(
		this->lookUpConfigurationTokenIdentifier(rConfigurationTokenName, false) == OV_UndefinedIdentifier,
		"Configuration token name " << rConfigurationTokenName << " already exists",
		ErrorType::BadResourceCreation
	);

	CIdentifier l_oIdentifier=this->getUnusedIdentifier();
	m_vConfigurationToken[l_oIdentifier].m_sConfigurationName=rConfigurationTokenName;
	m_vConfigurationToken[l_oIdentifier].m_sConfigurationValue=rConfigurationTokenValue;
	return l_oIdentifier;
}

OpenViBE::boolean CConfigurationManager::releaseConfigurationToken(
	const CIdentifier& rConfigurationTokenIdentifier)
{
	std::map < CIdentifier, SConfigurationToken >::iterator itConfigurationToken=m_vConfigurationToken.find(rConfigurationTokenIdentifier);

	OV_ERROR_UNLESS_KRF(
		itConfigurationToken != m_vConfigurationToken.end(),
		"Configuration token not found " << rConfigurationTokenIdentifier.toString(),
		ErrorType::ResourceNotFound
	);

	m_vConfigurationToken.erase(itConfigurationToken);
	return true;
}

CIdentifier CConfigurationManager::getNextConfigurationTokenIdentifier(
	const CIdentifier& rPreviousConfigurationTokenIdentifier) const
{
	std::map < CIdentifier, SConfigurationToken >::const_iterator itConfigurationToken;

	if(rPreviousConfigurationTokenIdentifier==OV_UndefinedIdentifier)
	{
		itConfigurationToken=m_vConfigurationToken.begin();
	}
	else
	{
		itConfigurationToken=m_vConfigurationToken.find(rPreviousConfigurationTokenIdentifier);
		if(itConfigurationToken==m_vConfigurationToken.end())
		{
			return OV_UndefinedIdentifier;
		}
		itConfigurationToken++;
	}

	return itConfigurationToken!=m_vConfigurationToken.end()?itConfigurationToken->first:OV_UndefinedIdentifier;
}

// ----------------------------------------------------------------------------------------------------------------------------

CString CConfigurationManager::getConfigurationTokenName(
	const CIdentifier& rConfigurationTokenIdentifier) const
{
	std::map < CIdentifier, SConfigurationToken >::const_iterator itConfigurationToken=m_vConfigurationToken.find(rConfigurationTokenIdentifier);
	if(itConfigurationToken!=m_vConfigurationToken.end())
	{
		return itConfigurationToken->second.m_sConfigurationName;
	}
	return "";
}

CString CConfigurationManager::getConfigurationTokenValue(
	const CIdentifier& rConfigurationTokenIdentifier) const
{
	std::map < CIdentifier, SConfigurationToken >::const_iterator itConfigurationToken=m_vConfigurationToken.find(rConfigurationTokenIdentifier);
	if(itConfigurationToken!=m_vConfigurationToken.end())
	{
		return itConfigurationToken->second.m_sConfigurationValue;
	}
	return "";
}

// ----------------------------------------------------------------------------------------------------------------------------

OpenViBE::boolean CConfigurationManager::setConfigurationTokenName(
	const CIdentifier& rConfigurationTokenIdentifier,
	const CString& rConfigurationTokenName)
{
	OV_ERROR_UNLESS_KRF(
		this->lookUpConfigurationTokenIdentifier(rConfigurationTokenName, false) == OV_UndefinedIdentifier,
		"Configuration token name " << rConfigurationTokenName << " already exists",
		ErrorType::BadResourceCreation
	);

	std::map < CIdentifier, SConfigurationToken >::iterator itConfigurationToken=m_vConfigurationToken.find(rConfigurationTokenIdentifier);

	OV_ERROR_UNLESS_KRF(
		itConfigurationToken != m_vConfigurationToken.end(),
		"Configuration token " << rConfigurationTokenIdentifier.toString() << " does not exist",
		ErrorType::BadResourceCreation
	);

	itConfigurationToken->second.m_sConfigurationName=rConfigurationTokenName;
	return true;
}

OpenViBE::boolean CConfigurationManager::setConfigurationTokenValue(
	const CIdentifier& rConfigurationTokenIdentifier,
	const CString& rConfigurationTokenValue)
{
	std::map < CIdentifier, SConfigurationToken >::iterator itConfigurationToken=m_vConfigurationToken.find(rConfigurationTokenIdentifier);

	OV_ERROR_UNLESS_KRF(
		itConfigurationToken != m_vConfigurationToken.end(),
		"Configuration token " << rConfigurationTokenIdentifier.toString() << " does not exist",
		ErrorType::BadResourceCreation
	);

	itConfigurationToken->second.m_sConfigurationValue=rConfigurationTokenValue;
	return true;
}

OpenViBE::boolean CConfigurationManager::addOrReplaceConfigurationToken(
	const CString& rConfigurationTokenName,
	const CString& rConfigurationTokenValue)
{
	const CIdentifier l_oOldIdentifier = this->lookUpConfigurationTokenIdentifier(rConfigurationTokenName, false);
	if(l_oOldIdentifier == OV_UndefinedIdentifier)
	{
		return OV_UndefinedIdentifier != this->createConfigurationToken(rConfigurationTokenName,rConfigurationTokenValue);
	}
	else
	{
		return this->setConfigurationTokenValue(l_oOldIdentifier, rConfigurationTokenValue);
	}
}

// ----------------------------------------------------------------------------------------------------------------------------

CIdentifier CConfigurationManager::lookUpConfigurationTokenIdentifier(
	const CString& rConfigurationTokenName,
	const OpenViBE::boolean bRecursive) const
{
	std::map < CIdentifier, SConfigurationToken >::const_iterator itConfigurationToken=m_vConfigurationToken.begin();
	while(itConfigurationToken!=m_vConfigurationToken.end())
	{
		if(itConfigurationToken->second.m_sConfigurationName==rConfigurationTokenName)
		{
			return itConfigurationToken->first;
		}
		itConfigurationToken++;
	}
	if(bRecursive && m_pParentConfigurationManager)
	{
		return m_pParentConfigurationManager->lookUpConfigurationTokenIdentifier(rConfigurationTokenName, bRecursive);
	}
	return OV_UndefinedIdentifier;
}

CString CConfigurationManager::lookUpConfigurationTokenValue(
	const CString& rConfigurationTokenName) const
{
	std::map < CIdentifier, SConfigurationToken >::const_iterator itConfigurationToken=m_vConfigurationToken.begin();
	while(itConfigurationToken!=m_vConfigurationToken.end())
	{
		if(itConfigurationToken->second.m_sConfigurationName==rConfigurationTokenName)
		{
			return itConfigurationToken->second.m_sConfigurationValue;
		}
		itConfigurationToken++;
	}
	if(m_pParentConfigurationManager)
	{
		return m_pParentConfigurationManager->lookUpConfigurationTokenValue(rConfigurationTokenName);
	}
	return "";
}

// ----------------------------------------------------------------------------------------------------------------------------

OpenViBE::boolean CConfigurationManager::registerKeywordParser(const OpenViBE::CString& rKeyword, const IConfigurationKeywordExpandCallback& rCallback)
{
	OV_ERROR_UNLESS_KRF(
		rKeyword != CString("") && rKeyword != CString("core") && rKeyword != CString("environment"),
		"Trying to overwrite internal keyword " << rKeyword,
		ErrorType::BadResourceCreation
	);

	m_vKeywordOverride[rKeyword] = &rCallback;

	return true;
}

OpenViBE::boolean CConfigurationManager::unregisterKeywordParser(const OpenViBE::CString& rKeyword)
{
	OV_ERROR_UNLESS_KRF(
		m_vKeywordOverride.count(rKeyword),
		"Override for keyword [" << rKeyword << "] was not found",
		ErrorType::ResourceNotFound
	);

	m_vKeywordOverride.erase(rKeyword);

	return true;
}

OpenViBE::boolean CConfigurationManager::unregisterKeywordParser(const IConfigurationKeywordExpandCallback& rCallback)
{
	std::map < OpenViBE::CString, const OpenViBE::Kernel::IConfigurationKeywordExpandCallback*>::iterator l_itOverrideIterator = m_vKeywordOverride.begin();

	OpenViBE::boolean l_bResult = false;
	while (l_itOverrideIterator != m_vKeywordOverride.end())
	{
		if (l_itOverrideIterator->second == &rCallback)
		{
			m_vKeywordOverride.erase(l_itOverrideIterator);
			l_bResult = true;
			break;
		}
		l_itOverrideIterator++;
	}

	OV_ERROR_UNLESS_KRF(
		l_bResult,
		"Override for the callback was not found",
		ErrorType::ResourceNotFound
	);

	return l_bResult;
}

// ----------------------------------------------------------------------------------------------------------------------------

CString CConfigurationManager::expand(
	const CString& rExpression) const
{
	std::string l_sValue(rExpression.toASCIIString());
	std::string l_sResult;
	if(this->internalExpand(l_sValue, l_sResult))
	{
		return l_sResult.c_str();
	}
	return l_sValue.c_str();
}

// ----------------------------------------------------------------------------------------------------------------------------

CIdentifier CConfigurationManager::getUnusedIdentifier(void) const
{
	uint64 l_ui64Identifier=(((uint64)rand())<<32)+((uint64)rand());
	CIdentifier l_oResult;
	std::map < CIdentifier, SConfigurationToken >::const_iterator i;
	do
	{
		l_ui64Identifier++;
		l_oResult=CIdentifier(l_ui64Identifier);
		i=m_vConfigurationToken.find(l_oResult);
	}
	while(i!=m_vConfigurationToken.end() || l_oResult==OV_UndefinedIdentifier);
	return l_oResult;
}

// ----------------------------------------------------------------------------------------------------------------------------

namespace
{
	enum ENodeType
	{
		NodeType_Value,
		NodeType_NamePrefix,
		NodeType_NamePostfix,
	};
};

OpenViBE::boolean CConfigurationManager::internalExpand(const std::string& sValue, std::string& sResult) const
{
	std::stack < std::pair < ENodeType, std::string > > l_vChildren;
	l_vChildren.push(std::make_pair(NodeType_Value, std::string()));

	std::string l_sPrefix;
	std::string l_sPostfix;
	std::string l_sLowerPrefix;
	std::string l_sLowerPostfix;
	std::string l_sValue;
	std::string l_sExpandedValue;

	for(std::string::size_type i=0; i<sValue.length(); i++)
	{
		boolean l_bShouldExpand;

		switch(sValue[i])
		{
			case '$':
				l_vChildren.push(std::make_pair(NodeType_NamePrefix, std::string()));
				break;

			case '{':
				OV_ERROR_UNLESS_KRF(
					l_vChildren.top().first == NodeType_NamePrefix,
					"Could not expand token with syntax error while expanding " << CString(sValue.c_str()),
					ErrorType::BadFileParsing
				);
				l_vChildren.push(std::make_pair(NodeType_NamePostfix, std::string()));
				break;

			case '}':
				OV_ERROR_UNLESS_KRF(
					l_vChildren.top().first == NodeType_NamePostfix,
					"Could not expand token with syntax error while expanding " << CString(sValue.c_str()),
					ErrorType::BadFileParsing
				);
				l_sPostfix=l_vChildren.top().second;
				l_vChildren.pop();

				l_sPrefix=l_vChildren.top().second;
				l_vChildren.pop();

				l_bShouldExpand=true;

				l_sLowerPrefix=l_sPrefix;
				l_sLowerPostfix=l_sPostfix;
				std::transform(l_sLowerPrefix.begin(), l_sLowerPrefix.end(), l_sLowerPrefix.begin(), ::to_lower<std::string::value_type>);
				std::transform(l_sLowerPostfix.begin(), l_sLowerPostfix.end(), l_sLowerPostfix.begin(), ::to_lower<std::string::value_type>);

				if(l_sLowerPrefix=="")
				{
					// l_sValue=this->getConfigurationTokenValue(this->lookUpConfigurationTokenIdentifier(l_sPostfix.c_str()));
					// this->internalGetConfigurationTokenValueFromName(l_sPostfix, l_sValue);
					l_sValue=this->lookUpConfigurationTokenValue(l_sPostfix.c_str()).toASCIIString();
				}
				else if(l_sLowerPrefix=="environment" || l_sLowerPrefix=="env")
				{
					char* l_sEnvValue=::getenv(l_sPostfix.c_str());
					l_sValue=(l_sEnvValue?l_sEnvValue:"");
					l_bShouldExpand=false;
				}
				else if(l_sLowerPrefix=="core")
				{
					char l_sLocalValue[1024];
					if(l_sLowerPostfix=="random")
					{
						sprintf(l_sLocalValue, "%u", this->getRandom());
						l_sValue=l_sLocalValue;
					}
					else if(l_sLowerPostfix=="index")
					{
						sprintf(l_sLocalValue, "%u", this->getIndex());
						l_sValue=l_sLocalValue;
					}
					else if(l_sLowerPostfix=="time")
					{
						l_sValue=this->getTime();
					}
					else if(l_sLowerPostfix=="date")
					{
						l_sValue=this->getDate();
					}
					else if(l_sLowerPostfix=="real-time")
					{
						sprintf(l_sLocalValue, "%u", this->getRealTime());
						l_sValue=l_sLocalValue;
					}
					else if(l_sLowerPostfix=="process-id")
					{
						sprintf(l_sLocalValue, "%u", this->getProcessId());
						l_sValue=l_sLocalValue;
					}
					else
					{
						OV_ERROR_UNLESS_KRF(
							false,
							"Could not expand token with " << CString(l_sPrefix.c_str()) << " prefix and " << CString(l_sPostfix.c_str()) << " postfix while expanding " << CString(sValue.c_str()),
							ErrorType::BadFileParsing
						);
					}
				}
				else
				{
					OV_ERROR_UNLESS_KRF(
						m_vKeywordOverride.count(l_sLowerPrefix.c_str()),
						"Could not expand token with " << CString(l_sPrefix.c_str()) << " prefix while expanding " << CString(sValue.c_str()),
						ErrorType::BadFileParsing
					);

					CString l_sOverridenValue("");

					OV_ERROR_UNLESS_KRF(
						(m_vKeywordOverride.find(l_sLowerPrefix.c_str())->second)->expand(CString(l_sPostfix.c_str()), l_sOverridenValue),
						"Could not expand $" << l_sLowerPrefix.c_str() << "{" << l_sLowerPostfix.c_str() << "}",
						ErrorType::BadFileParsing
					);

					l_sValue = l_sOverridenValue;
				}

				if(l_bShouldExpand)
				{
					OV_ERROR_UNLESS_KRF(
						this->internalExpand(l_sValue, l_sExpandedValue),
						"Could not expand " << CString(l_sValue.c_str()) << " while expanding " << CString(sValue.c_str()),
						ErrorType::BadFileParsing
					);

					l_vChildren.top().second += l_sExpandedValue;
				}
				else
				{
					l_vChildren.top().second += l_sValue;
				}
				break;

			case '\\':
				i++;
				OV_ERROR_UNLESS_KRF(
					i < sValue.length(),
					"Could not expand token with unterminated string while expanding " << CString(sValue.c_str()),
					ErrorType::BadFileParsing
				);

			default:
				l_vChildren.top().second+=sValue[i];
				break;
		}
	}

	OV_ERROR_UNLESS_KRF(
		l_vChildren.size() == 1,
		"Could not expand token with unterminated string while expanding " << CString(sValue.c_str()),
		ErrorType::BadFileParsing
	);

	sResult=l_vChildren.top().second;

	return true;
}

OpenViBE::boolean CConfigurationManager::internalExpandOnlyKeyword(const std::string& sKeyword, const std::string& sValue, std::string& sResult) const
{
	std::stack < std::pair < ENodeType, std::string > > l_vChildren;
	l_vChildren.push(std::make_pair(NodeType_Value, std::string()));

	std::string l_sPrefix;
	std::string l_sPostfix;
	std::string l_sLowerPrefix;
	std::string l_sLowerPostfix;
	std::string l_sValue;
	std::string l_sExpandedValue;

	for(std::string::size_type i=0; i<sValue.length(); i++)
	{
		boolean l_bShouldExpand;

		switch(sValue[i])
		{
			case '$':
				l_vChildren.push(std::make_pair(NodeType_NamePrefix, std::string()));
				break;

			case '{':
				OV_ERROR_UNLESS_KRF(
					l_vChildren.top().first == NodeType_NamePrefix,
					"Could not expand token with syntax error while expanding " << CString(sValue.c_str()),
					ErrorType::BadFileParsing
				);
				l_vChildren.push(std::make_pair(NodeType_NamePostfix, std::string()));
				break;

			case '}':
				OV_ERROR_UNLESS_KRF(
					l_vChildren.top().first == NodeType_NamePostfix,
					"Could not expand token with syntax error while expanding " << CString(sValue.c_str()),
					ErrorType::BadFileParsing
				);

				l_sPostfix=l_vChildren.top().second;
				l_vChildren.pop();

				l_sPrefix=l_vChildren.top().second;
				l_vChildren.pop();

				l_bShouldExpand = true;

				l_sLowerPrefix = l_sPrefix;
				l_sLowerPostfix = l_sPostfix;
				std::transform(l_sLowerPrefix.begin(), l_sLowerPrefix.end(), l_sLowerPrefix.begin(), ::to_lower<std::string::value_type>);
				std::transform(l_sLowerPostfix.begin(), l_sLowerPostfix.end(), l_sLowerPostfix.begin(), ::to_lower<std::string::value_type>);


			/*
				else if(l_sLowerPrefix=="environment" || l_sLowerPrefix=="env")
				{
					char* l_sEnvValue=::getenv(l_sPostfix.c_str());
					l_sValue=(l_sEnvValue?l_sEnvValue:"");
					l_bShouldExpand=false;
				}
				else if(l_sLowerPrefix=="core")
				{
					char l_sLocalValue[1024];
					if(l_sLowerPostfix=="random")
					{
						sprintf(l_sLocalValue, "%u", this->getRandom());
						l_sValue=l_sLocalValue;
					}
					else if(l_sLowerPostfix=="index")
					{
						sprintf(l_sLocalValue, "%u", this->getIndex());
						l_sValue=l_sLocalValue;
					}
					else if(l_sLowerPostfix=="time")
					{
						l_sValue=this->getTime();
					}
					else if(l_sLowerPostfix=="date")
					{
						l_sValue=this->getDate();
					}
					else if(l_sLowerPostfix=="real-time")
					{
						sprintf(l_sLocalValue, "%u", this->getRealTime());
						l_sValue=l_sLocalValue;
					}
					else if(l_sLowerPostfix=="process-id")
					{
						sprintf(l_sLocalValue, "%u", this->getProcessId());
						l_sValue=l_sLocalValue;
					}
					else
					{
						this->getLogManager() << LogLevel_Warning << "Could not expand token with " << CString(l_sPrefix.c_str()) << " prefix and " << CString(l_sPostfix.c_str()) << " postfix while expanding " << CString(sValue.c_str()) << "\n";
						return false;
					}
				}
				else*/
				if (l_sLowerPrefix == sKeyword)
				{
					OV_ERROR_UNLESS_KRF(
						m_vKeywordOverride.count(l_sLowerPrefix.c_str()),
						"Could not expand token with " << CString(l_sPrefix.c_str()) << " prefix while expanding " << CString(sValue.c_str()),
						ErrorType::BadFileParsing
					);

					CString l_sOverridenValue("");

					OV_ERROR_UNLESS_KRF(
						(m_vKeywordOverride.find(l_sLowerPrefix.c_str())->second)->expand(CString(l_sPostfix.c_str()), l_sOverridenValue),
						"Could not expand $" << l_sLowerPrefix.c_str() << "{" << l_sLowerPostfix.c_str() << "}",
						ErrorType::BadFileParsing
					);

					l_sValue = l_sOverridenValue;
				}
				else
				{
					// If the previous token was not something we want to parse we will simply put it back
					l_sValue = "$" + l_sPrefix + "{" + l_sPostfix + "}";
					l_bShouldExpand = false;
				}

				if(l_bShouldExpand)
				{
					OV_ERROR_UNLESS_KRF(
						this->internalExpandOnlyKeyword(sKeyword, l_sValue, l_sExpandedValue),
						"Could not expand " << CString(l_sValue.c_str()) << " while expanding " << CString(sValue.c_str()),
						ErrorType::BadFileParsing
					);

					l_vChildren.top().second += l_sExpandedValue;
				}
				else
				{
					l_vChildren.top().second += l_sValue;
				}
				break;

			case '\\':
				i++;
				OV_ERROR_UNLESS_KRF(
					i < sValue.length(),
					"Could not expand token with unterminated string while expanding " << CString(sValue.c_str()),
					ErrorType::BadFileParsing
				);

			default:
				l_vChildren.top().second+=sValue[i];
				break;
		}
	}

	OV_ERROR_UNLESS_KRF(
		l_vChildren.size() == 1,
		"Could not expand token with unterminated string while expanding " << CString(sValue.c_str()),
		ErrorType::BadFileParsing
	);

	sResult = l_vChildren.top().second;

	return true;
}

OpenViBE::boolean CConfigurationManager::internalGetConfigurationTokenValueFromName(const std::string& sTokenName, std::string& sTokenValue) const
{
	CIdentifier l_oTokenIdentifier=this->lookUpConfigurationTokenIdentifier(sTokenName.c_str(), false);
	if(l_oTokenIdentifier == OV_UndefinedIdentifier)
	{
		OV_ERROR_UNLESS_KRF(
			m_pParentConfigurationManager,
			"Could not expand token [" << CString(sTokenName.c_str()) << "]. This token does not exist. If this is expected behavior, please add \"" << sTokenName.c_str() << " = \" to your configuration file",
			ErrorType::ResourceNotFound
		);

		std::string l_sNewString=std::string("${")+sTokenName+("}");
		sTokenValue=m_pParentConfigurationManager->expand(l_sNewString.c_str());
	}
	else
	{
		sTokenValue=this->getConfigurationTokenValue(l_oTokenIdentifier);
	}
	return true;
}

CString CConfigurationManager::expandOnlyKeyword(
        const CString& rKeyword,
        const CString& rExpression) const
{
	std::string l_sValue(rExpression.toASCIIString());
	std::string l_sResult;
	if(this->internalExpandOnlyKeyword(rKeyword.toASCIIString(), l_sValue, l_sResult))
	{
		return l_sResult.c_str();
	}
	return l_sValue.c_str();
}

float64 CConfigurationManager::expandAsFloat(
	const CString& rExpression,
	const float64 f64FallbackValue) const
{
	CString l_sResult=this->expand(rExpression);
	float64 l_f64Result=0;
	if(sscanf(l_sResult.toASCIIString(), "%lf", &l_f64Result)==1)
	{
		return l_f64Result;
	}

	return f64FallbackValue;
}

int64 CConfigurationManager::expandAsInteger(
	const CString& rExpression,
	const int64 i64FallbackValue) const
{
	CString l_sResult=this->expand(rExpression);
	int64 l_i64Result=0;
	if(sscanf(l_sResult.toASCIIString(), "%lli", &l_i64Result)==1)
	{
		return l_i64Result;
	}

	return i64FallbackValue;
}

uint64 CConfigurationManager::expandAsUInteger(
	const CString& rExpression,
	const uint64 ui64FallbackValue) const
{
	CString l_sResult=this->expand(rExpression);
	uint64 l_ui64Result=0;
	if(sscanf(l_sResult.toASCIIString(), "%llu", &l_ui64Result)==1)
	{
		return l_ui64Result;
	}

	return ui64FallbackValue;
}

OpenViBE::boolean CConfigurationManager::expandAsBoolean(
	const CString& rExpression,
	const OpenViBE::boolean bFallbackValue) const
{
	std::string l_sResult=this->expand(rExpression).toASCIIString();
	std::transform(l_sResult.begin(), l_sResult.end(), l_sResult.begin(), ::to_lower<std::string::value_type>);

	if(l_sResult=="true")  return true;
	if(l_sResult=="on")    return true;
	if(l_sResult=="1")     return true;

	if(l_sResult=="false") return false;
	if(l_sResult=="off")   return false;
	if(l_sResult=="0")     return false;

	return bFallbackValue;
}

uint64 CConfigurationManager::expandAsEnumerationEntryValue(
	const CString& rExpression,
	const CIdentifier& rEnumerationTypeIdentifier,
	const uint64 ui64FallbackValue) const
{
	CString l_sResult=this->expand(rExpression);
	uint64 l_ui64Result=this->getTypeManager().getEnumerationEntryValueFromName(rEnumerationTypeIdentifier, l_sResult);
	if(l_ui64Result!=0xffffffffffffffffll)
	{
		return l_ui64Result;
	}

	return ui64FallbackValue;
}

uint32 CConfigurationManager::getRandom(void) const
{
	return System::Math::randomUInteger32();
}

uint32 CConfigurationManager::getIndex(void) const
{
	return m_ui32Index++;
}

CString CConfigurationManager::getTime(void) const
{
	char l_sResult[1024];
	::time_t l_oRawTime;
	struct tm* l_pTimeInfo;

	::time(&l_oRawTime);
	l_pTimeInfo=::localtime(&l_oRawTime);

	sprintf(l_sResult, "%02i.%02i.%02i", l_pTimeInfo->tm_hour, l_pTimeInfo->tm_min, l_pTimeInfo->tm_sec);
	return l_sResult;
}

CString CConfigurationManager::getDate(void) const
{
	char l_sResult[1024];
	::time_t l_oRawTime;
	struct tm* l_pTimeInfo;

	::time(&l_oRawTime);
	l_pTimeInfo=::localtime(&l_oRawTime);

	sprintf(l_sResult, "%04i.%02i.%02i", l_pTimeInfo->tm_year+1900, l_pTimeInfo->tm_mon+1, l_pTimeInfo->tm_mday);
	return l_sResult;
}

uint32 CConfigurationManager::getRealTime(void) const
{
	return System::Time::getTime()-m_ui32StartTime;
}

uint32 CConfigurationManager::getProcessId(void) const
{
#if defined TARGET_OS_Linux || defined TARGET_OS_MacOS
	return (uint32)getpid();
#elif defined TARGET_OS_Windows
	return (uint32)GetCurrentProcessId();
#else
	#error TODO
#endif
}
