#include <iostream>
#include <util_traits.hpp>
#include <intros_ptree.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;
using namespace utils::intros_ptree;
using namespace boost::property_tree;

struct book
{
	int id;
	string name;
	string author;
};

BEGIN_INTROS_TYPE(book)
	ADD_INTROS_ITEM(id)
	ADD_INTROS_ITEM(name)
	ADD_INTROS_ITEM(author)
END_INTROS_TYPE(book)

void quick_intro()
{
	string sample_xml = R"(
<book>
    <id>102</id>
    <name>i am a book</name>
    <author>i am a writer</author>
</book>
)";

	ptree tree;
	stringstream ss(sample_xml);
	read_xml(ss, tree);
	auto ob = make_intros_object<book>(tree);

	ob.name = "new name"; // let's make some modification before writing
	auto tree2 = make_ptree(ob);
	xml_writer_settings<string> settings(' ', 4); // this is needed for xml printing to have proper whitespace
	write_xml(cout, tree2, settings);
}

struct book1
{
	int id;
	string name;
	string author;
};

BEGIN_INTROS_TYPE_USER_NAME(book1, "root")
	ADD_INTROS_ITEM(id)
	ADD_INTROS_ITEM(name)
	ADD_INTROS_ITEM(author)
END_INTROS_TYPE(book1)

template<typename MyStruct>
void read_write(const string& s)
{
	// here is how we populate a structure
	ptree tree;
	stringstream ss(s);
	read_xml(ss, tree);
	auto ob = make_intros_object<MyStruct>(tree);

	// here is how we write the data to xml file
	auto tree2 = make_ptree(ob);
	xml_writer_settings<string> settings(' ', 4);
	write_xml(cout, tree2, settings);
}

void example1()
{
	string sample_xml = R"(
<root>
    <id>102</id>
    <name>i am a book</name>
    <author>i am a writer</author>
</root>
)";
	read_write<book1>(sample_xml);
}

struct book2
{
	int id;
	string name;
	string author;
};
BEGIN_INTROS_TYPE_USER_NAME(book2, "book")
	ADD_INTROS_ITEM_USER_NAME(id, "book_id")
	ADD_INTROS_ITEM(name)
	ADD_INTROS_ITEM(author)
END_INTROS_TYPE(book2)

void example2()
{
	string sample_xml = R"(
<book>
    <book_id>102</book_id>
    <name>i am a book</name>
    <author>i am a writer</author>
</book>
)";
	read_write<book2>(sample_xml);
}

struct book3
{
	int id;
	string name;
	vector<string> author;
};

BEGIN_INTROS_TYPE_USER_NAME(book3, "book")
	ADD_INTROS_ITEM_USER_NAME(id, "book_id")
	ADD_INTROS_ITEM(name)
	ADD_INTROS_ITEM(author)
END_INTROS_TYPE(book3)

void example3()
{
	string sample_xml = R"(
<book>
    <book_id>102</book>
    <name>i am a book</name>
    <author>i am writer 1</author>
    <author>i am writer 2</author>
    <author>i am writer 3</author>
</book>
)";
	read_write<book3>(sample_xml);
}

struct book4
{
	int id;
	string name;
	vector<string> author;
};

BEGIN_INTROS_TYPE_USER_NAME(book4, "book")
	ADD_INTROS_ITEM_USER_NAME(id, "book_id")
	ADD_INTROS_ITEM(name)
	ADD_INTROS_ITEM_USER_NAME(author, MAKE_USER_NAME("author", "all_authors", false))
END_INTROS_TYPE(book4)

void example4()
{
	string sample_xml = R"(
<book>
    <book_id>102</book>
    <name>i am a book</name>
    <all_authors>
        <author>i am writer 1</author>
        <author>i am writer 2</author>
        <author>i am writer 3</author>
    </all_authors>
</book>
)";
	read_write<book4>(sample_xml);
}

struct book5
{
	int id;
	string name;
	string author;
};

