#pragma once

#include <boost\property_tree\ptree.hpp>
#include "details\intros_struct_details.hpp"

#define INTROS_STRUCT(struct_name, ...)	\
	INTROS_STRUCT_IMPL(struct_name, __VA_ARGS__)

#define INTROS_STRUCT_USER_NAME(struct_name, ...) \
	INTROS_STRUCT_USER_NAME_IMPL(struct_name, __VA_ARGS__)

#define INTROS_ITEM(item, name)	\
	GET_ITEM_DESCRIPTION(item, name, utils::details::get_item_name_intermediate(name, utils::details::item_category<decltype(item)>()))

#define INTROS_ITEM_INTERMEDIATE_NAME(item, name, intermediate_name)	\
	GET_ITEM_DESCRIPTION(item, name, intermediate_name)

namespace utils
{
	template<typename T>
	auto make_intros_item(T& x, const std::string& name)
	{
		item_description<T>{name, "", x};
	}

	template<typename... T>
	auto make_intros_struct(const std::string& struct_name, T... params)
	{
		return struct_introspection<decltype(params)...>{struct_name, std::tuple<decltype(params)...>{params...}};
	}

	template<typename T>
	boost::property_tree::ptree struct_to_ptree(const T& in)
	{
		return details::struct_to_ptree_impl(in);
	}

	template<typename T>
	void struct_from_ptree(T& out, const boost::property_tree::ptree& tree)
	{
		details::struct_from_ptree_impl("", out, tree);
	}
}
