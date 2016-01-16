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


IMPLEMENT_EXPR_TEST(has_intros, decltype(get_intros_type(declval<T&>())), T)
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


namespace utils
{
	namespace details
	{
		//template<typename T>
		//boost::property_tree::path get_path(boost::property_tree::path cur_path, T& x)
		//{
		//	cur_path /= x.intermediate_name;
		//	if (x.flags & IPF::ATTRIBUTE)
		//		cur_path /= "<xmlattr>";

		//	return cur_path;
		//}

		//namespace write_to_tree
		//{
		//	template<typename T>
		//	void write_to_tree_impl(boost::property_tree::path cur_path, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_has_intros)
		//	{
		//		intros_to_ptree_impl(cur_path /= name, tree, const_cast<T&>(val).get_intros_type());
		//	}

		//	template<typename T>
		//	void write_to_tree_impl(boost::property_tree::path cur_path, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_is_array)
		//	{
		//		static_assert(false, "Array not supported for intros_type");
		//	}

		//	template<typename T>
		//	void write_to_tree_impl(boost::property_tree::path cur_path, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_is_streamable)
		//	{
		//		tree.add(cur_path /= name, val);
		//	}

		//	template<typename T>
		//	void write_to_tree_impl(boost::property_tree::path cur_path, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_has_forward_iterator)
		//	{
		//		std::for_each(val.begin(), val.end(),
		//			[&name, &cur_path, &tree](const auto& x) {
		//				write_to_tree_impl(cur_path, name, x, tree, item_category<T::value_type>());
		//			}
		//		);
		//	}

		//	template<typename T>
		//	void write_to_tree_impl(boost::property_tree::path cur_path, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_not_supported)
		//	{
		//		static_assert(false, "Unsupported type for intros_type");
		//	}

		//	template<typename T>
		//	void write_to_tree(boost::property_tree::path cur_path, const T& x, boost::property_tree::ptree& tree)
		//	{
		//		using t = remove_reference_t<decltype(remove_reference_t<T>::val)>;
		//		write_to_tree_impl(get_path(cur_path, x), x.name, x.val, tree, item_category<t>());
		//	}
		//}

		namespace add_to_tree
		{
			template<typename T>
			void add_to_tree_impl(boost::property_tree::ptree& tree, const std::string& name, const T& val, item_has_intros)
			{
				boost::property_tree::ptree subtree;
				intros_to_ptree_impl(subtree, get_intros_type_c(val));
				tree.add_child(name, subtree);
			}
			template<typename T>
			void add_to_tree_impl(boost::property_tree::ptree& tree, const std::string& name, const T& val, item_is_array)
			{
				static_assert(false, "Array not supported for intros_type");
			}
			template<typename T>
			void add_to_tree_impl(boost::property_tree::ptree& tree, const std::string& name, const T& val, item_is_streamable)
			{
				tree.add(name, val);
			}
			template<typename T>
			void add_to_tree_impl(boost::property_tree::ptree& tree, const std::string& name, const T& val, item_has_forward_iterator)
			{
				std::for_each(val.begin(), val.end(), [&val, &tree, &name](const auto& x) {
					//boost::property_tree::ptree subtree;
					add_to_tree_impl(tree, name, x, item_category<T::value_type>());
					//tree.add_child(name, subtree);
				});
			}
			template<typename T>
			void add_to_tree_impl(boost::property_tree::ptree& tree, const std::string& name, const T& val, item_not_supported)
			{
				static_assert(false, "Unsupported type for intros_type");
			}
			template<typename T>
			void add_to_tree(boost::property_tree::ptree& tree, const T& x)
			{
				using t = remove_reference_t<decltype(remove_reference_t<T>::val)>;
				add_to_tree_impl(tree, x.name, x.val, item_category<t>());
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
			void add_to_intros_impl(T& val, const std::string& name, const boost::property_tree::ptree& tree, item_has_intros)
			{
				intros_from_ptree_impl(val, 
					(name == "" ? tree : tree.get_child(name)));
			}
			template<typename T>
			void add_to_intros_impl(T& val, const std::string& name, const boost::property_tree::ptree& tree, item_is_array)
			{
				static_assert(false, "Array not supported for intros_type");
			}
			template<typename T>
			void add_to_intros_impl(T& val, const std::string& name, const boost::property_tree::ptree& tree, item_is_streamable)
			{
				val = tree.get<T>(name);
			}
			template<typename T>
			void add_to_intros_impl(T& val, const std::string& name, const boost::property_tree::ptree& tree, item_has_forward_iterator)
			{
				std::string last_path;
				auto source = tree.get_child(get_path_last_but(boost::property_tree::path(name), last_path));

				int n = source.count(last_path);
				val.resize(source.count(last_path));

				auto dest_it = val.begin();

				for (auto source_it = source.begin(); source_it != source.end(); ++source_it)
				{
					stringstream ss;
					write_xml(ss, source_it->second);
					string s = ss.str();
					if (source_it->first == last_path)
					{
						add_to_intros_impl(*dest_it++, "", source_it->second, item_category<T::value_type>());
					}
				}
			}
			template<typename T>
			void add_to_intros_impl(T& val, const std::string& name, const boost::property_tree::ptree& tree, item_not_supported)
			{
				static_assert(false, "Unsupported type for intros_type");
			}
			template<typename T>
			void add_to_intros(T& x, const boost::property_tree::ptree& tree)
			{
				using t = remove_reference_t<decltype(remove_reference_t<T>::val)>;
				add_to_intros_impl(x.val, x.name, tree, item_category<t>());
			}
		}


