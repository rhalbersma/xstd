#pragma once
#include <xstd/int_set.hpp>             // int_set
#include <boost/test/unit_test.hpp>     // BOOST_CHECK, BOOST_CHECK_EQUAL

namespace xstd {
namespace prim {

// 20.9.1 bitset constructors [bitset.cons]

// [bitset.cons]/1
template<int N>
constexpr auto cons_default() noexcept
{
        BOOST_CHECK(int_set<N>{}.empty());
}

// 20.9.2 bitset members [bitset.members]

// [bitset.members]/1
struct bitand_assign 
{
	template<int N>
	constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		auto const prev = lhs;
		auto next = lhs;

		next &= rhs;

		for (auto i = 0; i < N; ++i) {
		        if (!rhs.test(i)) {
		                BOOST_CHECK(!next.test(i));
		        } else {
		                BOOST_CHECK_EQUAL(next.test(i), prev.test(i));
		        }
		}
	}
};

// [bitset.members]/3
struct bitor_assign
{
	template<int N>
	constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		auto const prev = lhs;
		auto next = lhs;

		next |= rhs;

		for (auto i = 0; i < N; ++i) {
		        if (rhs.test(i)) {
		                BOOST_CHECK(next.test(i));
		        } else {
		                BOOST_CHECK_EQUAL(next.test(i), prev.test(i));
		        }
		}
	}
};

// [bitset.members]/5
struct xor_assign
{
	template<int N>
	constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		auto const prev = lhs;
		auto next = lhs;

		next ^= rhs;

		for (auto i = 0; i < N; ++i) {
		        if (rhs.test(i)) {
		                BOOST_CHECK_EQUAL(next.test(i), !prev.test(i));
		        } else {
		                BOOST_CHECK_EQUAL(next.test(i),  prev.test(i));
		        }
		}
	}
};

struct minus_assign 
{
	template<int N>
	constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		auto const prev = lhs;
		auto next = lhs;

		next -= rhs;

		for (auto i = 0; i < N; ++i) {
		        if (rhs.test(i)) {
		                BOOST_CHECK(!next.test(i));
		        } else {
		                BOOST_CHECK_EQUAL(next.test(i), prev.test(i));
		        }
		}
	}
};

// [bitset.members]/7
template<int N>
auto shift_left_assign(int_set<N> const& is, int const pos)
{
        auto const prev = is;
        auto next = is;

        next <<= pos;

        for (auto I = 0; I < N; ++I) {
                if (I < pos) {
                        BOOST_CHECK(!next.test(I));
                } else {
                        BOOST_CHECK_EQUAL(next.test(I), prev.test(I - pos));
                }
        }
}

// [bitset.members]/9
template<int N>
auto shift_right_assign(int_set<N> const& is, int const pos)
{
        auto const prev = is;
        auto next = is;

        next >>= pos;

        for (auto I = 0; I < N; ++I) {
                if (pos >= N - I) {
                        BOOST_CHECK(!next.test(I));
                } else {
                        BOOST_CHECK_EQUAL(next.test(I), prev.test(I + pos));
                }
        }
}

// [bitset.members]/11
template<int N>
auto fill(int_set<N> const& is) noexcept
{
        auto value = is;

        value.fill();

        BOOST_CHECK(value.full());
}

// [bitset.members]/14
template<int N>
constexpr auto insert(int_set<N> const& is, int const pos)
{
        auto const prev = is;
        auto next = prev;

        next.insert(pos);

        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        BOOST_CHECK(next.test(i));
                } else {
                        BOOST_CHECK_EQUAL(next.test(i), prev.test(i));
                }
        }
}

// [bitset.members]/16
template<int N>
auto clear(int_set<N> const& is) noexcept
{
        auto value = is;

        value.clear();

        BOOST_CHECK(value.empty());
}

// [bitset.members]/19
template<int N>
constexpr auto erase(int_set<N> const& is, int const pos)
{
        auto const prev = is;
        auto next = prev;

        next.erase(pos);

        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        BOOST_CHECK(!next.test(i));
                } else {
                        BOOST_CHECK_EQUAL(next.test(i), prev.test(i));
                }
        }
}

// [bitset.members]/21
template<int N>
constexpr auto compl_(int_set<N> const& is) noexcept
{
        auto expected = is;

        expected.flip();

        BOOST_CHECK(~is == expected);
}

// [bitset.members]/23
template<int N>
constexpr auto flip_all(int_set<N> const& is) noexcept
{
        auto const prev = is;
        auto next = is;

        next.flip();

        for (auto i = 0; i < N; ++i) {
                BOOST_CHECK_NE(next.test(i), prev.test(i));
        }
}

// [bitset.members]/26
template<int N>
constexpr auto flip_one(int_set<N> const& is, int const pos)
{
        auto const prev = is;
        auto next = is;

        next.flip(pos);

        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        BOOST_CHECK_NE   (next.test(i), prev.test(i));
                } else {
                        BOOST_CHECK_EQUAL(next.test(i), prev.test(i));
                }
        }
}

// [bitset.members]/34
template<int N>
auto size_(int_set<N> const& is) noexcept
{
        auto expected = 0;

        for (auto i = 0; i < N; ++i) {
                expected += is.test(i);
        }

        BOOST_CHECK_EQUAL(is.size(), expected);
}

