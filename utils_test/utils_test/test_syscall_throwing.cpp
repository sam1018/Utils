#include "stdafx.h"
#include "syscall_throwing.hpp"
#include <windows.h>
#include <system_error>
#include <iostream>

using namespace utils;
using namespace std;

int LoadSomeLibrary()
{
	HMODULE hModule = LoadLibrary(L"SomeLibrary");
	return (hModule != nullptr);
}

class SomeLibrary
{
	HMODULE hModule;
public:
	int LoadSomeLibrary()
	{
		hModule = LoadLibrary(L"SomeLibrary");
		return (hModule != nullptr);
	}
};

void LoadSomeLibrary_ReturnsVoid()
{
	HMODULE hModule = LoadLibrary(L"SomeLibrary");
}

BOOST_AUTO_TEST_CASE(test_syscall_throwing)
{
	// will throw
	try
	{
		syscall_throwing("Failed to load module", LoadLibrary, L"SomeLibrary");
	}
	catch (system_error)
	{
	}

	// system call through some other global function
	try
	{
		syscall_throwing("Failed to load module", LoadSomeLibrary);
	}
	catch (system_error)
	{
	}


	// system call throurh member function
	SomeLibrary someLibrary;
	auto memfn = mem_fn(&SomeLibrary::LoadSomeLibrary);
	try
	{
		syscall_throwing("Failed to load module", memfn, someLibrary);
	}
	catch (system_error)
	{
	}

	// void return type
	try
	{
		syscall_throwing("Failed to load module", LoadSomeLibrary_ReturnsVoid);
	}
	catch (system_error)
	{
	}
}
