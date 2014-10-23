#include <xstd/cstddef.hpp>     // _z, _t, size_t, ptrdiff_t
#include <type_traits>          // is_same

int main()
{
        using namespace xstd::support_literals;

        {
                static_assert(std::is_same<decltype(0_z), std::size_t>::value, "");
                constexpr std::size_t a = 0_z;
                constexpr auto b = 0_z;
                static_assert(a == static_cast<std::size_t>(0), "");
                static_assert(a == b, "");
        }

        {
                static_assert(std::is_same<decltype(0_t), std::ptrdiff_t>::value, "");
                constexpr std::size_t a = 0_t;
                constexpr auto b = 0_t;
                static_assert(a == static_cast<ptrdiff_t>(0), "");
                static_assert(a == b, "");
        }
}
