#pragma once
#include <xstd/bit/primitive/builtin.hpp>       // ctznz, clznz, ctz, clz, bsfnz, bsrnz, popcount using gcc built-in functions
#include <xstd/bit/primitive/lookup.hpp>        // ctznz, clznz, ctz, clz, bsfnz, bsrnz, popcount using table lookup
#include <xstd/bit/primitive/loop.hpp>          // ctznz, clznz, ctz, clz, bsfnz, bsrnz, popcount using brute-force looping

namespace xstd {
namespace bit {

namespace lib = builtin;
using namespace lib;

}       // namespace bit
}       // namespace xstd
