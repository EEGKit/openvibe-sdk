/*********************************************************************
* Software License Agreement (AGPL-3 License)
*
* OpenViBE SDK Test Software
* Based on OpenViBE V1.1.0, Copyright (C) Inria, 2006-2015
* Copyright (C) Inria, 2015-2017,V1.0
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License version 3,
* as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.
* If not, see <http://www.gnu.org/licenses/>.
*/

#include <string>

#include "ovkCErrorManager.h"

using namespace OpenViBE;
using namespace /*OpenViBE::*/Kernel;

// Error manager IError internal implementation
namespace OpenViBE {
namespace Kernel {
class CError final : public IError
{
public:

	CError(ErrorType type, const char* description, IError* nestedError, const char* filename, const size_t line)
		: m_errorType(type), m_nestedError(nestedError), m_description(description), m_location(std::string(filename) + ":" + std::to_string(line)) { }

	~CError() override { }

	const char* getErrorString() const override { return m_description.c_str(); }
	const char* getErrorLocation() const override { return m_location.c_str(); }
	ErrorType getErrorType() const override { return m_errorType; }
	const IError* getNestedError() const override { return m_nestedError.get(); }

	_IsDerivedFromClass_Final_(IError, OVK_ClassId_Kernel_Error_Error)

private:

	ErrorType m_errorType;
	std::unique_ptr<IError> m_nestedError;
	std::string m_description;
	std::string m_location;
};
}  // namespace Kernel
}  // namespace OpenViBE

// Error manager implementation
namespace OpenViBE {
namespace Kernel {
CErrorManager::CErrorManager(const IKernelContext& context) : TKernelObject<IErrorManager>(context), m_topError(nullptr) { }

CErrorManager::~CErrorManager() { this->releaseErrors(); }

void CErrorManager::pushError(const ErrorType type, const char* description) { this->pushErrorAtLocation(type, description, "NoLocationInfo", 0); }

void CErrorManager::pushErrorAtLocation(const ErrorType type, const char* description, const char* filename, const size_t line)
{
	std::lock_guard<std::mutex> lock(m_managerGuard);
	const auto lastTopError = m_topError.release();
	m_topError.reset(new CError(type, description, lastTopError, filename, line));
}

void CErrorManager::releaseErrors()
{
	std::lock_guard<std::mutex> lock(m_managerGuard);
	m_topError.reset(nullptr);
}

bool CErrorManager::hasError() const
{
	std::lock_guard<std::mutex> lock(m_managerGuard);
	return (m_topError != nullptr);
}

const IError* CErrorManager::getLastError() const
{
	std::lock_guard<std::mutex> lock(m_managerGuard);
	return m_topError.get();
}

const char* CErrorManager::getLastErrorString() const
{
	std::lock_guard<std::mutex> lock(m_managerGuard);
	return (m_topError ? m_topError->getErrorString() : "");
}

ErrorType CErrorManager::getLastErrorType() const
{
	std::lock_guard<std::mutex> lock(m_managerGuard);
	return (m_topError ? m_topError->getErrorType() : ErrorType::NoErrorFound);
}
}  // namespace Kernel
}  // namespace OpenViBE
