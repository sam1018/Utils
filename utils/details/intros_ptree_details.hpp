#pragma once

#include <tuple>
#include <string>
#include <algorithm>
#include <type_traits>
#include <boost\property_tree\ptree.hpp>
#include <boost\fusion\adapted\std_tuple.hpp>
#include <boost\fusion\algorithm\iteration\for_each.hpp>
#include "intros_type.hpp"
#include "util_traits.hpp"
#include "is_valid_expression.hpp"

// item tag conditions
namespace utils { namespace intros_ptree { namespace details
{
	namespace tags
	{
		//template<typename T>
		//using is_streamable_check = std::void_t<
		//	decltype(std::declval<std::istream&>() >> std::declval<T&>()),
		//	decltype(std::declval<std::ostream&>() << std::declval<T&>())
		//>;

		//template<typename T>
		//using has_fwd_iter_check = std::enable_if_t<
		//	std::is_base_of<
		//	std::forward_iterator_tag,
		//	typename std::iterator_traits<typename T::iterator>::iterator_category
		//	>::value
		//>;

		//template<typename T>
		//using has_input_iterator_check = std::enable_if_t<
		//	std::is_base_of<
		//		std::input_iterator_tag,
		//		typename std::iterator_traits<typename T::iterator>::iterator_category
		//	>::value
		//>;

		// item tags
		struct item_has_intros {};
		struct item_is_array {};
		//struct item_is_streamable {};
		//struct item_has_forward_iterator {};
		struct item_can_stream_insert {};
		struct item_has_input_iterator {};
		struct item_can_stream_extract {};
		struct item_can_insert_at_end {};
		struct item_not_supported {};

		// After defining both tags and tag conditions, now we are ready to
		// define our get_tag alias
		//template<typename T>
		//using item_category = traits::get_tag<
		//	has_intros<T>, item_has_intros,
		//	std::is_array<T>, item_is_array,
		//	is_streamable<T>, item_is_streamable,
		//	has_forward_iterator<T>, item_has_forward_iterator,
		//	item_not_supported
		//>;
	}

	//template<typename T>
	//using has_fwd_iter_check = std::enable_if_t<
	//	utils::traits::conjunction<
	//	std::is_base_of<
	//	std::input_iterator_tag,
	//	typename std::iterator_traits<typename T::iterator>::iterator_category
	//	>,
	//	std::is_base_of<
	//	std::output_iterator_tag,
	//	typename std::iterator_traits<typename T::iterator>::iterator_category
	//	>
	//	>::value
	//>;

	template<typename T>
	using has_input_iterator_check = std::enable_if_t<
		std::is_base_of<
		std::input_iterator_tag,
		typename std::iterator_traits<typename T::iterator>::iterator_category
		>::value>;

	IMPLEMENT_EXPR_TEST(has_intros, decltype(get_intros_type(std::declval<T&>())), T)
	//IMPLEMENT_EXPR_TEST_INDIRECT(is_streamable, is_streamable_check, T)
	//IMPLEMENT_EXPR_TEST_INDIRECT(has_forward_iterator, has_fwd_iter_check, T)
	IMPLEMENT_EXPR_TEST(can_stream_insert, decltype(std::declval<std::ostream&>() << std::declval<T&>()), T)
	IMPLEMENT_EXPR_TEST(can_stream_extract, decltype(std::declval<std::istream&>() >> std::declval<T&>()), T)
	IMPLEMENT_EXPR_TEST_INDIRECT(has_input_iterator, has_input_iterator_check, T)
	IMPLEMENT_EXPR_TEST(can_insert_at_end, decltype(std::inserter(std::declval<T&>(), std::end(std::declval<T&>()))), T)

	template<typename T>
	using item_category_read_intros = traits::get_tag<
		has_intros<T>,			tags::item_has_intros,
		std::is_array<T>,		tags::item_is_array,
		can_stream_insert<T>,	tags::item_can_stream_insert,
		has_input_iterator<T>,	tags::item_has_input_iterator,
		tags::item_not_supported
	>;

	template<typename T>
	using item_category_write_intros = traits::get_tag<
		has_intros<T>,			tags::item_has_intros,
		std::is_array<T>,		tags::item_is_array,
		can_stream_extract<T>,	tags::item_can_stream_extract,
		can_insert_at_end<T>,	tags::item_can_insert_at_end,
		tags::item_not_supported
	>;

