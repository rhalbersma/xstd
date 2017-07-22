#pragma once
#include <type_traits>  // void_t
#include <utility>      // declval

namespace xstd {
namespace tti {

template<class T, class = void>
constexpr static auto has_member_op_minus_assign_v = false;

template<class T>
constexpr static auto has_member_op_minus_assign_v<
        T, std::void_t<decltype(std::declval<T>() -= std::declval<T>())>
> = true;

template<class T, class = void>
constexpr static auto has_iterators_v = false;

template<class T>
constexpr static auto has_iterators_v<
        T, std::void_t<
                decltype(std::declval<T>(). begin()),
                decltype(std::declval<T>().   end()),
                decltype(std::declval<T>().cbegin()),
                decltype(std::declval<T>().  cend()),
                decltype( begin(std::declval<T>())),
                decltype(   end(std::declval<T>())),
                decltype(cbegin(std::declval<T>())),
                decltype(  cend(std::declval<T>()))
        >
> = true;

template<class T, class = void>
constexpr static auto has_reverse_iterators_v = false;

template<class T>
constexpr static auto has_reverse_iterators_v<
        T, std::void_t<
                decltype(std::declval<T>(). rbegin()),
                decltype(std::declval<T>().   rend()),
                decltype(std::declval<T>().crbegin()),
                decltype(std::declval<T>().  crend()),
                decltype( rbegin(std::declval<T>())),
                decltype(   rend(std::declval<T>())),
                decltype(crbegin(std::declval<T>())),
                decltype(  crend(std::declval<T>()))
        >
> = true;

template<class T, class = void>
constexpr static auto has_reverse_for_each_v = false;

// lambdas cannot appear in unevaluated contexts
// http://open-std.org/JTC1/SC22/WG21/docs/papers/2017/p0315r2.pdf
constexpr auto unary_function = [](auto) {};

template<class T>
constexpr static auto has_reverse_for_each_v<
        T, std::void_t<decltype(reverse_for_each(std::declval<T>(), unary_function))>
> = true;

template<class T, class = void>
constexpr static auto has_static_member_max_size_v = false;

template<class T>
constexpr static auto has_static_member_max_size_v<
        T, std::void_t<decltype(T::max_size())>
> = true;

template<class T, class = void>
constexpr static auto has_static_member_capacity_v = false;

template<class T>
constexpr static auto has_static_member_capacity_v<
        T, std::void_t<decltype(T::capacity())>
> = true;

template<class T, class = void>
constexpr static auto has_op_less_v = false;

template<class T>
constexpr static auto has_op_less_v<
        T, std::void_t<decltype(std::declval<T>() < std::declval<T>())>
> = true;

template<class T, class = void>
constexpr static auto has_member_full_v = false;

template<class T>
constexpr static auto has_member_full_v<
        T, std::void_t<decltype(std::declval<T>().full())>
> = true;

template<class T, class = void>
constexpr static auto has_member_empty_v = false;

template<class T>
constexpr static auto has_member_empty_v<
        T, std::void_t<decltype(std::declval<T>().empty())>
> = true;

}       // namespace tti
}       // namespace xstd
