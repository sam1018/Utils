#include "stdafx.h"
#include "is_valid_expression.hpp"
#include <iostream>
#include <vector>
#include <type_traits>

using namespace std;

struct struct_with_x
{
	int x;
};

auto& operator>>(std::istream& is, struct_with_x& x)
{
	int val;
	is >> val;
	x.x = val;
	return is;
}

struct struct_without_x
{
	int y;
};

IMPLEMENT_EXPR_TEST(has_x, decltype(&T::x), T)
IMPLEMENT_EXPR_TEST(has_y, decltype(&T::y), T)
IMPLEMENT_EXPR_TEST(can_stream_insert, decltype(declval<stream&>() >> declval<T&>()), stream, T)
IMPLEMENT_EXPR_TEST(has_int_var, decltype(&T::int_var), T)

template<typename T, typename U>
using indirect_test_add_sub = void_t<decltype(declval<T&>() + declval<U&>()), decltype(declval<T&>() - declval<U&>())>;

IMPLEMENT_EXPR_TEST_INDIRECT(can_add_sub, indirect_test_add_sub, T, U)

BOOST_AUTO_TEST_CASE(test_is_valid_expression)
{
	static_assert(has_x<struct_with_x>::value, "");
	static_assert(!has_x<struct_without_x>::value, "");

	static_assert(!has_y<struct_with_x>::value, "");
	static_assert(has_y<struct_without_x>::value, "");

	static_assert(can_stream_insert<std::istream, int>::value, "");
	static_assert(can_stream_insert<std::istream, double>::value, "");
	static_assert(can_stream_insert<std::istream, std::string>::value, "");
	static_assert(can_stream_insert<std::istream, struct_with_x>::value, "");
	//static_assert(can_stream_insert<std::istream, struct_without_x>::value, "");
	BOOST_CHECK(false == (can_stream_insert<std::istream, struct_without_x>::value));

	static_assert(can_add_sub<int, int>::value, "");
	static_assert(!can_add_sub<string, int>::value, "");
}
