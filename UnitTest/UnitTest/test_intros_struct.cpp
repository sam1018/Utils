#include "stdafx.h"
#include "CppUnitTest.h"
#include "intros_struct.hpp"
#include <map>
#include <list>
#include <string>
#include <iostream>
#include <boost\property_tree\json_parser.hpp>

using namespace std;
using namespace utils;
using namespace utils::details;
using namespace boost::property_tree;
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

enum class enum_item_cat
{
	HAS_INTROS,
	IS_ARRAY,
	IS_STREAMABLE,
	HAS_FORWARD_ITERATOR,
	NOT_SUPPORTED
};

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<> inline std::wstring ToString<enum_item_cat>(const enum_item_cat& val) 
			{
				switch (val)
				{
				case enum_item_cat::HAS_INTROS:
					return L"HAS_INTROS";
					break;
				case enum_item_cat::IS_ARRAY:
					return L"IS_ARRAY";
					break;
				case enum_item_cat::IS_STREAMABLE:
					return L"IS_STREAMABLE";
					break;
				case enum_item_cat::HAS_FORWARD_ITERATOR:
					return L"HAS_FORWARD_ITERATOR";
					break;
				case enum_item_cat::NOT_SUPPORTED:
					return L"NOT_SUPPORTED";
					break;
				default:
					break;
				}

				return L"";
			}
		}
	}
}

#define TEST_STRUCT(in, out)			\
auto tree = struct_to_ptree(in);		\
stringstream ss;						\
write_json(ss, tree);					\
Logger::WriteMessage(ss.str().c_str());	\
try{struct_from_ptree(out, tree);}		\
catch (exception& e){					\
	Logger::WriteMessage(e.what());		\
	Assert::Fail();						\
}										\
Assert::IsTrue(in == out, L"Not Same!!!");


struct test_no_intros
{
	int val;
};

bool operator==(const test_no_intros& lhs, const test_no_intros& rhs)
{
	return lhs.val == rhs.val;
}

namespace UnitTest
{
	struct Test2
	{
		string name1;
		string name2;
	};

	struct Test
	{
		int int_val;
		float float_val;
		double double_val;
		string name;
		vector<int> int_vals;
		vector<string> string_vals;
		Test2 test2;
		int arr[6];

		INTROS_STRUCT("Test", int_vals);
	};

	struct item_cat
	{
		auto operator()(item_has_intros)			{ return enum_item_cat::HAS_INTROS;				}
		auto operator()(item_is_array)				{ return enum_item_cat::IS_ARRAY;				}
		auto operator()(item_is_streamable)			{ return enum_item_cat::IS_STREAMABLE;			}
		auto operator()(item_has_forward_iterator)	{ return enum_item_cat::HAS_FORWARD_ITERATOR;	}
		auto operator()(item_not_supported)			{ return enum_item_cat::NOT_SUPPORTED;			}
	};