template<int N>
auto non_member_size(int_set<N> const& is) noexcept
{
        BOOST_CHECK_EQUAL(size(is), is.size());
}

// [bitset.members]/35
template<int N>
auto max_size(int_set<N> const& is) noexcept
{
        BOOST_CHECK_EQUAL(is.max_size(), N);
}

// [bitset.members]/36
struct equal_to
{
	template<int N>
	auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		auto expected = true;

		for (auto i = 0; i < N; ++i) {
		        expected &= lhs.test(i) == rhs.test(i);
		}

		BOOST_CHECK_EQUAL(lhs == rhs, expected);
	}
};

// [bitset.members]/37
struct not_equal_to
{
	template<int N>
	auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		BOOST_CHECK_EQUAL(lhs != rhs, !(lhs == rhs));
	}
};

struct less
{
	template<int N>
	auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		auto expected = false;

		for (auto i = N - 1; i >= 0; --i) {
		        if (!lhs.test(i) && rhs.test(i)) { expected = true; break; }
		        if (!rhs.test(i) && lhs.test(i)) {                  break; }
		}

		BOOST_CHECK_EQUAL(lhs < rhs, expected);
	}
};

struct greater
{
	template<int N>
	auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		BOOST_CHECK_EQUAL(lhs > rhs, rhs < lhs);
	}
};

struct greater_equal
{
	template<int N>
	auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		BOOST_CHECK_EQUAL(lhs >= rhs, !(lhs < rhs));
	}
};

struct less_equal
{
	template<int N>
	auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		BOOST_CHECK_EQUAL(lhs <= rhs, !(rhs < lhs));
	}
};

struct is_subset_of_
{
	template<int N>
	auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		auto expected = true;

		for (auto i = 0; i < N; ++i) {
		        expected &= !lhs.test(i) || rhs.test(i);
		}

		BOOST_CHECK_EQUAL(is_subset_of(lhs, rhs), expected);
	}
};

struct is_superset_of_
{
	template<int N>
	auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		BOOST_CHECK_EQUAL(is_superset_of(lhs, rhs), is_subset_of(rhs, lhs));
	}
};

struct is_proper_subset_of_
{
	template<int N>
	auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		BOOST_CHECK_EQUAL(is_proper_subset_of(lhs, rhs), is_subset_of(lhs, rhs) && !is_subset_of(rhs, lhs));
	}
};

struct is_proper_superset_of_
{
	template<int N>
	auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		BOOST_CHECK_EQUAL(is_proper_superset_of(lhs, rhs), is_proper_subset_of(rhs, lhs));
	}
};

// [bitset.members]/39
template<int N>
auto test(int const pos) noexcept
{
        auto is = int_set<N>{};

        is.insert(pos);

        for (auto i = 0; i < N; ++i) {
                if (i == pos) {
                        BOOST_CHECK( is.test(i));
                } else {
                        BOOST_CHECK(!is.test(i));
                }
        }
}

// [bitset.members]/40
template<int N>
auto full(int_set<N> const& is) noexcept
{
        BOOST_CHECK_EQUAL(is.full(), is.size() == is.max_size());
}

// [bitset.members]/41
template<int N>
auto not_empty(int_set<N> const& is) noexcept
{
        BOOST_CHECK_EQUAL(!is.empty(), is.size() != 0);
}

// [bitset.members]/42
template<int N>
auto empty_(int_set<N> const& is) noexcept
{
        BOOST_CHECK_EQUAL(is.empty(), is.size() == 0);
}

template<int N>
auto non_member_empty(int_set<N> const& is) noexcept
{
        BOOST_CHECK_EQUAL(empty(is), is.empty());
}

// [bitset.members]/43
template<int N>
auto shift_left(int_set<N> const& is, int n)
{
        auto expected = is;

        expected <<= n;

        BOOST_CHECK(is << n == expected);
}

// [bitset.members]/44
template<int N>
auto shift_right(int_set<N> const& is, int n)
{
        auto expected = is;

        expected >>= n;

        BOOST_CHECK(is >> n == expected);
}

// 20.9.4 bitset operators [bitset.operators]

// [bitset.operators]/1
struct bitand_
{
	template<int N>
	constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		auto expected = lhs;

		expected &= rhs;

		BOOST_CHECK((lhs & rhs) == expected);
	}
};

// [bitset.operators]/2
struct bitor_
{
	template<int N>
	constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		auto expected = lhs;

		expected |= rhs;

		BOOST_CHECK((lhs | rhs) == expected);
	}
};

// [bitset.operators]/3
struct xor_
{
	template<int N>
	constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		auto expected = lhs;

		expected ^= rhs;

		BOOST_CHECK((lhs ^ rhs) == expected);
	}
};

struct minus
{
	template<int N>
	constexpr auto operator()(int_set<N> const& lhs, int_set<N> const& rhs) const noexcept
	{
		auto expected = lhs;

		expected -= rhs;

		BOOST_CHECK(lhs - rhs == expected);
	}
};

}       // namespace prim
}       // namespace xstd
