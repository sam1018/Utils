#pragma once


#include "details\intros_ptree_details.hpp"
#include <boost\property_tree\ptree.hpp>


#define MAKE_USER_NAME(name, intermediate_name, is_attribute)	\
	((boost::property_tree::path(intermediate_name) /= (is_attribute ? "<xmlattr>" : "")) /= name).dump()


namespace utils
{
	// INTROS_PTREE FLAGS
	//namespace IPF
	//{
	//	constexpr int ATTRIBUTE = 0x01;
	//}

	template<typename T, std::enable_if_t<!has_intros<T>::value, int> = 0>
	boost::property_tree::ptree intros_to_ptree(T)
	{
		static_assert(0, "Use intros_type for introspection support");
	}

	template<typename T, std::enable_if_t<has_intros<T>::value, int> = 0>
	boost::property_tree::ptree intros_to_ptree(const T& in)
	{
		auto intros_type = get_intros_type_c(in);
		boost::property_tree::ptree tree, subtree;

		details::intros_to_ptree_impl(subtree, intros_type);

		tree.add_child(intros_type.type_name, subtree);
		return tree;
	}


	template<typename T, std::enable_if_t<!has_intros<T>::value, int> = 0>
	void intros_from_ptree(T& out, const boost::property_tree::ptree& tree)
	{
		static_assert(0, "Use intros_type for introspection support");
	}

	template<typename T, std::enable_if_t<has_intros<T>::value, int> = 0>
	void intros_from_ptree(T& out, const boost::property_tree::ptree& tree)
	{
		details::intros_from_ptree_impl(out, 
			tree.get_child(get_intros_type(out).type_name));
	}
}