	namespace add_to_tree
	{
		template<typename T>
		void add_to_tree_impl(boost::property_tree::ptree& tree, const std::string& name, const T& val, tags::item_has_intros)
		{
			boost::property_tree::ptree subtree;
			intros_to_ptree_impl(subtree, get_intros_type(val));
			tree.add_child(name, subtree);
		}
		template<typename T>
		void add_to_tree_impl(boost::property_tree::ptree& tree, const std::string& name, const T& val, tags::item_is_array)
		{
			static_assert(false, "Array not supported for intros_type");
		}
		template<typename T>
		void add_to_tree_impl(boost::property_tree::ptree& tree, const std::string& name, const T& val, tags::item_can_stream_insert)
		{
			tree.add(name, val);
		}
		template<typename T>
		void add_to_tree_impl(boost::property_tree::ptree& tree, const std::string& name, const T& val, tags::item_has_input_iterator)
		{
			std::for_each(val.begin(), val.end(), [&val, &tree, &name](const auto& x) {
				//boost::property_tree::ptree subtree;
				add_to_tree_impl(tree, name, x, item_category_read_intros<T::value_type>());
				//tree.add_child(name, subtree);
			});
		}
		template<typename T>
		void add_to_tree_impl(boost::property_tree::ptree& tree, const std::string& name, const T& val, tags::item_not_supported)
		{
			static_assert(false, "Unsupported type for intros_type");
		}
		template<typename T>
		void add_to_tree(boost::property_tree::ptree& tree, const T& x)
		{
			using t = std::remove_reference_t<decltype(std::remove_reference_t<T>::val)>;
			add_to_tree_impl(tree, x.name, x.val, item_category_read_intros<t>());
		}
	}

	namespace add_to_intros
	{
		template<typename T>
		T get_path_last_but(T path, std::string& last_path)
		{
			boost::property_tree::path last_but;

			while (!path.single())
				last_but /= path.reduce();

			last_path = path.reduce();

			return last_but;
		}

		template<typename T>
		void add_to_intros_impl(T& val, const std::string& name, const boost::property_tree::ptree& tree, tags::item_has_intros)
		{
			intros_from_ptree_impl(val,
				(name == "" ? tree : tree.get_child(name)));
		}
		template<typename T>
		void add_to_intros_impl(T& val, const std::string& name, const boost::property_tree::ptree& tree, tags::item_is_array)
		{
			static_assert(false, "Array not supported for intros_type");
		}
		template<typename T>
		void add_to_intros_impl(T& val, const std::string& name, const boost::property_tree::ptree& tree, tags::item_can_stream_extract)
		{
			val = tree.get<T>(name);
		}
		template<typename T>
		void add_to_intros_impl(T& val, const std::string& name, const boost::property_tree::ptree& tree, tags::item_can_insert_at_end)
		{
			std::string last_path;
			auto source = tree.get_child(get_path_last_but(boost::property_tree::path(name), last_path));

			//int n = source.count(last_path);
			//val.resize(source.count(last_path));

			//auto dest_it = val.begin();

			for (auto source_it = source.begin(); source_it != source.end(); ++source_it)
			{
				if (source_it->first == last_path)
				{
					add_to_intros_impl(*std::inserter(val, val.end()), "", source_it->second, item_category_write_intros<T::value_type>());
				}
			}
		}
		template<typename T>
		void add_to_intros_impl(T& val, const std::string& name, const boost::property_tree::ptree& tree, tags::item_not_supported)
		{
			static_assert(false, "Unsupported type for intros_type");
		}
		template<typename T>
		void add_to_intros(T& x, const boost::property_tree::ptree& tree)
		{
			using t = remove_reference_t<decltype(remove_reference_t<T>::val)>;
			add_to_intros_impl(x.val, x.name, tree, item_category_write_intros<t>());
		}
	}

	template<typename T>
	void intros_to_ptree_impl(boost::property_tree::ptree& tree, const T& in)
	{
		boost::fusion::for_each(in.items,
			[&tree](auto& x) {
			add_to_tree::add_to_tree(tree, x);
		});
	}

	template<typename T>
	void intros_from_ptree_impl(T& out, const boost::property_tree::ptree& tree)
	{
		boost::fusion::for_each(get_intros_type(out).items,
			[&tree](auto& x) {
			add_to_intros::add_to_intros(x, tree); });
	}
}}}
