#include "stdafx.h"
#include "CppUnitTest.h"
#include "intros_ptree.hpp"
#include <map>
#include <list>
#include <string>
#include <vector>
#include <boost\property_tree\json_parser.hpp>

using namespace std;
using namespace boost::property_tree;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace utils::intros_ptree;

struct test_multiple_items
{
	int x;
	string s;
	double d;
	bool b;
};

bool operator==(const test_multiple_items& lhs, const test_multiple_items& rhs)
{
	return
		lhs.x == rhs.x &&
		lhs.s == rhs.s &&
		lhs.d == rhs.d &&
		lhs.b == rhs.b;
}

BEGIN_INTROS_TYPE(test_multiple_items)
	ADD_INTROS_ITEM(x)
	ADD_INTROS_ITEM(s)
	ADD_INTROS_ITEM(d)
	ADD_INTROS_ITEM(b)
END_INTROS_TYPE(test_multiple_items)

struct test_multiple_items_diff_names
{
	int x;
	string s;
	double d;
	bool b;
};

bool operator==(const test_multiple_items_diff_names& lhs, const test_multiple_items_diff_names& rhs)
{
	return
		lhs.x == rhs.x &&
		lhs.s == rhs.s &&
		lhs.d == rhs.d &&
		lhs.b == rhs.b;
}

BEGIN_INTROS_TYPE_USER_NAME(test_multiple_items_diff_names, "test_another_name")
	ADD_INTROS_ITEM(x)
	ADD_INTROS_ITEM_USER_NAME(s, "string_name_different")
	ADD_INTROS_ITEM(d)
	ADD_INTROS_ITEM(b)
END_INTROS_TYPE(test_multiple_items_diff_names)

struct test_multiple_items_diff_scope
{
	int x;
	string s;
	double d;
	bool b;
};

bool operator==(const test_multiple_items_diff_scope& lhs, const test_multiple_items_diff_scope& rhs)
{
	return
		lhs.x == rhs.x &&
		lhs.s == rhs.s &&
		lhs.d == rhs.d &&
		lhs.b == rhs.b;
}

BEGIN_INTROS_TYPE(test_multiple_items_diff_scope)
ADD_INTROS_ITEM(x)
ADD_INTROS_ITEM_USER_NAME(s, MAKE_USER_NAME("s", "temp_scope", false))
ADD_INTROS_ITEM_USER_NAME(d, MAKE_USER_NAME("d", "temp_scope", false))
ADD_INTROS_ITEM(b)
END_INTROS_TYPE(test_multiple_items_diff_scope)

struct test2
{
	int x;
};
namespace test_sc
{
	BEGIN_INTROS_TYPE(test2)
		ADD_INTROS_ITEM(x)
	END_INTROS_TYPE(test2)
}

namespace test_sc2
{
	struct test3
	{
		int x;
	};
}
BEGIN_INTROS_TYPE(test_sc2::test3)
	ADD_INTROS_ITEM(x)
END_INTROS_TYPE(test_sc2::test3)

template<typename T>
void check(T& ob1, T& ob2, istream& is, const T& res)
{
	ptree tree;
	read_json(is, tree);

	intros_from_ptree(ob1, tree);

	Assert::IsTrue(ob1 == res);

	auto tree2 = intros_to_ptree(res);
	intros_from_ptree(ob2, tree2);

	Assert::IsTrue(ob2 == res);
}

struct test_c_array
{
	int a[10];
};

BEGIN_INTROS_TYPE(test_c_array)
	ADD_INTROS_ITEM(a)
END_INTROS_TYPE(test_c_array)

struct test_no_intros
{
	int x;
};

struct test_nested_no_intros
{
	int x;
	test_no_intros y;
};

BEGIN_INTROS_TYPE(test_nested_no_intros)
	ADD_INTROS_ITEM(x)
	ADD_INTROS_ITEM(y)
END_INTROS_TYPE(test_nested_no_intros)

struct test_unsupported_type
{
	map<int, string> ms;
};
BEGIN_INTROS_TYPE(test_unsupported_type)
	ADD_INTROS_ITEM(ms)
END_INTROS_TYPE(test_unsupported_type)

namespace UnitTest
{
	TEST_CLASS(UT_INTROS_PTREE_UNSUPPORTED_SCENARIOS)
	{
	public:
		TEST_METHOD(test_intros_ptree_c_array)
		{
			test_c_array ob;
			ptree tree;
			// following won't compile
			//tree = intros_to_ptree(ob);
			//intros_from_ptree(ob, tree);
			ob = { 10 }; // to remove unused variable warning
		}

		TEST_METHOD(test_intros_ptree_no_intros)
		{
			test_no_intros ob;
			ptree tree;
			// following won't compile
			//tree = intros_to_ptree(ob);
			//intros_from_ptree(ob, tree);
			ob = {10};// to remove unused variable warning
		}

		TEST_METHOD(test_intros_ptree_nested_no_intros)
		{
			test_nested_no_intros ob;
			ptree tree;
			// following won't compile
			//tree = intros_to_ptree(ob);
			//intros_from_ptree(ob, tree);
			ob = {};
		}

		TEST_METHOD(test_intros_ptree_unsupported_type)
		{
			test_unsupported_type ob;
			ptree tree;
			// following won't compile
			//tree = intros_to_ptree(ob);
			//intros_from_ptree(ob, tree);
			ob = {};
		}
	};

