// stdafx.cpp : source file that includes just the standard includes
// Boost Unit Test Project.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#define BOOST_TEST_MODULE MyTest

#include "stdafx.h"

#include <boost\filesystem.hpp>

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

std::string get_test_file_full_path(std::string filename)
{
	boost::filesystem::path p;

	p /= TEST_FILES_PATH;
	p /= filename;

	return p.string();
}
