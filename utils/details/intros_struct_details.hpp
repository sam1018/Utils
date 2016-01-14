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

IMPLEMENT_EXPR_TEST(has_intros, decltype(&T::get_intros_struct), T)
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

		constexpr auto container_prefix = "";
		constexpr auto container_postfix = "s";

		auto get_container_name_decorated(const std::string& name)
		{
			return container_prefix + name + container_postfix;
		}

		std::string get_item_name_intermediate(const std::string& name, item_has_forward_iterator)
		{
			return get_container_name_decorated(name);
		}

		std::string get_item_name_intermediate(...)
		{
			return{};
		}
	}
}

template<typename... TupleItems>
struct intros_struct
{
	std::string struct_name;
	std::tuple<TupleItems...> items;
};

template<typename T>
struct intros_item
{
	std::string name;
	std::string intermediate_name;
	T& val;
};

namespace utils
{
	namespace details
	{
		template<class, class = std::void_t<>>
		struct has_iterator : std::false_type {};

		template<class T>
		struct has_iterator<T, std::void_t<typename T::iterator>> : std::true_type {};

		///////// Write implementation start

		template<typename T>
		void write_item_impl(boost::property_tree::path cur_path, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_has_intros)
		{
			struct_to_ptree_impl(cur_path /= name, tree, val.intros_struct);
		}

		template<typename T>
		void write_item_impl(boost::property_tree::path cur_path, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_is_array)
		{
			static_assert(false, "Array not supported for intros_struct");
		}

		template<typename T>
		void write_item_impl(boost::property_tree::path cur_path, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_is_streamable)
		{
			tree.add(cur_path /= name, val);
		}

		template<typename T>
		void write_item_impl(boost::property_tree::path cur_path, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_has_forward_iterator)
		{
			std::for_each(val.begin(), val.end(), 
				[&name, &cur_path, &tree](const auto& x) {
				write_item_impl(cur_path, name, x, tree, item_category<T::value_type>());
			}
			);
		}

		template<typename T>
		void write_item_impl(boost::property_tree::path cur_path, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_not_supported)
		{
			static_assert(false, "Unsupported type for intros_struct");
		}

		template<typename T>
		void write_item(boost::property_tree::path cur_path, T& x, boost::property_tree::ptree& tree)
		{
			using t = remove_reference_t<decltype(remove_reference_t<T>::val)>;
			write_item_impl(cur_path /= x.intermediate_name, x.name, x.val, tree, item_category<t>());
		}

		////////// Write implementation end



		///////// Read implementation start
		template<typename T>
		void read_item_impl(boost::property_tree::path cur_path, const std::string& name, T& val, const boost::property_tree::ptree& tree, item_has_intros)
		{
			struct_from_ptree_impl(cur_path /= name, val, tree);
		}

		template<typename T>
		void read_item_impl(boost::property_tree::path cur_path, const std::string& name, T& val, const boost::property_tree::ptree& tree, item_is_array)
		{
			static_assert(false, "Array not supported for intros_struct");
		}

		template<typename T>
		void read_item_impl(boost::property_tree::path cur_path, const std::string& name, T& val, const boost::property_tree::ptree& tree, item_is_streamable)
		{
			val = tree.get<T>(cur_path /= name);
		}

		template<typename T>
		void read_item_impl(boost::property_tree::path cur_path, const std::string& name, T& val, const boost::property_tree::ptree& tree, item_has_forward_iterator)
		{
			auto& source = tree.get_child(cur_path);

			val.resize(source.size());

			auto source_it = source.begin();
			auto dest_it = val.begin();
			for (; source_it != source.end(); ++source_it, ++dest_it)
				read_item_impl("", "", *dest_it, source_it->second, item_category<T::value_type>());
		}

		template<typename T>
		void read_item_impl(boost::property_tree::path cur_path, const std::string& name, T& val, const boost::property_tree::ptree& tree, item_not_supported)
		{
			static_assert(false, "Unsupported type for intros_struct");
		}

		template<typename T>
		void read_item(boost::property_tree::path cur_path, T& x, const boost::property_tree::ptree& tree)
		{
			using t = remove_reference_t<decltype(remove_reference_t<T>::val)>;
			read_item_impl(cur_path /= x.intermediate_name, x.name, x.val, tree, item_category<t>());
		}
		///////// Read implementation end

		template<typename T, std::enable_if_t<!has_intros<T>::value, int> = 0>
		boost::property_tree::ptree struct_to_ptree_impl(T)
		{
			static_assert(0, "Use INTROS_STRUCT for introspection support");
		}

		template<typename T, std::enable_if_t<has_intros<T>::value, int> = 0>
		boost::property_tree::ptree struct_to_ptree_impl(T& in)
		{
			boost::property_tree::ptree tree;

			struct_to_ptree_impl("", tree, in.get_intros_struct());

			return tree;
		}

		template<typename T>
		boost::property_tree::ptree struct_to_ptree_impl(boost::property_tree::path cur_path, boost::property_tree::ptree& tree, T& in)
		{
			cur_path /= in.struct_name;

			boost::fusion::for_each(in.items,
				[&tree, &cur_path](auto& x) {
				write_item(cur_path, x, tree);
			});

			return tree;
		}

		template<typename T, std::enable_if_t<!has_intros<T>::value, int> = 0>
		void struct_from_ptree_impl(boost::property_tree::path cur_path, T& out, const boost::property_tree::ptree& tree)
		{
			static_assert(0, "Use INTROS_STRUCT for introspection support");
		}

		template<typename T, std::enable_if_t<has_intros<T>::value, int> = 0>
		void struct_from_ptree_impl(boost::property_tree::path cur_path, T& out, const boost::property_tree::ptree& tree)
		{
			cur_path /= out.get_intros_struct().struct_name;

			boost::fusion::for_each(out.get_intros_struct().items,
				[&tree, &cur_path](auto& x) {
				read_item(cur_path, x, tree); });
		}
	}
}
