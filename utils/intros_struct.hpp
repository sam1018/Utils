#pragma once

#include <boost\property_tree\ptree.hpp>
#include "details\intros_struct_details.hpp"

#define BEGIN_INTROS_STRUCT(struct_name)	\
	auto get_intros_struct() {				\
		return make_intros_struct(struct_name

#define END_INTROS_STRUCT()	\
	);}


#define ADD_INTROS_ITEM(x)	\
	, make_intros_item(x, BOOST_PP_STRINGIZE(x))

namespace utils
{
	template<typename T>
	auto make_intros_item(T& x, const std::string& name)
	{
		return intros_item<T>{
			name, 
			details::get_item_name_intermediate(name, utils::details::item_category<T>()), 
			x
		};
	}

	template<typename... T>
	auto make_intros_struct(const std::string& struct_name, T... params)
	{
		return intros_struct<decltype(params)...>{struct_name, std::tuple<decltype(params)...>{params...}};
	}

	template<typename T>
	boost::property_tree::ptree struct_to_ptree(T& in)
	{
		return details::struct_to_ptree_impl(in);
	}

	template<typename T>
	void struct_from_ptree(T& out, const boost::property_tree::ptree& tree)
	{
		details::struct_from_ptree_impl("", out, tree);
	}
}