	TEST_CLASS(UnitTest)
	{
	public:
		TEST_METHOD(test_item_category)
		{
			struct struct_without_intros
			{
				int x;
			};
			struct struct_with_intros
			{
				int x;
				INTROS_STRUCT("AA", x);
			};

			item_cat item_cat_tags;

			//enum_item_cat::HAS_INTROS
			Assert::AreEqual(enum_item_cat::HAS_INTROS, item_cat_tags(item_category<struct_with_intros>()));

			//enum_item_cat::IS_ARRAY
			Assert::AreEqual(enum_item_cat::IS_ARRAY, item_cat_tags(item_category<int[10]>()));
			Assert::AreEqual(enum_item_cat::IS_ARRAY, item_cat_tags(item_category<char[]>()));

			//enum_item_cat::IS_STREAMABLE
			Assert::AreEqual(enum_item_cat::IS_STREAMABLE, item_cat_tags(item_category<int>()));
			Assert::AreEqual(enum_item_cat::IS_STREAMABLE, item_cat_tags(item_category<float>()));
			Assert::AreEqual(enum_item_cat::IS_STREAMABLE, item_cat_tags(item_category<double>()));
			Assert::AreEqual(enum_item_cat::IS_STREAMABLE, item_cat_tags(item_category<string>()));

			//enum_item_cat::HAS_FORWARD_ITERATOR
			Assert::AreEqual(enum_item_cat::HAS_FORWARD_ITERATOR, item_cat_tags(item_category<vector<int>>()));
			Assert::AreEqual(enum_item_cat::HAS_FORWARD_ITERATOR, item_cat_tags(item_category<map<string, int>>()));
			Assert::AreEqual(enum_item_cat::HAS_FORWARD_ITERATOR, item_cat_tags(item_category<list<string>>()));

			//enum_item_cat::NOT_SUPPORTED
			Assert::AreEqual(enum_item_cat::NOT_SUPPORTED, item_cat_tags(item_category<map<string, int>::value_type>()));
			Assert::AreEqual(enum_item_cat::NOT_SUPPORTED, item_cat_tags(item_category<struct_without_intros>()));
		}
		
		TEST_METHOD(test_intros_struct)
		{
			//Test test;

			//test.int_val = 100;
			//test.float_val = 120.345f;
			//test.double_val = 123456789.123456789;
			//test.name = "SomeRandomName";

			//for (int i = 0; i < 2; ++i)
			//	test.int_vals.push_back(i);

			//for (char ch = 'a'; ch < 'e'; ++ch)
			//	test.string_vals.push_back(string({ch, ch, ch}));



			//test.test2.name1 = "Internal Name 1";
			//test.test2.name2 = "Internal Name 2";

			//for (int i = 0; i < 4; i++)
			//	test.arr[i] = i * 2;

			//auto tree = struct_to_ptree(test);

			//stringstream ss;

			//write_json(ss, tree);

			//Logger::WriteMessage(ss.str().c_str());

			//Test test2;

			//try
			//{
			//	struct_from_ptree(test2, tree);
			//}
			//catch (exception& e)
			//{
			//	Logger::WriteMessage(e.what());
			//}

			//ss = stringstream();

			//ss << test2.int_val << " " << test2.float_val << " " << test2.double_val << " " << test2.name << "\n";
			//copy(test2.int_vals.begin(), test2.int_vals.end(), ostream_iterator<int>(ss, " "));
			//ss << "\n";
			//copy(test2.string_vals.begin(), test2.string_vals.end(), ostream_iterator<string>(ss, " "));
			//ss << "\n";

			//Logger::WriteMessage(ss.str().c_str());
		}

		TEST_METHOD(test_code)
		{
			//typedef item_category<int[10]> tag;
			//tag x;
			//Logger::WriteMessage(typeid(x).name());
		}

		TEST_METHOD(test_struct_unsupported_no_intros)
		{
			struct struct_no_intros
			{
				int val;
				//INTROS_STRUCT("struct_int", val);

				bool operator==(const struct_no_intros& rhs)
				{
					return (val == rhs.val);
				}

			} in1, out1;

			in1.val = -1234;

			// this will fail to compile with static_assert
			//TEST_STRUCT(in1, out1);

			out1.val = 10; // to remove unreferenced variable warning
		}

		TEST_METHOD(test_struct_unsupported_array)
		{
			struct struct_no_intros
			{
				int val;
				int a[10];
				INTROS_STRUCT("struct_int", a);

				bool operator==(const struct_no_intros& rhs)
				{
					return (val == rhs.val);
				}

			} in1, out1;

			in1.val = -1234;

			// this will fail to compile with static_assert
			//TEST_STRUCT(in1, out1);

			out1.val = 123; // to remove unreferenced variable warning
		}

		TEST_METHOD(test_struct_unsupported_type)
		{
			struct struct_unsupported_type
			{
				int val;
				map<string, int> m;
				INTROS_STRUCT("struct_int", val, m);

				bool operator==(const struct_unsupported_type& rhs)
				{
					return (val == rhs.val);
				}

			} in1, out1;

			in1.val = -1234;

			// this will fail to compile with static_assert
			//TEST_STRUCT(in1, out1);

			out1.val = 10; // to remove unreferenced variable warning
		}

		TEST_METHOD(test_struct_int)
		{
			struct struct_int
			{
				int val;
				INTROS_STRUCT("struct_int", val);

				bool operator==(const struct_int& rhs)
				{
					return (val == rhs.val);
				}

			} in, out;

			in.val = -1234;

			TEST_STRUCT(in, out);
		}

		TEST_METHOD(test_struct_int_string)
		{
			struct struct_int_string
			{
				int val;
				string s;
				INTROS_STRUCT("struct_int_string", val, s);

				bool operator==(const struct_int_string& rhs)
				{
					return (
						val == rhs.val && 
						s == rhs.s);
				}

			} in, out;

			in.val = -1234;
			in.s = "Hello World!!!";

			TEST_STRUCT(in, out);
		}

		TEST_METHOD(test_struct_multiple_elems)
		{
			struct struct_int_string
			{
				int val;
				string s;
				string s2;
				float f;
				double d;
				bool b_f;
				bool b_t;
				INTROS_STRUCT("struct_int_string", val, s, s2, f, d, b_f, b_t);

				bool operator==(const struct_int_string& rhs)
				{
					return (
						val == rhs.val &&
						s == rhs.s &&
						s2 == rhs.s2 &&
						f == rhs.f &&
						d == rhs.d &&
						b_t == rhs.b_t &&
						b_t == rhs.b_t);
				}

			} in, out;

			in.val = -1234;
			in.s = "Hello World!!!";
			in.s2 = "another string\nline 2 12345 fsdfsd <> {};   \tvvdfv";
			in.f = 1234.56789f;
			in.d = 234563.127657432;
			in.b_f = false;
			in.b_t = true;

			TEST_STRUCT(in, out);
		}

		TEST_METHOD(test_struct_containers)
		{
			struct struct_conts
			{
				int val;
				vector<int> vi;
				list<string> ls;
				INTROS_STRUCT("struct_conts", val, vi, ls);

				bool operator==(const struct_conts& rhs)
				{
					return (
						val == rhs.val &&
						vi == rhs.vi &&
						ls == rhs.ls
						);
				}

			} in, out;

			in.val = -1234;

			in.vi = {1, 2, 3, 4, 5};
			in.ls = { "some", "strings", "five", "items", "else" };

			TEST_STRUCT(in, out);
		}

		TEST_METHOD(test_struct_container_struct)
		{
			struct struct_conts
			{
				int val;
				vector<test_no_intros> v;
				INTROS_STRUCT("struct_conts", val, v);

				bool operator==(const struct_conts& rhs)
				{
					return (
						val == rhs.val &&
						v == rhs.v);
				}

			} in, out;

			in.val = -1234;

			in.v = { test_no_intros{1}, test_no_intros{2}, test_no_intros{3} };

			TEST_STRUCT(in, out);
		}
	};
}
