#include "stdafx.h"
#include "pp_variadic_transform.hpp"
#include <vector>

using namespace std;

#define ADD(s, data, elem)	BOOST_PP_ADD(data, elem)

BOOST_AUTO_TEST_CASE(test_pp_variadic_transform)
{
	vector<int> v{ UTILS_PP_VARIADIC_TRANSFORM(ADD, 10, 1, 2, 3, 4, 5) };

	vector<int> res{ 11, 12, 13, 14, 15 };

	BOOST_CHECK(res == v);
}
