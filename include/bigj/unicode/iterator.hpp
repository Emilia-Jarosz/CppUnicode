#pragma once

#include "encoding.hpp"

namespace bigj {
namespace unicode {

template<encoding E>
struct iterator {

    using code_unit = typename E::code_unit;
    using value_type = code_point;

    constexpr iterator() noexcept : m_ptr{nullptr} {}
    explicit constexpr iterator(const code_unit* ptr) noexcept : m_ptr{ptr} {}

    constexpr auto operator*() const noexcept -> value_type {
        return E::decode(m_ptr);
    }

    constexpr auto operator++() noexcept -> iterator& {
        m_ptr = E::next_code_point(m_ptr);
        return *this;
    }

    constexpr auto operator++(int) noexcept -> iterator {
        auto tmp = *this;
        m_ptr = E::next_code_point(m_ptr);
        return tmp;
    }

    constexpr auto operator--() noexcept -> iterator& {
        m_ptr = E::prev_code_point(m_ptr);
        return *this;
    }

    constexpr auto operator--(int) noexcept -> iterator {
        auto tmp = *this;
        m_ptr = E::prev_code_point(m_ptr);
        return tmp;
    }

    constexpr auto operator<=>(iterator& other) const noexcept {
        return m_ptr <=> other.m_ptr;
    }

    constexpr auto data() const noexcept -> const code_unit* {
        return m_ptr;
    }

  private:
    const code_unit* m_ptr;
};

} // namespace unicode
} // namespace bigj
