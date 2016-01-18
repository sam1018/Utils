#include "stdafx.h"
#include "CppUnitTest.h"
#include "util_traits.hpp"
#include "is_valid_expression.hpp"
#include <map>
#include <list>
#include <tuple>
#include <string>
#include <vector>
#include <type_traits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;
using namespace utils::traits;

struct val_true
{
	static constexpr bool value = true;
	int y;
};

struct val_false
{
	static constexpr bool value = false;
	int x;
};

struct test_has_intros
{
	int intros;
	int xyz;
};
struct test2
{
	int xyz;
};

struct test_x { int x; };
struct test_y { int y; };
struct test_z { int z; };

IMPLEMENT_EXPR_TEST(has_x, decltype(&T::x), T)
IMPLEMENT_EXPR_TEST(has_y, decltype(&T::y), T)
IMPLEMENT_EXPR_TEST(has_z, decltype(&T::z), T)

struct tag_has_x {};
struct tag_has_y {};
struct tag_has_z {};
struct tag_def   {};

enum class tag_enum
{
	HAS_X,
	HAS_Y,
	HAS_Z,
	BASE_TAG,
	DEF_TAG
};

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<> inline std::wstring ToString<tag_enum>(const tag_enum& val)
			{
				switch (val)
				{
				case tag_enum::HAS_X:
					return L"HAS_X";
				case tag_enum::HAS_Y:
					return L"HAS_Y";
				case tag_enum::HAS_Z:
					return L"HAS_Z";
				case tag_enum::BASE_TAG:
					return L"BASE_TAG";
				case tag_enum::DEF_TAG:
					return L"DEF_TAG";
				default:
					break;
				}

				return L"";
			}
		}
	}
}

template<typename T>
using tags_without_def = get_tag<
	has_x<T>, tag_has_x,
	has_y<T>, tag_has_y,
	has_z<T>, tag_has_z
>;

template<typename T>
using tags_with_def = get_tag<
	has_x<T>, tag_has_x,
	has_y<T>, tag_has_y,
	has_z<T>, tag_has_z,
	tag_def
>;

struct func_tag
{
	auto operator()(...) { return tag_enum::BASE_TAG; }
	auto operator()(tag_def) { return tag_enum::DEF_TAG; }
	auto operator()(tag_has_x) { return tag_enum::HAS_X; }
	auto operator()(tag_has_y) { return tag_enum::HAS_Y; }
	auto operator()(tag_has_z) { return tag_enum::HAS_Z; }
};

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		TEST_METHOD(test_disjunction)
		{
			using type = disjunction<val_true, true_type, val_false, false_type>;
			static_assert(type::value, "");
			type val;
			val.y = 10; // type should be val_true type
			using type2 = disjunction<val_false, false_type>;
			static_assert(!type2::value, "");
		}

		TEST_METHOD(test_conjunction)
		{
			using type = conjunction<val_true, true_type, val_false, false_type>;
			static_assert(!type::value, "");
			type val;
			val.x = 10; // type should be val_false type
			using type2 = conjunction<val_true, true_type>;
			static_assert(type2::value, "");
		}

		TEST_METHOD(test_even_tags)
		{
			func_tag tag_caller;

			Assert::AreEqual(tag_enum::HAS_X, tag_caller(tags_without_def<test_x>()));
			Assert::AreEqual(tag_enum::HAS_Y, tag_caller(tags_without_def<test_y>()));
			Assert::AreEqual(tag_enum::HAS_Z, tag_caller(tags_without_def<test_z>()));
			Assert::AreEqual(tag_enum::BASE_TAG, tag_caller(tags_without_def<string>()));
		}

		TEST_METHOD(test_odd_tags)
		{
			func_tag tag_caller;

			Assert::AreEqual(tag_enum::HAS_X, tag_caller(tags_with_def<test_x>()));
			Assert::AreEqual(tag_enum::HAS_Y, tag_caller(tags_with_def<test_y>()));
			Assert::AreEqual(tag_enum::HAS_Z, tag_caller(tags_with_def<test_z>()));
			Assert::AreEqual(tag_enum::DEF_TAG, tag_caller(tags_with_def<string>()));
		}
	};
}
