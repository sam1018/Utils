#pragma once


#include <tuple>
#include <string>
#include <type_traits>
#include <boost\preprocessor.hpp>
#include "is_valid_expression.hpp"
#include "util_traits.hpp"


//#define BEGIN_INTROS_TYPE(type_name)														\
//	auto get_intros_type() {																\
//		return utils::make_intros_type(type_name


#define BEGIN_INTROS_TYPE(type)														\
	BEBING_INTROS_TYPE_USER_NAME(type, BOOST_PP_STRINGIZE(type))

#define BEBING_INTROS_TYPE_USER_NAME(type, name)	\
	auto get_intros_type(type& val) {																\
		return make_intros_type(name

#define END_INTROS_TYPE(type)																	\
	); } \
	auto get_intros_type_c(const type& val)	{	\
		using ret_type = decltype(get_intros_type(type()));	\
		return static_cast<const ret_type>(get_intros_type(const_cast<type&>(val)));	\
}


//namespace utils
//{
//	template<typename T>
//	auto get_intros_type(T)
//	{
//		static_cast(false, "intros type not supported for this type");
//	}
//}

//template<typename T>
//using non_const_get_intros_type_test = std::void_t<
//	decltype(utils::get_intros_type(std::declval<std::remove_const_t<T>&>()))
//>;
//
//IMPLEMENT_EXPR_TEST(has_non_const_get_intros_type, non_const_get_intros_type_test, T)
//
//template<typename T>
//using const_get_intros_type_test = utils::conjunction<
//	std::is_const<T>,
//	utils::has_non_const_get_intros_type<T>
//>;
//
//namespace utils
//{
//	template<typename T, std::enable_if_t<const_get_intros_type_test<T>::value, int> = 0>
//	auto get_intros_type(T& val)
//	{
//		auto f = [&val]() {
//			return get_intros_type(const_cast<T&>(val)); };
//
//		return static_cast<std::add_const_t<decltype(f())>>(f());
//	}
//}


//#define ADD_INTROS_ITEM(x)																	\
//	ADD_INTROS_ITEM_USER_NAME(x, BOOST_PP_STRINGIZE(x))
//
//
//#define ADD_INTROS_ITEM_USER_NAME(x, name)													\
//	ADD_INTROS_ITEM_USER_NAME_INTERMEDIATE_NAME(x, name, "")
//
//
//#define ADD_INTROS_ITEM_USER_NAME_INTERMEDIATE_NAME(x, name, intermediate_name)				\
//	ADD_INTROS_ITEM_USER_NAME_INTERMEDIATE_NAME_FLAG(x, name, intermediate_name, 0)
//
//
//#define ADD_INTROS_ITEM_USER_NAME_INTERMEDIATE_NAME_FLAG(x, name, intermediate_name, flags)	\
//	, utils::make_intros_item(x, name, intermediate_name, flags)

#define ADD_INTROS_ITEM(x)	\
	ADD_INTROS_ITEM_USER_NAME(x, BOOST_PP_STRINGIZE(x))

#define ADD_INTROS_ITEM_USER_NAME(x, name)	\
	, utils::make_intros_item(val.x, name)


namespace utils
{
	template<typename... TupleItems>
	struct intros_type
	{
		std::string type_name;
		std::tuple<TupleItems...> items;
	};


	template<typename T>
	struct intros_item
	{
		T& val;
		std::string name;
		//std::string intermediate_name;
		//int flags;
	};

	template<typename T>
	auto make_intros_item(T& x, const std::string& name/*, const std::string& intermediate_name, int flags*/)
	{
		return intros_item<T>{x, name/*, intermediate_name, flags*/};
	}

	template<typename... T>
	auto make_intros_type(const std::string& type_name, T... params)
	{
		return intros_type<decltype(params)...>{type_name, std::tuple<decltype(params)...>{params...}};
	}
}
