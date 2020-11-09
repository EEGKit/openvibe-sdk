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

#include <iostream>

#include "ovtAssert.h"

namespace OpenViBE {
namespace Test {
static void printErrorCore(const char* expression, const char* file, const int line)
{
	std::cerr << "Failed to evaluate: " << expression << std::endl;
	std::cerr << "File = " << file << std::endl;
	std::cerr << "Line = " << line << std::endl;
}

void printError(const char* expression, const char* message, const char* file, const int line)
{
	printErrorCore(expression, file, line);
	std::cerr << "Error message: " << message << std::endl;
}

void printError(const char* expression, const std::string& message, const char* file, const int line) { printError(expression, message.c_str(), file, line); }

void printError(const char* expression, const std::ostream& message, const char* file, const int line)
{
	printErrorCore(expression, file, line);
	std::cerr << "Error message: " << message.rdbuf() << std::endl;
}

void printExpressionPair(const char* str1, const char* str2)
{
	std::cerr << "Expression 1 is : " << str1 << std::endl;
	std::cerr << "Expression 2 is : " << str2 << std::endl;
}

std::string buildExpressionFromPair(const char* str1, const char* str2)
{
	std::string expression = "( " + std::string(str1);
	expression += " = ";
	expression += std::string(str2) + " )";

	return expression;
}
}  // namespace Test
}  // namespace OpenViBE
