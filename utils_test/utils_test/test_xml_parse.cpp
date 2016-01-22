#include "stdafx.h"
#include "intros_ptree.hpp"
#include <boost\property_tree\ptree.hpp>
#include <boost\property_tree\xml_parser.hpp>

using namespace std;
using namespace boost::property_tree;
using namespace utils::intros_ptree;

struct book
{
	string id;
	string author;
	string title;
	string genre;
	string price;
	string publish_date;
	string description;
};

BEGIN_INTROS_TYPE(book)
	ADD_INTROS_ITEM_USER_NAME(id, MAKE_USER_NAME("id", "", true))
	ADD_INTROS_ITEM(author)
	ADD_INTROS_ITEM(title)
	ADD_INTROS_ITEM(genre)
	ADD_INTROS_ITEM(price)
	ADD_INTROS_ITEM(publish_date)
	ADD_INTROS_ITEM(description)
END_INTROS_TYPE(book)

bool operator==(const book& lhs, const book& rhs)
{
	return (
		lhs.id == rhs.id			&&
		lhs.author == rhs.author		&&
		lhs.title == rhs.title		&&
		lhs.genre == rhs.genre		&&
		lhs.price == rhs.price		&&
		lhs.publish_date == rhs.publish_date &&
		lhs.description == rhs.description
		);
}

struct catalog
{
	vector<book> books;
};

BEGIN_INTROS_TYPE(catalog)
	ADD_INTROS_ITEM_USER_NAME(books, "book")
END_INTROS_TYPE(catalog)

bool operator==(const catalog& lhs, const catalog& rhs)
{
	return lhs.books == rhs.books;
}

catalog get_actual_value()
{
	vector<book> books = {
		book{ "bk101", "Gambardella, Matthew",	"XML Developer's Guide",					"Computer",			"44.95",	"2000-10-01", "\n      An in-depth look at creating applications\n      with XML.\n    " },
		book{ "bk102", "Ralls, Kim",			"Midnight Rain",							"Fantasy",			"5.95",		"2000-12-16", "\n      A former architect battles corporate zombies,\n      an evil sorceress, and her own childhood to become queen\n      of the world.\n    " },
		book{ "bk103", "Corets, Eva",			"Maeve Ascendant",							"Fantasy",			"5.95",		"2000-11-17", "\n      After the collapse of a nanotechnology\n      society in England, the young survivors lay the\n      foundation for a new society.\n    " },
		book{ "bk104", "Corets, Eva",			"Oberon's Legacy",							"Fantasy",			"5.95",		"2001-03-10", "\n      In post-apocalypse England, the mysterious\n      agent known only as Oberon helps to create a new life\n      for the inhabitants of London. Sequel to Maeve\n      Ascendant.\n    " },
		book{ "bk105", "Corets, Eva",			"The Sundered Grail",						"Fantasy",			"5.95",		"2001-09-10", "\n      The two daughters of Maeve, half-sisters,\n      battle one another for control of England. Sequel to\n      Oberon's Legacy.\n    " },
		book{ "bk106", "Randall, Cynthia",		"Lover Birds",								"Romance",			"4.95",		"2000-09-02", "\n      When Carla meets Paul at an ornithology\n      conference, tempers fly as feathers get ruffled.\n    " },
		book{ "bk107", "Thurman, Paula",		"Splish Splash",							"Romance",			"4.95",		"2000-11-02", "\n      A deep sea diver finds true love twenty\n      thousand leagues beneath the sea.\n    " },
		book{ "bk108", "Knorr, Stefan",			 "Creepy Crawlies",							"Horror",			"4.95",		"2000-12-06", "\n      An anthology of horror stories about roaches,\n      centipedes, scorpions  and other insects.\n    " },
		book{ "bk109", "Kress, Peter",			"Paradox Lost",								"Science Fiction",	"6.95",		"2000-11-02", "\n      After an inadvertant trip through a Heisenberg\n      Uncertainty Device, James Salway discovers the problems\n      of being quantum.\n    " },
		book{ "bk110", "O'Brien, Tim",			"Microsoft .NET: The Programming Bible",	"Computer",			"36.95",	"2000-12-09", "\n      Microsoft's .NET initiative is explored in\n      detail in this deep programmer's reference.\n    " },
		book{ "bk111", "O'Brien, Tim",			"MSXML3: A Comprehensive Guide",			"Computer",			"36.95",	"2000-12-01", "\n      The Microsoft MSXML3 parser is covered in\n      detail, with attention to XML DOM interfaces, XSLT processing,\n      SAX and more.\n    " },
		book{ "bk112", "Galos, Mike",			"Visual Studio 7: A Comprehensive Guide",	"Computer",			"49.95",	"2001-04-16", "\n      Microsoft Visual Studio 7 is explored in depth,\n      looking at how Visual Basic, Visual C++, C#, and ASP+ are\n      integrated into a comprehensive development\n      environment.\n    " }
	};

	return catalog{ books };
}

struct debug
{
	string filename;
	int level;
	vector<string> modules;
};

BEGIN_INTROS_TYPE(debug)
	ADD_INTROS_ITEM(filename)
	ADD_INTROS_ITEM(level)
	ADD_INTROS_ITEM_USER_NAME(modules, MAKE_USER_NAME("module", "modules", false))
END_INTROS_TYPE(debug)

bool operator==(const debug& lhs, const debug& rhs)
{
	return lhs.filename == rhs.filename && lhs.level == rhs.level && lhs.modules == rhs.modules;
}

BOOST_AUTO_TEST_CASE(test_load_xml)
{
	ptree tree;
	read_xml(get_test_file_full_path("debug_settings.xml"), tree);

	auto in = make_intros_object<debug>(tree);

	vector<string> res{ "Finance", "Admin", "HR" };

	BOOST_CHECK(in.filename == "debug.log" && in.level == 2 && in.modules == res);

	auto tree2 = make_ptree(in);

	auto out = make_intros_object<debug>(tree2);

	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(test_books_xml)
{
	ptree tree;
	read_xml(get_test_file_full_path("books.xml"), tree);

	auto in = make_intros_object<catalog>(tree);

	auto actual_value = get_actual_value();

	BOOST_CHECK(in == actual_value);

	ptree tree2 = make_ptree(in);

	auto out = make_intros_object<catalog>(tree2);

	BOOST_CHECK(in == out);
}
