#include <boost\type_traits.hpp>
#include <boost\preprocessor.hpp>
#include <boost\property_tree\ptree.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <tuple>
#include <string>
#include <algorithm>
#include <type_traits>

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

#define EXPAND_TUPLE_ITEMS(struct_name, ...)							\
BOOST_PP_SEQ_ENUM(														\
	BOOST_PP_SEQ_TRANSFORM(												\
		ADD_ITEM_X, struct_name, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

#define EXPAND_TUPLE_TYPES(...)								\
BOOST_PP_SEQ_ENUM(											\
	BOOST_PP_SEQ_TRANSFORM(									\
		GET_TYPE, ~, BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__)))

#define INTROS_STRUCT(struct_name, ...)					\
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

		//template<class T, class = std::void_t<>>
		//struct has_intros : std::false_type {};

		//template<class T>
		//struct has_intros<T, std::void_t<decltype(&T::intros_struct)>> : std::true_type {};

		template<typename T, typename = decltype(&T::intros_struct)> std::true_type has_intros_impl2(T);
		template<typename T> decltype(has_intros_impl2(T())) has_intros_impl(void*);
		template<typename T> std::false_type has_intros_impl(...);
		template<typename T> struct has_intros { static constexpr bool value = decltype(has_intros_impl<T>(nullptr))::value; };

		struct item_has_intros {};
		struct item_not_supported {};

		template<typename T>
		using item_category = typename 
			boost::conditional<has_intros<T>::value, item_has_intros, item_not_supported>::type;

		//template<typename T/*, std::enable_if_t <has_iterator<T>::value, int> = 0*/>
		//void write_item_impl(const std::string& prefix, const std::string& name, const T& val, boost::property_tree::ptree& tree, item_iterable)
		//{
		//	std::for_each(val.begin(), val.end(), 
		//		[path = make_path(prefix, array_prefix + name, name), &tree](const auto& x) {
		//		tree.add(path, x); }
		//	);
		//}

		//template<typename T/*, std::enable_if_t<!has_iterator<T>::value, int> = 0*/>
		//void write_item_impl(const std::string& prefix, const std::string& name, const T& val, boost::property_tree::ptree& tree)
		//{
		//	tree.put(make_path(prefix, name), val);
		//}

		//void write_item_impl(const std::string& prefix, const std::string& name, const std::string& val, boost::property_tree::ptree& tree)
		//{
		//	tree.put(make_path(prefix, name), val);
		//}

		//void write_item_impl(...) {}

		template<typename T>
		void write_item(const std::string& prefix, T& x, boost::property_tree::ptree& tree)
		{
			//typename item_category<T> tag;
			//write_item_impl(prefix, x.name, x.val, tree, tag);
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
	}

	template<typename T, std::enable_if_t<!has_intros_struct<T>::value, int> = 0>
	boost::property_tree::ptree struct_to_ptree(T)
	{
		static_assert(0, "Use INTROS_STRUCT for introspection support");
	}

	template<typename T, std::enable_if_t<has_intros_struct<T>::value, int> = 0>
	boost::property_tree::ptree struct_to_ptree(const T& in)
	{
		boost::property_tree::ptree tree;

		boost::fusion::for_each(in.intros_struct.items, [&tree, prefix = in.intros_struct.struct_name](auto& x) {
			details::write_item(prefix, x, tree);
		});

		return tree;
	}

	template<typename T, std::enable_if_t<has_intros_struct<T>::value, int> = 0>
	void struct_from_ptree(T& out, const boost::property_tree::ptree& tree)
	{
		boost::fusion::for_each(out.intros_struct.items, [&tree, prefix = out.intros_struct.struct_name](auto& x) {
			details::read_item(prefix, x, tree); });
	}
}
