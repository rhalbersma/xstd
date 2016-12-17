#pragma once

namespace xstd {
namespace bit {
namespace mask {

template<class T> constexpr auto none =  static_cast<T>(0);
template<class T> constexpr auto one  =  static_cast<T>(1);
template<class T> constexpr auto all  = ~static_cast<T>(0);

}       // namespace mask
}       // namespace bit
}       // namespace xstd
