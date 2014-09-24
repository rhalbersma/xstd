#pragma once
#include <xstd/bitset/intrinsic/builtin.hpp>    // ctznz, clznz, ctz, clz, bsfnz, bsrnz, popcount using gcc built-in functions
#include <xstd/bitset/intrinsic/lookup.hpp>     // ctznz, clznz, ctz, clz, bsfnz, bsrnz, popcount using table lookup
#include <xstd/bitset/intrinsic/loop.hpp>       // ctznz, clznz, ctz, clz, bsfnz, bsrnz, popcount using brute-force looping

namespace xstd {
namespace intrinsic {

namespace lib = builtin;
using namespace lib;

}       // namespace intrinsic
}       // namespace xstd
