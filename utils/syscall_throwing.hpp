#pragma once

#include <functional>
#include <type_traits>

namespace utils
{
	namespace details
	{
		template<typename FuncType, typename... ArgTypes>
		auto func_caller_impl(FuncType&& f, ArgTypes&&... args)
		{
			return std::invoke(std::forward<FuncType>(f), std::forward<ArgTypes>(args)...);
		}

		template<typename RetType, typename FuncType, typename... ArgTypes>
		class func_caller 
		{
			RetType retval;

		public:
			func_caller(FuncType&& f, ArgTypes&&... args)
			{
				retval = func_caller_impl(f, args...);
			}
			RetType get_retval()
			{
				return retval;
			}
		};

		template<typename FuncType, typename... ArgTypes>
		class func_caller<void, FuncType, ArgTypes...>
		{
		public:
			func_caller(FuncType&& f, ArgTypes&&... args)
			{
				func_caller_impl(f, args...);
			}
			void get_retval()
			{
			}
		};
	}

	template<typename FuncType, typename... ArgTypes>
	auto syscall_throwing(const std::string& err_msg, FuncType&& f, ArgTypes&&... args)
	{
		SetLastError(0);

		using RetType = std::result_of_t<FuncType&& (ArgTypes&&...)>;

		auto ret = details::func_caller<RetType, FuncType, ArgTypes...>(f, args...);

		if (auto err = GetLastError())
			throw std::system_error(std::error_code(err, std::system_category()), err_msg);

		return ret.get_retval();
	}
}
