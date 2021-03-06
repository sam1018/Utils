// stdafx.cpp : source file that includes just the standard includes
// Boost Unit Test Project.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#define BOOST_TEST_MODULE MyTest
#define _CRT_SECURE_NO_WARNINGS

#include "stdafx.h"
#include <stdlib.h>
#include <string>
#include <exception>
#include <boost/filesystem.hpp>

#ifndef _MSC_VER
#include <boost/test/included/unit_test.hpp>
#endif

using namespace std;
using namespace boost::filesystem;


#if defined(_WIN32)

#include <Windows.h>
std::string get_exe_full_path()
{
	char pBuf[MAX_PATH + 1];
	int bytes = GetModuleFileNameA(NULL, pBuf, MAX_PATH);
	return pBuf;
}

#else

#include <unistd.h>
std::string get_exe_full_path()
{
	char buff[PATH_MAX + 1];
	ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff));

	if(len == -1)
		throw logic_error("Unable to get executable path.");

	buff[len] = '\0';
	return buff;
}

#endif

string get_default_test_files_dir_path()
{
	path p(get_exe_full_path());
	p.remove_filename();
	p /= "../../test_files/";
	if (!exists(p))
		throw logic_error("test_files path not found. Please set TEST_FILES_PATH environment variable.");

	return p.string();
}

string get_test_files_path()
{
	auto c_res = getenv("TEST_FILES_PATH");

	return (c_res ? c_res : get_default_test_files_dir_path());
}

string get_test_file_full_path(const string& filename)
{
	path p(get_test_files_path());
	p /= filename;

	if (!exists(p))
		throw logic_error(p.string() + ": don't exist. Did you set TEST_FILES_PATH environment variable correctly?");

	return p.string();
}
