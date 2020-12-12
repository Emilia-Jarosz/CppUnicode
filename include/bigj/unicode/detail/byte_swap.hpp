#pragma once

#include <bit>

#include <cstdint>

namespace bigj {
namespace unicode {
namespace detail {

constexpr auto byte_swap(uint16_t x) noexcept -> uint16_t {
    return std::rotl(x, 8);
}

constexpr auto byte_swap(uint32_t x) noexcept -> uint32_t {
    return std::rotl((x & 0xFF00FF00) >> 8 | (x & 0x00FF00FF) << 8, 16);
}

} // namespace detail
} // namespace unicode
} // namespace bigj
