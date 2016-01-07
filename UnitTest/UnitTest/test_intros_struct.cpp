#include "stdafx.h"
#include "CppUnitTest.h"
#include "intros_struct.hpp"
#include <iostream>
#include <boost\property_tree\json_parser.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace utils;
using namespace std;
using namespace boost::property_tree;

enum class enum_item_cat
{
	HAS_INTROS,
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

	using namespace utils::details;

	auto item_cat(item_has_intros) { return enum_item_cat::HAS_INTROS; }
	auto item_cat(item_not_supported) { return enum_item_cat::NOT_SUPPORTED; }

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

			typename item_category<int> int_cat;
			typename item_category<string> string_cat;
			typename item_category<struct_without_intros> struct_without_intros_cat;
			typename item_category<struct_with_intros> struct_with_intros_cat;

			auto b1 = has_intros<struct_without_intros>::value;
			auto b2 = has_intros<struct_with_intros>::value;

			Assert::AreEqual(enum_item_cat::NOT_SUPPORTED, item_cat(int_cat));
			Assert::AreEqual(enum_item_cat::NOT_SUPPORTED, item_cat(string_cat));
			Assert::AreEqual(enum_item_cat::NOT_SUPPORTED, item_cat(struct_without_intros_cat));
			Assert::AreEqual(enum_item_cat::HAS_INTROS, item_cat(struct_with_intros_cat));
		}
		
		TEST_METHOD(test_intros_struct)
		{
			Test test;

			test.int_val = 100;
			test.float_val = 120.345f;
			test.double_val = 123456789.123456789;
			test.name = "SomeRandomName";

			for (int i = 0; i < 2; ++i)
				test.int_vals.push_back(i);

			for (char ch = 'a'; ch < 'e'; ++ch)
				test.string_vals.push_back(string({ch, ch, ch}));



			test.test2.name1 = "Internal Name 1";
			test.test2.name2 = "Internal Name 2";

			for (int i = 0; i < 4; i++)
				test.arr[i] = i * 2;

			auto tree = struct_to_ptree(test);

			stringstream ss;

			write_json(ss, tree);

			Logger::WriteMessage(ss.str().c_str());

			Test test2;

			try
			{
				struct_from_ptree(test2, tree);
			}
			catch (exception& e)
			{
				Logger::WriteMessage(e.what());
			}

			ss = stringstream();

			ss << test2.int_val << " " << test2.float_val << " " << test2.double_val << " " << test2.name << "\n";
			copy(test2.int_vals.begin(), test2.int_vals.end(), ostream_iterator<int>(ss, " "));
			ss << "\n";
			copy(test2.string_vals.begin(), test2.string_vals.end(), ostream_iterator<string>(ss, " "));
			ss << "\n";

			Logger::WriteMessage(ss.str().c_str());
		}
	};
}
