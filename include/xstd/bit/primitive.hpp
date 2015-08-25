#pragma once
#include <xstd/bit/primitive/builtin.hpp>       // ctznz, clznz, popcount using GCC / Clang built-in functions
#include <xstd/bit/primitive/lookup.hpp>        // ctznz, clznz, popcount using table lookup
#include <xstd/bit/primitive/loop.hpp>          // ctznz, clznz, popcount using brute-force looping

namespace xstd {
namespace bit {

namespace lib = builtin;
using namespace lib;

}       // namespace bit
}       // namespace xstd
