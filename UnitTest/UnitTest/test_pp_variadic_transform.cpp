#include "stdafx.h"
#include "CppUnitTest.h"
#include "pp_variadic_transform.hpp"
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

#define ADD(s, data, elem)	BOOST_PP_ADD(data, elem)

namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			template<> inline std::wstring ToString<vector<int>>(const vector<int>& vi)
			{
				wstring s;
				for (auto x : vi)
				{
					s += ToString(x);
					s += L":";
				}

				return s;
			}
		}
	}
}

namespace UnitTest
{
	TEST_CLASS(UnitTest)
	{
	public:
		TEST_METHOD(test_pp_variadic_transform)
		{
			vector<int> v{UTILS_PP_VARIADIC_TRANSFORM(ADD, 10, 1, 2, 3, 4, 5)};

			Assert::AreEqual({ 11, 12, 13, 14, 15 }, v);
		}
	};
}
