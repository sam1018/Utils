#pragma once

#include "details\is_valid_expression_details.hpp"

#define IMPLEMENT_EXPR_TEST(name, test, ...)	\
	IMPLEMENT_EXPR_TEST_IMPL(name, , test, __VA_ARGS__)

#define IMPLEMENT_EXPR_TEST_WITH_BASE(name, base, test, ...)	\
	IMPLEMENT_EXPR_TEST_IMPL(name, GET_BASE(base), test, __VA_ARGS__)

#define IMPLEMENT_EXPR_TEST_INDIRECT(name, ind_test, ...)	\
	IMPLEMENT_EXPR_TEST_IMPL(name, , GET_TEST(ind_test, __VA_ARGS__), __VA_ARGS__)

#define IMPLEMENT_EXPR_TEST_WITH_BASE_INDIRECT(name, base, ind_test, ...)	\
	IMPLEMENT_EXPR_TEST_IMPL(name, GET_BASE(base), GET_TEST(ind_test, __VA_ARGS__), __VA_ARGS__)
