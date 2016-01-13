#pragma once

#include <boost\property_tree\ptree.hpp>
#include "details\intros_struct_details.hpp"

#define INTROS_STRUCT(struct_name, ...)	\
	INTROS_STRUCT_IMPL(struct_name, __VA_ARGS__)

namespace utils
{
	template<typename T>
	boost::property_tree::ptree struct_to_ptree(const T& in)
	{
		return details::struct_to_ptree_impl(in);
	}

	template<typename T>
	void struct_from_ptree(T& out, const boost::property_tree::ptree& tree)
	{
		return details::struct_from_ptree_impl(out, tree);
	}
}
