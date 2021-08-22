#pragma once

#include <cstdint>

namespace bigj {
namespace unicode {
namespace detail {

constexpr auto byte_swap(uint16_t x) noexcept -> uint16_t {
    return (x << 8) | (x >> 8);
}

constexpr auto byte_swap(uint32_t x) noexcept -> uint32_t {
    x = ((x & 0x00FF00FF) <<  8) | ((x & 0xFF00FF00) >>  8);
    return (x << 16) | (x >> 16);
}

constexpr auto byte_swap(uint64_t x) noexcept -> uint64_t {
    x = ((x & 0x00FF00FF00FF00FF) <<  8) | ((x & 0xFF00FF00FF00FF00) >>  8);
    x = ((x & 0x0000FFFF0000FFFF) << 16) | ((x & 0xFFFF0000FFFF0000) >> 16);
    return (x << 32) | (x >> 32);
}

} // namespace detail
} // namespace unicode
} // namespace bigj
