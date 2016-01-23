#include "stdafx.h"
#include "intros_ptree.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace std;
using namespace boost::property_tree;
using namespace utils::intros_ptree;


struct customer
{
	string id;
	string company_name;
	string contact_name;
	string contact_title;
	string phone;
	string fax;
	string address;
	string city;
	string region;
	int postal_code;
	string country;
};
BEGIN_INTROS_TYPE_USER_NAME(customer, "Customer")
	ADD_INTROS_ITEM_USER_NAME(id, MAKE_USER_NAME("CustomerID", "", true))
	ADD_INTROS_ITEM_USER_NAME(company_name, "CompanyName")
	ADD_INTROS_ITEM_USER_NAME(contact_name, "ContactName")
	ADD_INTROS_ITEM_USER_NAME(contact_title, "ContactTitle")
	ADD_INTROS_ITEM_USER_NAME(phone, "Phone")
	ADD_INTROS_ITEM_USER_NAME(fax, "Fax")
	ADD_INTROS_ITEM_USER_NAME(address, MAKE_USER_NAME("Address", "FullAddress", false))
	ADD_INTROS_ITEM_USER_NAME(city, MAKE_USER_NAME("City", "FullAddress", false))
	ADD_INTROS_ITEM_USER_NAME(region, MAKE_USER_NAME("Region", "FullAddress", false))
	ADD_INTROS_ITEM_USER_NAME(postal_code, MAKE_USER_NAME("PostalCode", "FullAddress", false))
	ADD_INTROS_ITEM_USER_NAME(country, MAKE_USER_NAME("Country", "FullAddress", false))
END_INTROS_TYPE(customer)

struct order
{
	string id;
	int employee_id;
	string order_date;
	string required_date;
	string shipped_date;
	int ship_via;
	float freight;
	string ship_name;
	string ship_address;
	string ship_city;
	string ship_region;
	int ship_postal_code;
	string ship_country;
};

BEGIN_INTROS_TYPE_USER_NAME(order, "Order")
	ADD_INTROS_ITEM_USER_NAME(id, "CustomerID")
	ADD_INTROS_ITEM_USER_NAME(employee_id, "EmployeeID")
	ADD_INTROS_ITEM_USER_NAME(order_date, "OrderDate")
	ADD_INTROS_ITEM_USER_NAME(required_date, "RequiredDate")
	ADD_INTROS_ITEM_USER_NAME(shipped_date, MAKE_USER_NAME("ShippedDate", "ShipInfo", true))
	ADD_INTROS_ITEM_USER_NAME(ship_via, MAKE_USER_NAME("ShipVia", "ShipInfo", false))
	ADD_INTROS_ITEM_USER_NAME(freight, MAKE_USER_NAME("Freight", "ShipInfo", false))
	ADD_INTROS_ITEM_USER_NAME(ship_name, MAKE_USER_NAME("ShipName", "ShipInfo", false))
	ADD_INTROS_ITEM_USER_NAME(ship_address, MAKE_USER_NAME("ShipAddress", "ShipInfo", false))
	ADD_INTROS_ITEM_USER_NAME(ship_city, MAKE_USER_NAME("ShipCity", "ShipInfo", false))
	ADD_INTROS_ITEM_USER_NAME(ship_region, MAKE_USER_NAME("ShipRegion", "ShipInfo", false))
	ADD_INTROS_ITEM_USER_NAME(ship_postal_code, MAKE_USER_NAME("ShipPostalCode", "ShipInfo", false))
	ADD_INTROS_ITEM_USER_NAME(ship_country, MAKE_USER_NAME("ShipCountry", "ShipInfo", false))
END_INTROS_TYPE(order)


struct customer_orders
{
	vector<customer> customers;
	vector<order> orders;
};

BEGIN_INTROS_TYPE_USER_NAME(customer_orders, "Root")
	ADD_INTROS_ITEM_USER_NAME(customers, MAKE_USER_NAME("Customer", "Customers", false))
	ADD_INTROS_ITEM_USER_NAME(orders, MAKE_USER_NAME("Order", "Orders", false))
END_INTROS_TYPE(customer_orders)


bool operator==(const customer& lhs, const customer& rhs)
{
	return
		lhs.id == rhs.id &&
		lhs.company_name == rhs.company_name &&
		lhs.contact_name == rhs.contact_name &&
		lhs.contact_title == rhs.contact_title &&
		lhs.phone == rhs.phone &&
		lhs.fax == rhs.fax &&
		lhs.address == rhs.address &&
		lhs.city == rhs.city &&
		lhs.region == rhs.region &&
		lhs.postal_code == rhs.postal_code &&
		lhs.country == rhs.country;
}


bool operator==(const order& lhs, const order& rhs)
{
	return
		lhs.id == rhs.id &&
		lhs.employee_id == rhs.employee_id &&
		lhs.order_date == rhs.order_date &&
		lhs.required_date == rhs.required_date &&
		lhs.shipped_date == rhs.shipped_date &&
		lhs.ship_via == rhs.ship_via &&
		lhs.freight == rhs.freight &&
		lhs.ship_name == rhs.ship_name &&
		lhs.ship_address == rhs.ship_address &&
		lhs.ship_city == rhs.ship_city &&
		lhs.ship_region == rhs.ship_region &&
		lhs.ship_postal_code == rhs.ship_postal_code &&
		lhs.ship_country == rhs.ship_country;
}


bool operator==(const customer_orders& lhs, const customer_orders& rhs)
{
	return
		lhs.customers == rhs.customers &&
		lhs.orders == rhs.orders;
}


customer_orders get_actual_data()
{
	customer c{ "HUNGC", "Hungry Coyote Import Store", "Yoshi Latimer", "Sales Representative", "(503) 555-6874", "(503) 555-2376", "City Center Plaza 516 Main St.", "Elgin", "OR", 97827, "USA" };
	order o{ "GREAL", 8, "1997-07-04T00:00:00", "1997-08-01T00:00:00", "1997-07-14T00:00:00", 2, 4.42f, "Great Lakes Food Market", "2732 Baker Blvd.", "Eugene", "OR", 97403, "USA" };

	return customer_orders{ { c },{ o } };
}

BOOST_AUTO_TEST_CASE(test_xml_test_small_customer_orders)
{
	ptree tree;
	read_xml(get_test_file_full_path("CustomerOrders_small.xml"), tree);

	auto in = make_intros_object<customer_orders>(tree);

	auto res = get_actual_data();

	BOOST_CHECK(in == res);

	auto tree2 = make_ptree(in);

	auto out = make_intros_object<customer_orders>(tree2);

	BOOST_CHECK(in == out);
}

BOOST_AUTO_TEST_CASE(test_xml_test_customer_orders)
{
	ptree tree;
	read_xml(get_test_file_full_path("CustomerOrders.xml"), tree);

	customer_orders in;
	try
	{
		in = make_intros_object<customer_orders>(tree);
	}
	catch (exception& e)
	{
		auto s = e.what();
		int x = 10;
	}

	auto tree2 = make_ptree(in);

	auto out = make_intros_object<customer_orders>(tree2);

	BOOST_CHECK(in == out);
}