		//namespace write_to_intros
		//{
		//	template<typename T>
		//	void write_to_intros_impl(boost::property_tree::path cur_path, const std::string& name, T& val, const boost::property_tree::ptree& tree, item_has_intros)
		//	{
		//		intros_from_ptree_impl(cur_path /= name, val, tree);
		//	}

		//	template<typename T>
		//	void write_to_intros_impl(boost::property_tree::path cur_path, const std::string& name, T& val, const boost::property_tree::ptree& tree, item_is_array)
		//	{
		//		static_assert(false, "Array not supported for intros_type");
		//	}

		//	template<typename T>
		//	void write_to_intros_impl(boost::property_tree::path cur_path, const std::string& name, T& val, const boost::property_tree::ptree& tree, item_is_streamable)
		//	{
		//		stringstream ss;
		//		write_xml(ss, tree);
		//		string s = ss.str();
		//		val = tree.get<T>(cur_path /= name);
		//	}

		//	template<typename T>
		//	void write_to_intros_impl(boost::property_tree::path cur_path, const std::string& name, T& val, const boost::property_tree::ptree& tree, item_has_forward_iterator)
		//	{
		//		auto& source = tree.get_child(cur_path);

		//		val.resize(source.size());

		//		auto source_it = tree.begin();
		//		auto dest_it = val.begin();

		//		stringstream ss;
		//		write_xml(ss, tree);
		//		string s = ss.str();

		//		for (; source_it != source.end(); ++source_it, ++dest_it)
		//		{
		//			stringstream ss;
		//			write_xml(ss, source_it->second);
		//			string s = ss.str();

		//			string s2 = source_it->first;

		//			if(source_it->first == cur_path.dump())
		//				write_to_intros_impl("", name, *dest_it, source_it->second, item_category<T::value_type>());
		//		}
		//	}

		//	template<typename T>
		//	void write_to_intros_impl(boost::property_tree::path cur_path, const std::string& name, T& val, const boost::property_tree::ptree& tree, item_not_supported)
		//	{
		//		static_assert(false, "Unsupported type for intros_type");
		//	}

		//	template<typename T>
		//	void write_to_intros(boost::property_tree::path cur_path, T& x, const boost::property_tree::ptree& tree)
		//	{
		//		using t = remove_reference_t<decltype(remove_reference_t<T>::val)>;
		//		write_to_intros_impl(get_path(cur_path, x), x.name, x.val, tree, item_category<t>());
		//	}
		//}


		//template<typename T, std::enable_if_t<has_intros<T>::value, int> = 0>
		//boost::property_tree::ptree intros_to_ptree_impl(const T& in)
		//{
		//	boost::property_tree::ptree tree;

		//	intros_to_ptree_impl("", tree, const_cast<T&>(in).get_intros_type());

		//	return tree;
		//}

		//template<typename T>
		//boost::property_tree::ptree intros_to_ptree_impl(const T& in)
		//{
		//	boost::property_tree::ptree tree;

		//	intros_to_ptree_impl(tree, const_cast<T&>(in).get_intros_type());

		//	return tree;
		//}

		//template<typename T>
		//boost::property_tree::ptree intros_to_ptree_impl(boost::property_tree::path cur_path, boost::property_tree::ptree& tree, const T& in)
		//{
		//	cur_path /= in.type_name;

		//	boost::fusion::for_each(in.items,
		//		[&tree, &cur_path](auto& x) {
		//		write_to_tree::write_to_tree(cur_path, x, tree);
		//	});

		//	return tree;
		//}

		//template<typename T>
		//boost::property_tree::ptree intros_to_ptree_impl(boost::property_tree::ptree& tree, const T& in)
		//{
		//	boost::property_tree::ptree subtree;

		//	boost::fusion::for_each(in.items,
		//		[&subtree](auto& x) {
		//		add_to_tree::add_to_tree(subtree, x);
		//	});

		//	tree.add_child(in.type_name, subtree);

		//	return tree;
		//}

		template<typename T>
		void intros_to_ptree_impl(boost::property_tree::ptree& tree, const T& in)
		{
			boost::fusion::for_each(in.items,
				[&tree](auto& x) {
				add_to_tree::add_to_tree(tree, x);
			});
		}

		//template<typename T, std::enable_if_t<!has_intros<T>::value, int> = 0>
		//void intros_from_ptree_impl(boost::property_tree::path cur_path, T& out, const boost::property_tree::ptree& tree)
		//{
		//	static_assert(0, "Use intros_type for introspection support");
		//}

		//template<typename T, std::enable_if_t<has_intros<T>::value, int> = 0>
		//void intros_from_ptree_impl(boost::property_tree::path cur_path, T& out, const boost::property_tree::ptree& tree)
		//{
		//	cur_path /= out.get_intros_type().type_name;

		//	boost::fusion::for_each(out.get_intros_type().items,
		//		[&tree, &cur_path](auto& x) {
		//		write_to_intros::write_to_intros(cur_path, x, tree); });
		//}

		template<typename T>
		void intros_from_ptree_impl(T& out, const boost::property_tree::ptree& tree)
		{
			boost::fusion::for_each(get_intros_type(out).items,
				[&tree](auto& x) {
				add_to_intros::add_to_intros(x, tree); });
		}
	}
}
