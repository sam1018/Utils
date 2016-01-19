#include "stdafx.h"
#include "CppUnitTest.h"
#include "intros_ptree.hpp"
#include <map>
#include <set>
#include <list>
#include <string>
#include <vector>
#include <boost\property_tree\json_parser.hpp>
#include <boost\property_tree\xml_parser.hpp>

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
void check(istream& is, const T& res, bool is_xml = false)
{
	ptree tree;

	if (is_xml)
		read_xml(is, tree);
	else
		read_json(is, tree);

	auto ob1 = make_intros_object<T>(tree);

	Assert::IsTrue(ob1 == res);

	auto tree2 = make_ptree(res);

	auto ob2 = make_intros_object<T>(tree2);

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

struct test_unsupported_no_def_ctor
{
	int x;
	explicit test_unsupported_no_def_ctor(int _x) : x(_x) {}
};
BEGIN_INTROS_TYPE(test_unsupported_no_def_ctor)
	ADD_INTROS_ITEM(x)
END_INTROS_TYPE(test_unsupported_no_def_ctor)

struct test_containers
{
	int x;
	vector<string> vs;
	set<double> sd;
};
BEGIN_INTROS_TYPE(test_containers)
	ADD_INTROS_ITEM(x)
	ADD_INTROS_ITEM(vs)
	ADD_INTROS_ITEM(sd)
END_INTROS_TYPE(test_containers)

bool operator==(const test_containers& lhs, const test_containers& rhs)
{
	return
		lhs.x == rhs.x &&
		lhs.vs == rhs.vs &&
		lhs.sd == rhs.sd;
}

struct test_containers_diff_scope
{
	int x;
	vector<string> vs;
	set<double> sd;
};
BEGIN_INTROS_TYPE(test_containers_diff_scope)
	ADD_INTROS_ITEM(x)
	ADD_INTROS_ITEM_USER_NAME(vs, MAKE_USER_NAME("vs", "all_vs", false))
	ADD_INTROS_ITEM_USER_NAME(sd, MAKE_USER_NAME("sd", "all_sd", false))
END_INTROS_TYPE(test_containers_diff_scope)

bool operator==(const test_containers_diff_scope& lhs, const test_containers_diff_scope& rhs)
{
	return
		lhs.x == rhs.x &&
		lhs.vs == rhs.vs &&
		lhs.sd == rhs.sd;
}

struct test_attributes_2
{
	int x;
	string name;
	vector<string> vs;
	set<double> sd;
};
BEGIN_INTROS_TYPE(test_attributes_2)
	ADD_INTROS_ITEM_USER_NAME(x, MAKE_USER_NAME("x", "", true))
	ADD_INTROS_ITEM_USER_NAME(name, MAKE_USER_NAME("name", "", true))
	ADD_INTROS_ITEM_USER_NAME(vs, MAKE_USER_NAME("vs", "all_vs", false))
	ADD_INTROS_ITEM_USER_NAME(sd, MAKE_USER_NAME("sd", "all_sd", false))
END_INTROS_TYPE(test_attributes_2)

bool operator==(const test_attributes_2& lhs, const test_attributes_2& rhs)
{
	return
		lhs.x == rhs.x &&
		lhs.name == rhs.name &&
		lhs.vs == rhs.vs &&
		lhs.sd == rhs.sd;
}

namespace UnitTest
{
	TEST_CLASS(UT_INTROS_PTREE_TEST_TAGS)
	{
	public:
		TEST_METHOD(test_intros_ptree_tags_read_intros)
		{
			//item_has_intros
			static_assert(std::is_base_of<details::tags::item_has_intros, details::item_category_read_intros<test_multiple_items>>::value, "");
			static_assert(std::is_base_of<details::tags::item_has_intros, details::item_category_read_intros<const test_multiple_items>>::value, "");
			// the structure itself can belong to a different scope
			// as long as, inros is defined in global scope
			static_assert(std::is_base_of<details::tags::item_has_intros, details::item_category_read_intros<test_sc2::test3>>::value, "");

			//item_is_array
			//we only detect it to give static_assert
			static_assert(std::is_base_of<details::tags::item_is_array, details::item_category_read_intros<int[10]>>::value, "");

			//item_can_stream_insert
			static_assert(std::is_base_of<details::tags::item_can_stream_insert, details::item_category_read_intros<int>>::value, "");
			static_assert(std::is_base_of<details::tags::item_can_stream_insert, details::item_category_read_intros<string>>::value, "");
			static_assert(std::is_base_of<details::tags::item_can_stream_insert, details::item_category_read_intros<const string>>::value, "");

			//item_has_input_iterator
			static_assert(std::is_base_of<details::tags::item_has_input_iterator, details::item_category_read_intros<vector<int>>>::value, "");
			static_assert(std::is_base_of<details::tags::item_has_input_iterator, details::item_category_read_intros<list<int>>>::value, "");
			static_assert(std::is_base_of<details::tags::item_has_input_iterator, details::item_category_read_intros<map<int, string>>>::value, "");
			static_assert(std::is_base_of<details::tags::item_has_input_iterator, details::item_category_read_intros<set<double>>>::value, "");

			//item_not_supported
			//intros needs to be in global scope
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
			static_assert(std::is_base_of<details::tags::item_is_array, details::item_category_read_intros<int[10]>>::value, "");
			static_assert(std::is_base_of<details::tags::item_is_array, details::item_category_write_intros<int[10]>>::value, "");

			//item_can_stream_extract
			static_assert(std::is_base_of<details::tags::item_can_stream_extract, details::item_category_write_intros<int>>::value, "");
			static_assert(std::is_base_of<details::tags::item_can_stream_extract, details::item_category_write_intros<string>>::value, "");
			static_assert(std::is_base_of<details::tags::item_has_input_iterator, details::item_category_read_intros<set<double>>>::value, "");

			//item_can_insert_at_end
			static_assert(std::is_base_of<details::tags::item_can_insert_at_end, details::item_category_write_intros<vector<int>>>::value, "");
			static_assert(std::is_base_of<details::tags::item_can_insert_at_end, details::item_category_write_intros<list<int>>>::value, "");
			static_assert(std::is_base_of<details::tags::item_can_insert_at_end, details::item_category_write_intros<map<int, string>>>::value, "");

			//item_not_supported
			// intros needs to be in global scope
			static_assert(std::is_base_of<details::tags::item_not_supported, details::item_category_write_intros<test2>>::value, "");
			static_assert(std::is_base_of<details::tags::item_not_supported, details::item_category_write_intros<map<int, string>::value_type>>::value, "");
			// we can't write into const object
			static_assert(std::is_base_of<details::tags::item_not_supported, details::item_category_write_intros<const string>>::value, "");
			static_assert(std::is_base_of<details::tags::item_not_supported, details::item_category_write_intros<const int>>::value, "");
		}
	};

	TEST_CLASS(UT_INTROS_PTREE_UNSUPPORTED_SCENARIOS)
	{
	public:
		TEST_METHOD(test_intros_ptree_c_array)
		{
			ptree tree;
			test_c_array ob;
			// following won't compile
			//tree = make_ptree(ob);
			//ob = make_intros_object<test_c_array>(tree);
			ob = { 10 }; // to remove unused variable warning
		}

		TEST_METHOD(test_intros_ptree_no_intros)
		{
			test_no_intros ob;
			ptree tree;
			// following won't compile
			//tree = make_ptree(ob);
			//ob = make_intros_object<test_no_intros>(tree);
			ob = {10};// to remove unused variable warning
		}

		TEST_METHOD(test_intros_ptree_nested_no_intros)
		{
			test_nested_no_intros ob;
			ptree tree;
			// following won't compile
			//tree = make_ptree(ob);
			//ob = make_intros_object<test_nested_no_intros>(tree);
			ob = {};// to remove unused variable warning
		}

		TEST_METHOD(test_intros_ptree_unsupported_type)
		{
			test_unsupported_type ob;
			ptree tree;
			// following won't compile
			//tree = make_ptree(ob);
			//ob = make_intros_object<test_unsupported_type>(tree);
			ob = {};// to remove unused variable warning
		}

		TEST_METHOD(test_intros_type_unsupported_no_def_ctor)
		{
			test_unsupported_no_def_ctor ob(10);
			ptree tree;
			// we can read from it
			tree = make_ptree(ob);
			// but we can't write to it
			// following won't compile
			//ob = make_intros_object<test_unsupported_no_def_ctor>(tree);
		}
	};

	TEST_CLASS(UnitTest)
	{
	public:
		TEST_METHOD(test_intros_ptree_general)
		{
			string s = R"(
{"test_multiple_items":{"x":"10", "s":"hello", "d":"123.456", "b":"true"}}
)";
			test_multiple_items val{ 10, "hello", 123.456, true };

			check(stringstream(s), val);
		}

		TEST_METHOD(test_intros_ptree_dif_names)
		{
			string s = R"(
{"test_another_name":{"x":"10", "string_name_different":"hello", "d":"123.456", "b":"true"}}
)";
			test_multiple_items_diff_names val{ 10, "hello", 123.456, true };

			check(stringstream(s), val);
		}

		TEST_METHOD(test_intros_ptree_multiple_items_diff_scope)
		{
			string s = R"(
{"test_multiple_items_diff_scope":{"x":"10", "temp_scope":{"s":"hello", "d":"123.456"}, "b":"true"}}
)";
			test_multiple_items_diff_scope val{ 10, "hello", 123.456, true };

			auto tree = make_ptree(val);
			stringstream ss;
			write_json(ss, tree);
			Logger::WriteMessage(ss.str().c_str());

			check(stringstream(s), val);
		}

		TEST_METHOD(test_intros_struct_containers)
		{
			string s = R"(
{"test_containers":{"x":"10", "vs":"string1", "vs":"string2", "vs":"string3", "sd":"-20", "sd":"-40", "sd":"100"}}
)";
			test_containers val;
			val.x = 10;
			val.vs = vector<string>{"string1", "string2", "string3"};
			val.sd.insert(-20);
			val.sd.insert(-40);
			val.sd.insert(100);

			auto tree = make_ptree(val);
			stringstream ss;
			write_json(ss, tree);
			Logger::WriteMessage(ss.str().c_str());

			check(stringstream(s), val);
		}

		TEST_METHOD(test_intros_struct_containers_diff_scope)
		{
			string s = R"(
{"test_containers_diff_scope":{"x":"1234", "all_vs":{"vs":"1", "vs":"708", "vs":"789"}, "all_sd":{"sd":"123.456", "sd":"-108", "sd":"456"}}}
)";
			test_containers_diff_scope val;
			val.x = 1234;
			val.vs = vector<string>{"1", "708", "789"};
			val.sd.insert(123.456);
			val.sd.insert(-108);
			val.sd.insert(456);

			check(stringstream(s), val);
		}

		TEST_METHOD(test_intros_struct_attributes)
		{
			string s = R"(
<test_attributes_2 x="1234" name="hello">
	<all_vs>
		<vs>1</vs>
		<vs>708</vs>
		<vs>789</vs>
	</all_vs>
	<all_sd>
		<sd>123.456</sd>
		<sd>-108</sd>
		<sd>456</sd>
	</all_sd>
</test_attributes_2>
)";
			test_attributes_2 val;
			val.x = 1234;
			val.name = "hello";
			val.vs = vector<string>{ "1", "708", "789" };
			val.sd.insert(123.456);
			val.sd.insert(-108);
			val.sd.insert(456);

			check(stringstream(s), val, true);
		}
	};
}
