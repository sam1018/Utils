// stdafx.cpp : source file that includes just the standard includes
// Boost Unit Test Project.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#define BOOST_TEST_MODULE MyTest

#include "stdafx.h"
#include <string>
#include <exception>
#include <boost/filesystem.hpp>

using namespace std;
using namespace boost::filesystem;


string get_env(const string& env)
{
	char *c_s;
	auto ret = _dupenv_s(&c_s, nullptr, env.c_str());
	if (c_s == nullptr)
		throw logic_error("Environment variable "s + env + " is not set.");
	string s(c_s);
	free(c_s);

	return s;
}

string get_test_file_full_path(const string& filename)
{
	path p(get_env("TEST_FILES_PATH"));
	p /= filename;

	if (!exists(p))
		throw logic_error(p.string() + ": don't exist. Did you set TEST_FILES_PATH environment variable correctly?");

	return p.string();
}
