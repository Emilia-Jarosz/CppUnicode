#pragma once

#include <concepts>

#include <cstdint>

namespace bigj {
namespace unicode {

struct code_point {

    template<std::integral T>
        requires (sizeof(T) <= sizeof(uint32_t))
    constexpr code_point(T value) : m_value {static_cast<uint32_t>(value)} {}

    constexpr code_point() noexcept = default;
    
    constexpr operator uint32_t() const noexcept { return m_value; }
    constexpr auto value() const noexcept -> uint32_t { return m_value; }

    constexpr auto plane() const noexcept -> uint32_t { return m_value >> 16; }

  private:
    uint32_t m_value = 0;
};

} // namespace unicode
} // namespace bigj
