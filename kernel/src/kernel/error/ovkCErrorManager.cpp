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
using namespace Kernel;

// Error manager IError internal implementation
namespace OpenViBE
{
	namespace Kernel
	{
		class CError final : public IError
		{
		public:

			CError(ErrorType type, const char* description, IError* nestedError, const char* filename, unsigned int line) : m_ErrorType(type),
																															m_NestedError(nestedError),
																															m_Description(description),
																															m_Location(std::string(filename) + ":" + std::to_string(line)) { }

			~CError() { }

			const char* getErrorString() const override
			{
				return m_Description.c_str();
			}

			const char* getErrorLocation() const override
			{
				return m_Location.c_str();
			}

			ErrorType getErrorType() const override
			{
				return m_ErrorType;
			}

			const IError* getNestedError() const override
			{
				return m_NestedError.get();
			}

			_IsDerivedFromClass_Final_(OpenViBE::Kernel::IError, OVK_ClassId_Kernel_Error_Error)

		private:

			ErrorType m_ErrorType;
			std::unique_ptr<IError> m_NestedError;
			std::string m_Description;
			std::string m_Location;
		};
	}
}

// Error manager implementation
namespace OpenViBE
{
	namespace Kernel
	{
		CErrorManager::CErrorManager(const IKernelContext& context) : TKernelObject<IErrorManager>(context), m_TopError(nullptr) { }

		CErrorManager::~CErrorManager()
		{
			this->releaseErrors();
		}

		void CErrorManager::pushError(ErrorType type, const char* description)
		{
			this->pushErrorAtLocation(type, description, "NoLocationInfo", 0);
		}

		void CErrorManager::pushErrorAtLocation(ErrorType type, const char* description, const char* filename, unsigned int line)
		{
			std::lock_guard<std::mutex> lock(m_ManagerGuard);
			auto lastTopError = m_TopError.release();
			m_TopError.reset(new CError(type, description, lastTopError, filename, line));
		}

		void CErrorManager::releaseErrors()
		{
			std::lock_guard<std::mutex> lock(m_ManagerGuard);
			m_TopError.reset(nullptr);
		}

		bool CErrorManager::hasError() const
		{
			std::lock_guard<std::mutex> lock(m_ManagerGuard);
			return (m_TopError != nullptr);
		}

		const IError* CErrorManager::getLastError() const
		{
			std::lock_guard<std::mutex> lock(m_ManagerGuard);
			return m_TopError.get();
		}

		const char* CErrorManager::getLastErrorString() const
		{
			std::lock_guard<std::mutex> lock(m_ManagerGuard);
			return (m_TopError ? m_TopError->getErrorString() : "");
		}

		ErrorType CErrorManager::getLastErrorType() const
		{
			std::lock_guard<std::mutex> lock(m_ManagerGuard);
			return (m_TopError ? m_TopError->getErrorType() : ErrorType::NoErrorFound);
		}
	}
}
