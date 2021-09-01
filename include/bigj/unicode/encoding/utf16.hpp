#pragma once

#include "../detail/byte_swap.hpp"
#include "../encoding.hpp"

namespace bigj {
namespace unicode {

template<std::endian E>
    requires (E == std::endian::native)
    || (detail::big_or_little<E> && detail::big_or_little<std::endian::native>)
struct utf16 {

    using code_unit = uint16_t;
    using pointer = code_unit*;
    using const_pointer = const code_unit*;

    static constexpr auto encoded_size(code_point p) noexcept -> size_t {
        return 1 + (p > 0xFFFF);
    }

    static constexpr auto encode(code_point p, pointer it) noexcept -> pointer {
        if (p <= 0xFFFF) {
            *it++ = swap_endian(p);
        } else [[unlikely]] {
            p = p - 0x010000;
            *it++ = swap_endian(0xD800 | ((p & 0x0FFC00) >> 10));
            *it++ = swap_endian(0xDC00 | (p & 0x0003FF));
        }

        return it;
    }

    static constexpr auto decode(const_pointer it) noexcept -> code_point {
        if (auto w1 = swap_endian(*it++); w1 < 0xD800 || w1 > 0xDFFF) {
            return w1;
        } else [[unlikely]] {
            auto w2 = swap_endian(*it);

            return (static_cast<code_point>(w1 & 0x03FF) << 10
                | static_cast<code_point>(w2 & 0x03FF))
                + 0x010000;
        }
    }

    static constexpr auto validate(
        const_pointer it,
        const_pointer end
    ) noexcept -> error_code {
        if (auto w1 = swap_endian(*it++); w1 < 0xD800 || w1 > 0xDFFF) {
            return error_code::ok;
        } else [[unlikely]] {
            if (w1 <= 0xDBFF) {
                if (it != end) {
                    if (auto w2 = swap_endian(*it); w2 >= 0xDC00 && w2 <= 0xDFFF) {
                        return error_code::ok;
                    } else [[unlikely]] {
                        return error_code::unexpected_code_unit;
                    }
                } else [[unlikely]] {
                    return error_code::incomplete_sequence;
                }
            } else [[unlikely]] {
                return error_code::unexpected_code_unit;
            }
        }
    }

    static constexpr auto next_code_point(const_pointer it) noexcept -> const_pointer {
        bool is_surrogate = ((swap_endian(*(it++)) & 0xFC00) == 0xD800);
        return it + is_surrogate;
    }

    static constexpr auto prev_code_point(const_pointer it) noexcept -> const_pointer {
        bool is_surrogate = ((swap_endian(*(--it)) & 0xFC00) == 0xDC00);
        return it - is_surrogate;
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

using utf16be = utf16<std::endian::big>;
using utf16le = utf16<std::endian::little>;

static_assert(encoding<utf16be>);
static_assert(encoding<utf16le>);

} // namespace unicode
} // namespace bigj
