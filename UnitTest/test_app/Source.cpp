#include <iostream>
#include <boost\property_tree\xml_parser.hpp>
#include "intros_ptree.hpp"


using namespace std;
using namespace utils::intros_ptree;
using namespace boost::property_tree;


struct test_simple
{
	int a{};
	float f{};
	double d{};
	string x;
};
BEGIN_INTROS_TYPE(test_simple)
	ADD_INTROS_ITEM(a)
	ADD_INTROS_ITEM(f)
	ADD_INTROS_ITEM(d)
	ADD_INTROS_ITEM(x)
END_INTROS_TYPE(test_simple)


struct test_attributes
{
	int a{};
	float f{};
	double d{};
	string x;
	int att1{};
	string att2;
};
BEGIN_INTROS_TYPE(test_attributes)
	ADD_INTROS_ITEM(a)
	ADD_INTROS_ITEM(f)
	ADD_INTROS_ITEM(d)
	ADD_INTROS_ITEM(x)
	ADD_INTROS_ITEM_USER_NAME(att1, MAKE_USER_NAME("att1", "", true))
	ADD_INTROS_ITEM_USER_NAME(att2, MAKE_USER_NAME("att2", "", true))
END_INTROS_TYPE(test_attributes)



struct test_scope
{
	int a{};
	float f{};
	double d{};
	string x;
	int att1{};
	string att2;
	int a_newscope{};
	float f_newscope{};
	double d_newscope{};
	string x_newscope;
	int att1_newscope{};
	string att2_newscope;
};

BEGIN_INTROS_TYPE(test_scope)
	ADD_INTROS_ITEM(a)
	ADD_INTROS_ITEM(f)
	ADD_INTROS_ITEM(d)
	ADD_INTROS_ITEM(x)
	ADD_INTROS_ITEM_USER_NAME(att1, MAKE_USER_NAME("att1", "", true))
	ADD_INTROS_ITEM_USER_NAME(att2, MAKE_USER_NAME("att2", "", true))

	ADD_INTROS_ITEM_USER_NAME(a_newscope, MAKE_USER_NAME("a_newscope", "newscope", false))
	ADD_INTROS_ITEM_USER_NAME(f_newscope, MAKE_USER_NAME("f_newscope", "newscope", false))
	ADD_INTROS_ITEM_USER_NAME(d_newscope, MAKE_USER_NAME("d_newscope", "newscope", false))
	ADD_INTROS_ITEM_USER_NAME(x_newscope, MAKE_USER_NAME("x_newscope", "newscope", false))
	ADD_INTROS_ITEM_USER_NAME(att1_newscope, MAKE_USER_NAME("att1_newscope", "newscope", true))
	ADD_INTROS_ITEM_USER_NAME(att2_newscope, MAKE_USER_NAME("att2_newscope", "newscope", true))
END_INTROS_TYPE(test_scope)


struct test_container
{
	vector<string> vs;
};

BEGIN_INTROS_TYPE(test_container)
	ADD_INTROS_ITEM(vs)
END_INTROS_TYPE(test_container)


struct test_container_with_scope
{
	vector<string> vs;
};

BEGIN_INTROS_TYPE(test_container_with_scope)
	ADD_INTROS_ITEM_USER_NAME(vs, MAKE_USER_NAME("vs", "scoped", false))
END_INTROS_TYPE(test_container_with_scope)


struct test_nested
{
	test_container tc;
};

BEGIN_INTROS_TYPE(test_nested)
	ADD_INTROS_ITEM(tc)
END_INTROS_TYPE(test_nested)

struct test_nested_2
{
	test_container_with_scope tc;
};

BEGIN_INTROS_TYPE(test_nested_2)
	ADD_INTROS_ITEM(tc)
END_INTROS_TYPE(test_nested_2)


struct test_more_nested
{
	vector<test_nested> tc1;
	vector<test_nested_2> tc2;
	test_container_with_scope tc3;
	test_scope tc4;
};

BEGIN_INTROS_TYPE(test_more_nested)
	ADD_INTROS_ITEM(tc1)
	ADD_INTROS_ITEM(tc2)
	ADD_INTROS_ITEM(tc3)
	ADD_INTROS_ITEM(tc4)
END_INTROS_TYPE(test_more_nested)


template<typename T>
void print_xml(ostream& os, T ob = T())
{
	auto tree = make_ptree(ob);
	xml_writer_settings<string> settings(' ', 4);
	write_xml(os, tree, settings);
	os << "\n";
}

int main()
{
	print_xml<test_simple>(cout);

	print_xml<test_attributes>(cout);

	print_xml<test_scope>(cout);

	test_container ob_1;
	ob_1.vs.resize(4);
	print_xml<test_container>(cout, ob_1);

	test_container_with_scope ob_2;
	ob_2.vs.resize(3);
	print_xml<test_container_with_scope>(cout, ob_2);

	test_nested ob_3;
	ob_3.tc.vs.resize(2);
	print_xml<test_nested>(cout, ob_3);

	test_nested_2 ob_4;
	ob_4.tc.vs.resize(3);
	print_xml<test_nested_2>(cout, ob_4);

	test_more_nested ob;
	ob.tc1.resize(3);
	ob.tc2.resize(4);

	print_xml<test_more_nested>(cout, ob);
}