	TEST_CLASS(UnitTest)
	{
	public:
		TEST_METHOD(test_intros_ptree_tags_read_intros)
		{
			//item_has_intros
			static_assert(std::is_base_of<details::tags::item_has_intros, details::item_category_read_intros<test_multiple_items>>::value, "");
			// the structure itself can belong to a different scope
			// as long as, inros is defined in global scope
			static_assert(std::is_base_of<details::tags::item_has_intros, details::item_category_read_intros<test_sc2::test3>>::value, "");

			//item_is_array
			static_assert(std::is_base_of<details::tags::item_is_array, details::item_category_read_intros<int[10]>>::value, "");

			//item_can_stream_insert
			static_assert(std::is_base_of<details::tags::item_can_stream_insert, details::item_category_read_intros<int>>::value, "");
			static_assert(std::is_base_of<details::tags::item_can_stream_insert, details::item_category_read_intros<string>>::value, "");

			//item_has_input_iterator
			static_assert(std::is_base_of<details::tags::item_has_input_iterator, details::item_category_read_intros<vector<int>>>::value, "");
			static_assert(std::is_base_of<details::tags::item_has_input_iterator, details::item_category_read_intros<list<int>>>::value, "");
			static_assert(std::is_base_of<details::tags::item_has_input_iterator, details::item_category_read_intros<map<int, string>>>::value, "");

			//item_not_supported
			// intros needs to be in global scope
			static_assert(std::is_base_of<details::tags::item_not_supported, details::item_category_read_intros<test2>>::value, "");
			static_assert(std::is_base_of<details::tags::item_not_supported, details::item_category_read_intros<map<int, string>::value_type>>::value, "");
		}

		TEST_METHOD(test_intros_ptree_tags_write_intros)
		{
			//item_has_intros
			static_assert(std::is_base_of<details::tags::item_has_intros, details::item_category_write_intros<test_multiple_items>>::value, "");
			// the structure itself can belong to a different scope
			// as long as, inros is defined in global scope
			static_assert(std::is_base_of<details::tags::item_has_intros, details::item_category_write_intros<test_sc2::test3>>::value, "");

			//item_is_array
			static_assert(std::is_base_of<details::tags::item_is_array, details::item_category_write_intros<int[10]>>::value, "");

			//item_can_stream_extract
			static_assert(std::is_base_of<details::tags::item_can_stream_extract, details::item_category_write_intros<int>>::value, "");
			static_assert(std::is_base_of<details::tags::item_can_stream_extract, details::item_category_write_intros<string>>::value, "");

			//item_can_insert_at_end
			static_assert(std::is_base_of<details::tags::item_can_insert_at_end, details::item_category_write_intros<vector<int>>>::value, "");
			static_assert(std::is_base_of<details::tags::item_can_insert_at_end, details::item_category_write_intros<list<int>>>::value, "");
			static_assert(std::is_base_of<details::tags::item_can_insert_at_end, details::item_category_write_intros<map<int, string>>>::value, "");

			//item_not_supported
			// intros needs to be in global scope
			static_assert(std::is_base_of<details::tags::item_not_supported, details::item_category_write_intros<test2>>::value, "");
			static_assert(std::is_base_of<details::tags::item_not_supported, details::item_category_write_intros<map<int, string>::value_type>>::value, "");
		}

		TEST_METHOD(test_intros_ptree_general)
		{
			test_multiple_items ob1, ob2;

			string s;
			s = R"(
{"test_multiple_items":{"x":"10", "s":"hello", "d":"123.456", "b":"true"}}
)";
			test_multiple_items val{ 10, "hello", 123.456, true };

			check(ob1, ob2, stringstream(s), val);
		}

		TEST_METHOD(test_intros_ptree_dif_names)
		{
			test_multiple_items_diff_names ob1, ob2;

			string s;
			s = R"(
{"test_another_name":{"x":"10", "string_name_different":"hello", "d":"123.456", "b":"true"}}
)";
			test_multiple_items_diff_names val{ 10, "hello", 123.456, true };

			check(ob1, ob2, stringstream(s), val);
		}

		TEST_METHOD(test_intros_ptree_multiple_items_diff_scope)
		{
			test_multiple_items_diff_scope ob1, ob2;

			string s;
			s = R"(
{"test_multiple_items_diff_scope":{"x":"10", "temp_scope":{"s":"hello", "d":"123.456"}, "b":"true"}}
)";
			test_multiple_items_diff_scope val{ 10, "hello", 123.456, true };

			auto tree = intros_to_ptree(val);
			stringstream ss;
			write_json(ss, tree);
			Logger::WriteMessage(ss.str().c_str());

			check(ob1, ob2, stringstream(s), val);
		}
	};
}
