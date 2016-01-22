// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifndef __STDAFX_H__
#define __STDAFX_H__

#ifdef _WIN32
    #include "targetver.h"
#endif

#include <stdio.h>


// TODO: reference additional headers your program requires here


// to run this code with clang with mocrosoft codegen,
// we need to do the following, to fix some compilation issues
#ifdef __clang__
// to fix the following compile error:
// 1>C:\boost_1_60_0\boost / test / utils / basic_cstring / basic_cstring.hpp(64, 29) : error : enumerator value evaluates to 18446744073709551615, which cannot be narrowed to type 'int'[-Wc++11 - narrowing]
#define __IBMCPP__ 600
// to fix the following compile error:
// 1>C:\boost_1_60_0\boost/smart_ptr/detail/sp_counted_base_clang.hpp(29,1): error : cannot mangle this C11 atomic type yet
#define BOOST_SP_USE_STD_ATOMIC
// to fix a compile issue with string pasting with macros:
// 1>  C:\boost_1_60_0\boost/preprocessor/cat.hpp(29,36) :  note: expanded from macro 'BOOST_PP_CAT_I'
#define BOOST_PP_CONFIG_FLAGS() 4
#endif

//Adding required boost header
#include <boost/test/unit_test.hpp>

#ifndef TEST_FILES_PATH
#error Please set test files folder path
#endif

std::string get_test_file_full_path(std::string filename);

#endif // __STDAFX_H__
