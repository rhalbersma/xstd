#include <xstd/cstddef.hpp>     // _z, size_t
#include <type_traits>          // is_same

int main()
{
        using namespace xstd::support_literals;

        {
                static_assert(std::is_same<decltype(0_zu), std::size_t>::value, "");
                constexpr std::size_t a = 0_zu;
                constexpr auto b = 0_zu;
                static_assert(a == static_cast<std::size_t>(0), "");
                static_assert(a == b, "");
        }
}
