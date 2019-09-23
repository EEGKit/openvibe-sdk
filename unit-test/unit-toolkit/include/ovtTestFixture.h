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

#include <memory>

namespace OpenViBETest
{
	/**
	* \struct TestFixture
	* \author cgarraud (INRIA)
	* \date 2016-02-08
	* \brief Base abstract struct for test fixture
	*
	* A test fixture is used when an environment has to be set for a specific test.
	* TestFixture implementation should not be used directly but through ScopedTest.
	*/
	struct TestFixture
	{
		virtual ~TestFixture() = default;
		TestFixture()          = default;

		/**
		* \brief Setup resources for the test
		*/
		virtual void setUp() = 0;

		/**
		* \brief Release resources
		*/
		virtual void tearDown() = 0;

	private:

		TestFixture& operator=(const TestFixture&) = delete;
		TestFixture(const TestFixture&)            = delete;
	};

	/**
	* \struct ScopedTest
	* \author cgarraud (INRIA)
	* \date 2016-02-08
	* \brief Class used to ensure RAII when using TestFixture
	*
	* A scoped object is a wrapper around a test fixture used to
	* ensure RAII when running tests.
	*/
	template <typename T>
	struct ScopedTest
	{
		template <typename... TArgs>
		ScopedTest(TArgs&&... args) : fixture(new T(std::forward<TArgs>(args)...)) { fixture->setUp(); }

		~ScopedTest() { fixture->tearDown(); }

		const T* operator->() const { return fixture.get(); }

		T* operator->() { return fixture.get(); }

		std::unique_ptr<T> fixture;
	};
} // namespace OpenViBETest
