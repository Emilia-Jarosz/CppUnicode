#pragma once

#include "../detail/byte_swap.hpp"
#include "../encoding.hpp"

namespace bigj {
namespace unicode {

template<std::endian E>
struct utf32 {

    using code_unit = uint32_t;
    using pointer = code_unit*;
    using const_pointer = const code_unit*;

    static constexpr auto encoded_size(code_point) noexcept -> size_t {
        return 1;
    }

    static constexpr auto encode(code_point p, pointer it) noexcept -> pointer {
        *it++ = swap_endian(p);
        return it;
    }

    static constexpr auto decode(const_pointer it) noexcept -> code_point {
        return swap_endian(*it);
    }

    static constexpr auto validate(
        const_pointer it,
        const_pointer
    ) noexcept -> error_code {
        auto u = swap_endian(*it);

        if (u <= 0x10FFFF && (u < 0x00D800 || u > 0x00DFFF)) {
            return error_code::ok;
        } else [[unlikely]] {
            return error_code::invalid_code_point;
        }
    }

    static constexpr auto next_code_point(const_pointer it) noexcept -> const_pointer {
        return ++it;
    }

    static constexpr auto prev_code_point(const_pointer it) noexcept -> const_pointer {
        return --it;
    }

  private:

    static constexpr auto swap_endian(code_unit u) noexcept -> code_unit {
        if constexpr (E == std::endian::native) {
            return u;
        } else {
            return detail::byte_swap(u);
        }
    }
};

using utf32be = utf32<std::endian::big>;
using utf32le = utf32<std::endian::little>;

static_assert(encoding<utf32be>);
static_assert(encoding<utf32le>);

} // namespace unicode
} // namespace bigj
