#include "stdafx.h"
#include "CppUnitTest.h"
#include "is_valid_expression.hpp"
#include <iostream>
#include <vector>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
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

struct tag_base { int int_var; };

IMPLEMENT_EXPR_TEST_WITH_BASE(has_x_with_base, tag_base, decltype(&T::x), T)

IMPLEMENT_EXPR_TEST(has_int_var, decltype(&T::int_var), T)

template<typename T, typename U>
using indirect_test_add_sub = void_t<decltype(declval<T&>() + declval<U&>()), decltype(declval<T&>() - declval<U&>())>;

IMPLEMENT_EXPR_TEST_INDIRECT(can_add_sub, indirect_test_add_sub, T, U)
IMPLEMENT_EXPR_TEST_WITH_BASE_INDIRECT(can_add_sub_with_base, tag_base, indirect_test_add_sub, T, U)

using namespace utils;

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		TEST_METHOD(test_is_valid_expression)
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
			Assert::AreEqual(false, can_stream_insert<std::istream, struct_without_x>::value);

			static_assert(has_int_var<has_x_with_base<struct_with_x>>::value, "");

			static_assert(can_add_sub<int, int>::value, "");
			static_assert(!can_add_sub<string, int>::value, "");

			static_assert(can_add_sub_with_base<int, int>::value, "");
			static_assert(!can_add_sub_with_base<string, int>::value, "");

			static_assert(has_int_var<can_add_sub_with_base<int, int>>::value, "");
		}
	};
}
