#pragma once
#include <int_set/primitive.hpp>        // primitive tests
#include <xstd/int_set.hpp>             // int_set

namespace xstd {

template<int N>
constexpr auto bitand_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= bitand_assign(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
constexpr auto bitor_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= bitor_assign(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
constexpr auto xor_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= xor_assign(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
constexpr auto minus_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= minus_assign(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
auto shift_left_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= shift_left_assign(int_set<N>{i}, j);
                }
        }
        return check;
}

template<int N>
auto shift_right_assign() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= shift_right_assign(int_set<N>{i}, j);
                }
        }
        return check;
}

template<int N>
auto fill() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= fill(~(~int_set<N>{} << i));
        }
        check &= fill(~int_set<N>{});
        return check;
}

template<int N>
constexpr auto insert() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= insert(int_set<N>{i}, j);
                }
        }
        return check;
}

template<int N>
auto clear() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= clear(~(~int_set<N>{} << i));
        }
        check &= clear(~int_set<N>{});
        return check;
}

template<int N>
constexpr auto erase() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= erase(int_set<N>{i}, j);
                }
        }
        return check;
}

template<int N>
constexpr auto compl_() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= compl_(~(~int_set<N>{} << i));
        }
        check &= compl_(~int_set<N>{});
        return check;
}

template<int N>
constexpr auto flip_all() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= flip_all(~(~int_set<N>{} << i));
        }
        check &= flip_all(~int_set<N>{});
        return check;
}

template<int N>
constexpr auto flip_one() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= flip_one(int_set<N>{i}, j);
                }
        }
        return check;
}

template<int N>
auto size_() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= size_(~(~int_set<N>{} << i));
        }
        check &= size_(~int_set<N>{});
        return check;
}

template<int N>
auto non_member_size() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= non_member_size(~(~int_set<N>{} << i));
        }
        check &= non_member_size(~int_set<N>{});
        return check;
}

template<int N>
auto max_size() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= max_size(~(~int_set<N>{} << i));
        }
        check &= max_size(~int_set<N>{});
        return check;
}

template<int N>
auto equal_to() noexcept
{
        auto check = equal_to(int_set<N>{}, int_set<N>{});
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= equal_to(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
auto not_equal_to() noexcept
{
        auto check = not_equal_to(int_set<N>{}, int_set<N>{});
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= not_equal_to(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
auto less() noexcept
{
        auto check = less(int_set<N>{}, int_set<N>{});
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= less(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
auto greater() noexcept
{
        auto check = greater(int_set<N>{}, int_set<N>{});
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= greater(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
auto greater_equal() noexcept
{
        auto check = greater_equal(int_set<N>{}, int_set<N>{});
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= greater_equal(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
auto less_equal() noexcept
{
        auto check = less_equal(int_set<N>{}, int_set<N>{});
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= less_equal(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
auto is_subset_of_() noexcept
{
        auto check = is_subset_of_(int_set<N>{}, int_set<N>{});
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= is_subset_of_(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
auto is_superset_of_() noexcept
{
        auto check = is_superset_of_(int_set<N>{}, int_set<N>{});
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= is_superset_of_(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
auto is_proper_subset_of_() noexcept
{
        auto check = is_proper_subset_of_(int_set<N>{}, int_set<N>{});
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= is_proper_subset_of_(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
auto is_proper_superset_of_() noexcept
{
        auto check = is_proper_superset_of_(int_set<N>{}, int_set<N>{});
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= is_proper_superset_of_(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
constexpr auto test() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= test<N>(i);
        }
        return check;
}

template<int N>
auto full() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= full(~(~int_set<N>{} << i));
        }
        check &= full(~int_set<N>{});
        return check;
}

template<int N>
auto not_empty() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= not_empty(~(~int_set<N>{} << i));
        }
        check &= not_empty(~int_set<N>{});
        return check;
}

template<int N>
auto empty_() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= empty_(~(~int_set<N>{} << i));
        }
        check &= empty_(~int_set<N>{});
        return check;
}

template<int N>
auto non_member_empty() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                check &= non_member_empty(~(~int_set<N>{} << i));
        }
        check &= non_member_empty(~int_set<N>{});
        return check;
}

// operators

template<int N>
auto shift_left() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= shift_left(int_set<N>{i}, j);
                }
        }
        return check;
}

template<int N>
auto shift_right() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= shift_right(int_set<N>{i}, j);
                }
        }
        return check;
}

template<int N>
constexpr auto bitand_() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= bitand_(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
constexpr auto bitor_() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= bitor_(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
constexpr auto xor_() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= xor_(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

template<int N>
constexpr auto minus() noexcept
{
        auto check = true;
        for (auto i = 0; i < N; ++i) {
                for (auto j = 0; j < N; ++j) {
                        check &= minus(int_set<N>{i}, int_set<N>{j});
                }
        }
        return check;
}

}       // namespace xstd
