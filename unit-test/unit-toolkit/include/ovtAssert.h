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

#include <string>
#include <sstream>

namespace OpenViBETest
{
	void printError(const char* expression, const char* message, const char* file, int line);
	void printError(const char* expression, const std::string& message, const char* file, int line);
	void printError(const char* expression, const std::ostream& message, const char* file, int line);
	void printExpressionPair(const char* str1, const char* str2);
	std::string buildExpressionFromPair(const char* str1, const char* str2);
}

/**
* OVT_ASSERT_PRIV: Assess expression and
* return EXIT_FAILURE if expr is false
* - expr: expression to assess
* - origin: original assessed expression
* - msg: custom error message
* WARNING: SHOULD NOT BE USED DIRECTLY
*/
#define OVT_ASSERT_PRIV(expr, origin, msg)                             \
do {                                                                   \
	if (!(expr))                                                       \
	{                                                                  \
		OpenViBETest::printError(#origin, (msg), __FILE__, __LINE__);  \
		return EXIT_FAILURE;                                           \
	}                                                                  \
}                                                                      \
while (0)

/**
* OVT_ASSERT: Assess simple expression
* - expr: expression to assess
* - msg: custom error message
*/
#define OVT_ASSERT(expr, msg) OVT_ASSERT_PRIV((expr), (expr), (msg))

/**
* OVT_ASSERT_STR: Assess string equality
* - str1: reference string
* - str2: compared string
* - msg: custom error message
*/
#define OVT_ASSERT_STREQ(str1, str2, msg)                                \
do {                                                                     \
	if (!((str1) == (str2)))                                             \
	{                                                                    \
		OpenViBETest::printError(                                        \
			OpenViBETest::buildExpressionFromPair(#str1, #str2).c_str(), \
			(msg), __FILE__, __LINE__);                                  \
		OpenViBETest::printExpressionPair((str1).c_str(),(str2).c_str());\
		return EXIT_FAILURE;                                             \
	}                                                                    \
}                                                                        \
while (0)

/**
* OVT_ASSERT_EX: Assess expr throws an
* exception
* - expr: expression to assess
* - msg: custom error message
*/
#define OVT_ASSERT_EX(expr, msg)       \
do {                                   \
	bool hasTrown{ false };            \
	try                                \
	{                                  \
		(expr);                        \
	}                                  \
	catch (...)                        \
	{                                  \
		hasTrown = true;               \
	}                                  \
	OVT_ASSERT_PRIV(hasTrown, (msg));  \
}                                      \
while (0)