BEGIN_INTROS_TYPE_USER_NAME(book5, "book")
	ADD_INTROS_ITEM_USER_NAME(id, MAKE_USER_NAME("book_id", "", true))
	ADD_INTROS_ITEM(name)
	ADD_INTROS_ITEM(author)
END_INTROS_TYPE(book5)

void example5()
{
	string sample_xml = R"(
<book book_id="102">
    <name>i am a book</name>
    <author>i am a writer</author>
</book>
)";
	read_write<book5>(sample_xml);
}

struct book6
{
	int id;
	string name;
	string author;
};

BEGIN_INTROS_TYPE_USER_NAME(book6, "book")
	ADD_INTROS_ITEM(id)
	ADD_INTROS_ITEM(name)
	ADD_INTROS_ITEM(author)
END_INTROS_TYPE(book6)

struct catalog
{
	string name;
	vector<book> books;
};

BEGIN_INTROS_TYPE(catalog)
	ADD_INTROS_ITEM(name)
	ADD_INTROS_ITEM_USER_NAME(books, "book")
END_INTROS_TYPE(catalog)

void example6()
{
	string sample_xml = R"(
<catalog>
    <name>I am the catalog</name>
    <book>
        <id>102</id>
        <name>i am a book</name>
        <author>i am writer 1</author>
    </book>
    <book>
        <id>103</id>
        <name>i am also book</name>
        <author>i am writer 2</author>
    </book>
    <book>
        <id>104</id>
        <name>i am another book</name>
        <author>i am writer 3</author>
    </book>
</catalog>
)";
	read_write<catalog>(sample_xml);
}

void debug_tip()
{
	catalog c;
	c.books.resize(2);
	ptree tree = make_ptree(c);
	xml_writer_settings<string> settings(' ', 4); // this is needed for xml printing to have proper whitespace
	write_xml(cout, tree, settings);
}

void intros_example()
{
	book b;

	auto intros_object = get_intros_type(b);

	cout << typeid(intros_object).name() << "\n";

	cout << "Type name: " << intros_object.name << "\n";

	// set values
	auto t = intros_object.items;
	
	std::get<0>(t).val = 1234;

	cout << "Name of item: " << std::get<0>(t).name << "\n";
	cout << "Value from b: " << b.id << "\tValue from intros: " << std::get<0>(t).val << "\n";
}

IMPLEMENT_EXPR_TEST(has_x, decltype(T::x), T)

IMPLEMENT_EXPR_TEST(has_begin, decltype(declval<T&>().begin()), T)

template<typename T> using has_begin_test = decltype(declval<T&>().begin());

IMPLEMENT_EXPR_TEST_INDIRECT(has_begin_indirect, has_begin_test, T)

void sfinae_example()
{
	struct A { int x; int y; };
	static_assert(has_x<A>::value, "");

	static_assert(!has_begin<int>::value, "");
	static_assert(has_begin<vector<int>>::value, "");

	static_assert(!has_begin_indirect<int>::value, "");
	static_assert(has_begin_indirect<vector<int>>::value, "");
}

class tag_is_streamable {};
class tag_is_container {};

IMPLEMENT_EXPR_TEST(is_streamable, decltype(std::declval<std::ostream&>() << std::declval<T&>()), T)
IMPLEMENT_EXPR_TEST(is_container, decltype(std::declval<T&>().begin()), T)

template<typename T>
using my_tag = utils::traits::get_tag<
	is_streamable<T>, tag_is_streamable,
	is_container<T>, tag_is_container
>;

template<typename T>
void f(const T&, tag_is_streamable)
{
	cout << "tag_is_streamable f\n";
}

template<typename T>
void f(const T&, tag_is_container)
{
	cout << "tag_is_container f\n";
}

std::string s;

void tag_dispatch_example()
{
	f(s, my_tag<vector<int>>()); // will choose the container version
	f(s, my_tag<string>()); // string satisfies both streamable and container condition
							// so, streamable will be chosen as that gets the higher priority
}

int main()
{
	//quick_intro();
	//example1();
	//example2();
	//example3();
	//example4();
	//example5();
	//example6();
	//debug_tip();
	//intros_example();
	//sfinae_example();
	tag_dispatch_example();
}
