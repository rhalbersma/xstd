#pragma once
#include <xstd/int_set/bit/builtin.hpp> // ctznz, clznz, popcount using GCC / Clang built-in functions
#include <xstd/int_set/bit/lookup.hpp>  // ctznz, clznz, popcount using table lookup
#include <xstd/int_set/bit/loop.hpp>    // ctznz, clznz, popcount using brute-force looping

namespace xstd {
namespace bit {

namespace lib = builtin;
using namespace lib;

}       // namespace bit
}       // namespace xstd
