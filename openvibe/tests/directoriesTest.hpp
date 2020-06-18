///-------------------------------------------------------------------------------------------------
/// 
/// \file directoriesTest.hpp
/// \brief Test Definitions for OpenViBE directories functions (we only launch functions actually).
/// \author Thibaut Monseigne (Inria).
/// \version 1.0.
/// \date 11/05/2020.
/// \copyright <a href="https://choosealicense.com/licenses/agpl-3.0/">GNU Affero General Public License v3.0</a>.
/// 
///-------------------------------------------------------------------------------------------------

#pragma once

#include <gtest/gtest.h>
#include <openvibe/directories.hpp>

//---------------------------------------------------------------------------------------------------
TEST(directories_Tests, directories)
{
	std::cout << "Dist Dir :\t" << OpenViBE::Directories::getDistRootDir() << std::endl;
	std::cout << "Binary Dir :\t" << OpenViBE::Directories::getBinDir() << std::endl;
	std::cout << "Data Dir :\t" << OpenViBE::Directories::getDataDir() << std::endl;
	std::cout << "Library Dir :\t" << OpenViBE::Directories::getLibDir() << std::endl;
	std::cout << "User Home dir :\t" << OpenViBE::Directories::getUserHomeDir() << std::endl;
	std::cout << "User Data Dir :\t" << OpenViBE::Directories::getUserDataDir() << std::endl;
	std::cout << "All User Data dir :\t" << OpenViBE::Directories::getAllUsersDataDir() << std::endl;
	std::cout << "Log dir :\t" << OpenViBE::Directories::getLogDir() << std::endl;
}
//---------------------------------------------------------------------------------------------------
