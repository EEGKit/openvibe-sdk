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

#pragma once

#include <mutex>
#include <memory>

#include "../ovkTKernelObject.h"

namespace OpenViBE
{
	namespace Kernel
	{
		/**
		 * \class CErrorManager
		 * \author Charles Garraud (Inria)
		 * \date 2016-07-13
		 * \brief Error manager kernel default implementation
		 */
		class CErrorManager final : public TKernelObject<IErrorManager>
		{
		public:

			explicit CErrorManager(const IKernelContext& context);
			~CErrorManager() override;

			void pushError(ErrorType type, const char* description) override;

			void pushErrorAtLocation(ErrorType type, const char* description, const char* filename, size_t line) override;

			void releaseErrors() override;

			bool hasError() const override;

			const IError* getLastError() const override;

			const char* getLastErrorString() const override;

			ErrorType getLastErrorType() const override;

			_IsDerivedFromClass_Final_(TKernelObject<IErrorManager>, OVK_ClassId_Kernel_Error_ErrorManager)

		private:

			mutable std::mutex m_managerGuard;
			std::unique_ptr<IError> m_topError;
		};
	} // namespace Kernel
} // namespace OpenViBE
