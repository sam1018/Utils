#pragma once


#include "details\intros_ptree_details.hpp"
#include <boost\property_tree\ptree.hpp>


#define MAKE_USER_NAME(name, scope, is_attribute)		\
	(													\
		(												\
			boost::property_tree::path(scope)			\
				/= (is_attribute ? "<xmlattr>" : ""))	\
				/= name									\
	).dump()


namespace utils { namespace intros_ptree 
{
	template<typename T, std::enable_if_t<!details::has_intros<T>::value, int> = 0>
	boost::property_tree::ptree make_ptree(T)
	{
		static_assert(0, "Use intros_type for introspection support");
	}

	template<typename T, std::enable_if_t<details::has_intros<T>::value, int> = 0>
	boost::property_tree::ptree make_ptree(const T& in)
	{
		auto intros_type = get_intros_type(in);
		boost::property_tree::ptree tree, subtree;

		details::intros_to_ptree_impl(subtree, intros_type);

		tree.add_child(intros_type.name, subtree);
		return tree;
	}

	//template<typename T, std::enable_if_t<!details::has_intros<T>::value, int> = 0>
	//T make_intros_object(const boost::property_tree::ptree& tree)
	//{
	//	static_assert(0, "Use intros_type for introspection support");
	//}

	//template<typename T, std::enable_if_t<details::has_intros<T>::value, int> = 0>
	//T make_intros_object(const boost::property_tree::ptree& tree)
	//{
	//	T ret;

	//	auto& ret_intros = get_intros_type(ret);

	//	details::intros_from_ptree_impl(ret_intros.items, tree.get_child(ret_intros.name));

	//	return ret;
	//}

	template<typename T>
	T make_intros_object(const boost::property_tree::ptree& tree)
	{
		return details::make_intros_object_impl<T>(tree, details::object_write_category<T>());
	}
}}
