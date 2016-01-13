#pragma once

#include <tuple>
#include <string>
#include <algorithm>
#include <type_traits>
#include <boost\type_traits.hpp>
#include <boost\preprocessor.hpp>
#include <boost\property_tree\ptree.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include "util_traits.hpp"
#include "is_valid_expression.hpp"
#include "pp_variadic_transform.hpp"

// item tag conditions
namespace utils {
	namespace details
	{
		template<typename T>
		using is_streamable_check = std::void_t<
			decltype(std::declval<std::istream&>() >> std::declval<T&>()),
			decltype(std::declval<std::ostream&>() << std::declval<T&>())
		>;

		template<typename T>
		using has_fwd_iter_check = std::enable_if_t<
			std::is_base_of<
			std::forward_iterator_tag,
			typename std::iterator_traits<typename T::iterator>::iterator_category
			>::value
		>;
	}
}

IMPLEMENT_EXPR_TEST(has_intros, decltype(&T::intros_struct), T)
IMPLEMENT_EXPR_TEST_INDIRECT(is_streamable, is_streamable_check, T)
IMPLEMENT_EXPR_TEST_INDIRECT(has_forward_iterator, has_fwd_iter_check, T)


namespace utils {
	namespace details
	{
		// item tags
		struct item_has_intros {};
		struct item_is_array {};
		struct item_is_streamable {};
		struct item_has_forward_iterator {};
		struct item_not_supported {};

		// After defining both tags and tag conditions, now we are ready to
		// define our get_tag alias
		template<typename T>
		using item_category = get_tag<
			utils::has_intros<T>, item_has_intros,
			std::is_array<T>, item_is_array,
			utils::is_streamable<T>, item_is_streamable,
			utils::has_forward_iterator<T>, item_has_forward_iterator,
			item_not_supported
		>;
	}
}

template<typename... TupleItems>
struct struct_introspection
{
	std::string struct_name;
	std::tuple<TupleItems...> items;
};

template<typename T>
struct item_description
{
	std::string name;
	T& val;
};

#define GET_TYPE(ignore1, ignore2, val) item_description<decltype(val)>

#define ADD_ITEM(x, struct_name) (item_description<decltype(x)>{BOOST_PP_STRINGIZE(x), x})

#define ADD_ITEM_X(ignore1, struct_name, x) ADD_ITEM(x, struct_name)

#define EXPAND_TUPLE_ITEMS(struct_name, ...)	\
	UTILS_PP_VARIADIC_TRANSFORM(ADD_ITEM_X, struct_name, __VA_ARGS__)

#define EXPAND_TUPLE_TYPES(...)		\
	UTILS_PP_VARIADIC_TRANSFORM(GET_TYPE, ~, __VA_ARGS__)

#define INTROS_STRUCT_IMPL(struct_name, ...)			\
using intros_defined = int;								\
struct_introspection<EXPAND_TUPLE_TYPES(__VA_ARGS__)>	\
intros_struct											\
{														\
	struct_name,										\
	std::tuple<EXPAND_TUPLE_TYPES(__VA_ARGS__)>			\
		{EXPAND_TUPLE_ITEMS(struct_name, __VA_ARGS__)}	\
};

namespace utils
{
	template<class, class = std::void_t<>>
	struct has_intros_struct : std::false_type {};

	template<class T>
	struct has_intros_struct<T, std::void_t<typename T::intros_defined>> : std::true_type {};

	namespace details
	{
		constexpr auto array_prefix = "Array_";

		template<class, class = std::void_t<>>
		struct has_iterator : std::false_type {};

		template<class T>
		struct has_iterator<T, std::void_t<typename T::iterator>> : std::true_type {};

		template<typename... T>
		auto make_path(const std::string& start, const T&... prefix)
		{
			return boost::property_tree::path(start) /= make_path(prefix...);
		}

		auto make_path(const std::string& end)
		{
			return boost::property_tree::path(end);
		}

