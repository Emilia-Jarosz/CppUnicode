#pragma once

#include "iterator.hpp"

namespace bigj {
namespace unicode {

template<encoding E>
struct reverse_iterator {
    
    using iterator_type = iterator<E>;

    using code_unit = typename E::code_unit;

    using value_type = code_point;
    using pointer = const code_unit*;
    using iterator_category = std::bidirectional_iterator_tag;

    constexpr reverse_iterator() noexcept {}

    explicit constexpr reverse_iterator(pointer ptr, pointer last) noexcept
        : m_ptr{ptr}, m_last{last}
    {
        ++(*this);
    }

    explicit constexpr reverse_iterator(iterator_type it, iterator_type last) noexcept
        : reverse_iterator{it.base(), last.base()} {}

    constexpr auto operator*() const noexcept -> value_type {
        return E::decode(m_ptr);
    }

    constexpr auto operator++() noexcept -> reverse_iterator& {
        if (m_ptr != m_last) {
            m_ptr = E::prev_code_point(m_ptr);
        } else [[unlikely]] {
            m_ptr = nullptr;
        }

        return *this;
    }

    constexpr auto operator++(int) noexcept -> reverse_iterator {
        auto tmp = *this;
        ++(*this);
        return tmp;
    }

    constexpr auto operator--() noexcept -> reverse_iterator& {
        if (m_ptr != nullptr) {
            m_ptr = E::next_code_point(m_ptr);
        } else [[unlikely]] {
            m_ptr = m_last;
        }

        return *this;
    }

    constexpr auto operator--(int) noexcept -> reverse_iterator {
        auto tmp = *this;
        --(*this);
        return tmp;
    }

    constexpr auto operator<=>(const reverse_iterator& other) const noexcept {
        return other.m_ptr <=> m_ptr;
    }

    constexpr auto operator==(const reverse_iterator& other) const noexcept {
        return other.m_ptr == m_ptr;
    }

    constexpr auto base() const noexcept -> iterator_type {
        auto tmp = *this;
        --tmp;
        return iterator_type {tmp.m_ptr};
    }

  private:
    pointer m_ptr = nullptr;
    pointer m_last = nullptr;
};

} // namespace unicode
} // namespace bigj

namespace std {

template<bigj::unicode::encoding E>
struct iterator_traits<bigj::unicode::reverse_iterator<E>> {

    using difference_type = ptrdiff_t;
    using value_type = bigj::unicode::code_point;
    using pointer = const typename E::code_unit*;
    using reference = const bigj::unicode::code_point&;
    using iterator_category = std::bidirectional_iterator_tag;
};

} // namespace std
