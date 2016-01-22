#include "stdafx.h"
#include "intros_type.hpp"
#include <map>
#include <vector>

using namespace std;

struct test_empty
{
	int x;
};
// defining intros without any item is legal
// also, defining intros without all the members of the struct is also legal
// so, the following should compile
BEGIN_INTROS_TYPE(test_empty)
END_INTROS_TYPE(test_empty)

struct test
{
	int d;
	float f;
	string s;
	int a[10];
	vector<string> vs;
	map<int, vector<string>> mvs;
};

BEGIN_INTROS_TYPE(test)
	ADD_INTROS_ITEM(d)
	ADD_INTROS_ITEM(f)
	ADD_INTROS_ITEM(s)
	ADD_INTROS_ITEM(a)
	ADD_INTROS_ITEM(vs)
	ADD_INTROS_ITEM(mvs)
END_INTROS_TYPE(test)

test get_test_val()
{
	test ob;

	ob.a[4] = 10;
	ob.d = 12;
	ob.f = 123.456f;
	ob.mvs = map<int, vector<string>>{ make_pair<int, vector<string>>(1,{ "hello", "string2" }) };
	ob.s = "test";
	ob.vs = { "ab", "cd" };

	return ob;
}

template<typename T>
bool compare_test_with_intros(const test& ob, const T& intros)
{
	auto t = intros.items;
	return
		intros.name == "test" &&
		get<0>(t).name == "d" && get<0>(t).val == 12 &&
		get<1>(t).name == "f" && get<1>(t).val == 123.456f &&
		get<2>(t).name == "s" && get<2>(t).val == "test" &&
		get<3>(t).name == "a" && get<3>(t).val[4] == 10 &&
		get<4>(t).name == "vs" && get<4>(t).val == vector<string>{ "ab", "cd" } &&
		get<5>(t).name == "mvs" && get<5>(t).val == map<int, vector<string>>{ make_pair<int, vector<string>>(1, { "hello", "string2" }) }
	;
}

void set_test_val(test& ob)
{
	ob = get_test_val();
}

// we can set different name for the intros
struct test_diff_name
{
	int x;
};

BEGIN_INTROS_TYPE_USER_NAME(test_diff_name, "another name")
	ADD_INTROS_ITEM(x)
END_INTROS_TYPE(test_diff_name)

// we can set different name for item
struct test_diff_item_name
{
	int x;
};

BEGIN_INTROS_TYPE(test_diff_item_name)
	ADD_INTROS_ITEM_USER_NAME(x, "diff item name")
END_INTROS_TYPE(test_diff_item_name)

struct test_no_default_ctor
{
	int x;
	test_no_default_ctor(int _x) : x(_x)
	{
	}
};

// no default ctor works as well
// the following is ok
BEGIN_INTROS_TYPE(test_no_default_ctor)
	ADD_INTROS_ITEM(x)
END_INTROS_TYPE(test_no_default_ctor)

struct test_new_scope
{
	int x;
};

BEGIN_INTROS_TYPE(test_new_scope)
	ADD_INTROS_ITEM(x)
END_INTROS_TYPE(test_new_scope)

namespace new_scope
{
	// we can define intros in two different scopes
	BEGIN_INTROS_TYPE(test_new_scope)
		ADD_INTROS_ITEM(x)
	END_INTROS_TYPE(test_new_scope)
}

struct test2
{
	int x;
};

BEGIN_INTROS_TYPE(test2)
	ADD_INTROS_ITEM(x)
END_INTROS_TYPE(test2)

struct test_nest_intros
{
	test ob1;
	vector<test2> ob2;
	int x;
	string s;
};
BEGIN_INTROS_TYPE(test_nest_intros)
	ADD_INTROS_ITEM(ob1)
	ADD_INTROS_ITEM(ob2)
	ADD_INTROS_ITEM(x)
	ADD_INTROS_ITEM(s)
END_INTROS_TYPE(test_nest_intros)


BOOST_AUTO_TEST_CASE(test_intros)
{
	test ob;

	set_test_val(ob);

	auto t = get_intros_type(ob);

	// any modification in ob will be reflected in t

	BOOST_CHECK(compare_test_with_intros(ob, t));

	// the opposite is also true
	// any change in t will be reflected in ob

	get<0>(t.items).val = 12;

	BOOST_CHECK(ob.d == 12);
}

BOOST_AUTO_TEST_CASE(test_intros_for_const)
{
	const test ob = get_test_val();

	auto t = get_intros_type(ob);

	// We can read from t
	BOOST_CHECK(get<0>(t.items).val == 12);

	// but we can't modify t.items
	// the following won't compile
	//get<0>(t.items).val = 14;
}

BOOST_AUTO_TEST_CASE(test_intros_change_name)
{
	test ob;
	auto t = get_intros_type(ob);
	auto t2 = get_intros_type(ob);

	t.name = "changed_name";
	get<0>(t.items).name = "another name";

	// changing names have impact only that instance of t
	BOOST_CHECK(!(t.name == t2.name));
	BOOST_CHECK(!(get<0>(t.items).name == get<0>(t2.items).name));
}

BOOST_AUTO_TEST_CASE(test_intros_diff_name)
{
	test_diff_name ob;

	BOOST_CHECK(get_intros_type(ob).name == "another name"s);
}

BOOST_AUTO_TEST_CASE(test_intros_diff_item_name)
{
	test_diff_item_name ob;

	BOOST_CHECK(get<0>(get_intros_type(ob).items).name == "diff item name"s);
}

BOOST_AUTO_TEST_CASE(test_intros_nested)
{
	test_nest_intros ob;

	ob.ob1.s = "ABCD";

	auto nested_intros = get_intros_type(ob.ob1);

	BOOST_CHECK((get<2>(nested_intros.items)).val == ob.ob1.s);
}
