//          Copyright Rein Halbersma 2014-2026.
// Distributed under the Boost Software License, Version 1.0.

#include <xstd/type_traits/is_character.hpp>
#include <boost/test/unit_test.hpp>

namespace {
enum class enumeration {};
struct class_type {};
}

BOOST_AUTO_TEST_CASE(IsCharacter)
{
static_assert(xstd::is_character_v<char>);
static_assert(xstd::is_character_v<wchar_t>);
static_assert(xstd::is_character_v<char8_t>);
static_assert(xstd::is_character_v<char16_t>);
static_assert(xstd::is_character_v<char32_t>);
static_assert(not xstd::is_character_v<bool>);
static_assert(not xstd::is_character_v<signed char>);
static_assert(not xstd::is_character_v<unsigned char>);
static_assert(not xstd::is_character_v<short>);
static_assert(not xstd::is_character_v<int>);
static_assert(not xstd::is_character_v<long>);
static_assert(not xstd::is_character_v<float>);
static_assert(not xstd::is_character_v<double>);
static_assert(not xstd::is_character_v<int*>);
static_assert(not xstd::is_character_v<enumeration>);
static_assert(not xstd::is_character_v<class_type>);
static_assert(not xstd::is_character_v<char const>);
static_assert(not xstd::is_character_v<char volatile>);
BOOST_CHECK(true);
}