		template<typename T>
		void write_item_impl(const std::string& prefix, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_has_intros)
		{
			write_item(prefix, val, tree)
		}

		template<typename T>
		void write_item_impl(const std::string& prefix, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_is_array)
		{
			static_assert(false, "Array not supported for intros_struct");
		}

		template<typename T>
		void write_item_impl(const std::string& prefix, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_is_streamable)
		{
			tree.put(make_path(prefix, name), val);
		}

		template<typename T>
		void write_item_impl(const std::string& prefix, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_has_forward_iterator)
		{
			//std::for_each(val.begin(), val.end(), 
			//	[path = make_path(prefix, array_prefix + name, name), &tree](const auto& x) {
			//	tree.add(path, x); }
			//);

			std::for_each(val.begin(), val.end(), 
				[prefix_ = make_path(prefix, array_prefix + name), &tree, &name](const auto& x) {
				write_item(prefix_, name, tree);
				//tree.add(path, x); 
			}
			);
		}

		template<typename T>
		void write_item_impl(const std::string& prefix, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_not_supported)
		{
			static_assert(false, "Unsupported type for intros_struct");
		}

		template<typename T>
		void write_item(const std::string& prefix, T& x, boost::property_tree::ptree& tree)
		{
			using t = remove_reference_t<decltype(remove_reference_t<T>::val)>;
			write_item_impl(prefix, x.name, x.val, tree, item_category<t>());
		}

		template<typename T, std::enable_if_t<has_iterator<T>::value, int> = 0>
		void read_item_impl(const std::string& prefix, const std::string& name, T& val, const boost::property_tree::ptree& tree)
		{
			auto& source = tree.get_child(make_path(prefix, array_prefix + name));
			std::transform(source.begin(), source.end(), back_inserter(val), [](auto& x) {
				return x.second.BOOST_NESTED_TEMPLATE get_value<T::value_type>(); }
			);
		}

		template<typename T, std::enable_if_t<!has_iterator<T>::value, int> = 0>
		void read_item_impl(const std::string& prefix, const std::string& name, T& val, const boost::property_tree::ptree& tree)
		{
			val = tree.get<std::remove_reference_t<decltype(val)>>(make_path(prefix, name));
		}

		template<>
		void read_item_impl(const std::string& prefix, const std::string& name, std::string& val, const boost::property_tree::ptree& tree)
		{
			val = tree.get<std::remove_reference_t<decltype(val)>>(make_path(prefix, name));
		}

		template<typename T>
		void read_item(const std::string& prefix, T& x, const boost::property_tree::ptree& tree)
		{
			read_item_impl(prefix, x.name, x.val, tree);
		}

		template<typename T, std::enable_if_t<!has_intros_struct<T>::value, int> = 0>
		boost::property_tree::ptree struct_to_ptree_impl(T)
		{
			static_assert(0, "Use INTROS_STRUCT for introspection support");
		}

		template<typename T, std::enable_if_t<has_intros_struct<T>::value, int> = 0>
		boost::property_tree::ptree struct_to_ptree_impl(const T& in)
		{
			boost::property_tree::ptree tree;

			boost::fusion::for_each(in.intros_struct.items, 
				[&tree, prefix = in.intros_struct.struct_name](auto& x) {
				write_item(prefix, x, tree);
			});

			return tree;
		}

		template<typename T, std::enable_if_t<!has_intros_struct<T>::value, int> = 0>
		void struct_from_ptree_impl(T& out, const boost::property_tree::ptree& tree)
		{
			static_assert(0, "Use INTROS_STRUCT for introspection support");
		}

		template<typename T, std::enable_if_t<has_intros_struct<T>::value, int> = 0>
		void struct_from_ptree_impl(T& out, const boost::property_tree::ptree& tree)
		{
			boost::fusion::for_each(out.intros_struct.items, [&tree, prefix = out.intros_struct.struct_name](auto& x) {
				read_item(prefix, x, tree); });
		}
	}
}
