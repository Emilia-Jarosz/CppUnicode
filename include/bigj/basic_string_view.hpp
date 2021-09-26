#pragma once

#include "unicode/detail/validate_string.hpp"
#include "unicode/iterator.hpp"
#include "unicode/reverse_iterator.hpp"

#include <iterator>
#include <ranges>
#include <stdexcept>
#include <utility>

#include <cassert>

namespace bigj {

template<unicode::encoding E>
struct basic_string_view {

    using code_unit = typename E::code_unit;
    using value_type = unicode::code_point;
    using const_pointer = const code_unit*;
    using pointer = code_unit*;
    using const_iterator = unicode::iterator<E>;
    using iterator = const_iterator;
    using const_reverse_iterator = unicode::reverse_iterator<E>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    constexpr basic_string_view() noexcept = default;

    constexpr basic_string_view(const_pointer begin, const_pointer end) {
        assert(begin <= end);

        unicode::detail::validate_string<E>(begin, end);

        m_begin = begin;
        m_end = end;
    }

    constexpr basic_string_view(const_iterator begin, const_iterator end)
        : basic_string_view {begin.address(), end.address()} {}

    constexpr basic_string_view(const_pointer ptr, size_type size)
        : basic_string_view{ptr, ptr + size} {}

    constexpr basic_string_view(const basic_string_view&) noexcept = default;
    constexpr basic_string_view(basic_string_view&&) noexcept = default;

    constexpr basic_string_view& operator=(const basic_string_view&) noexcept = default;
    constexpr basic_string_view& operator=(basic_string_view&&) noexcept = default;

    // Iterators

    constexpr auto begin() const noexcept {
        return const_iterator{m_begin};
    }

    constexpr auto end() const noexcept {
        return const_iterator{m_end};
    }

    constexpr auto rbegin() const noexcept {
        return const_reverse_iterator{m_end, m_begin};
    }

    constexpr auto rend() const noexcept {
        return const_reverse_iterator{m_begin, m_begin};
    }

    constexpr auto cbegin() const noexcept {
        return begin();
    }

    constexpr auto cend() const noexcept {
        return end();
    }

    constexpr auto crbegin() const noexcept {
        return rbegin();
    }

    constexpr auto crend() const noexcept {
        return rend();
    }

    // Element access

    constexpr auto front() const noexcept -> value_type {
        return *begin();
    }

    constexpr auto back() const noexcept -> value_type {
        return *rbegin();
    }

    constexpr auto code_points() const noexcept {
        return std::ranges::subrange {begin(), end()};
    }

    constexpr auto code_units() const noexcept {
        return std::ranges::subrange {m_begin, m_end};
    }

    // Capacity

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return m_begin == m_end;
    }

    constexpr auto length() const noexcept -> size_type {
        return std::distance(begin(), end());
    }

    constexpr auto size() const noexcept -> size_type {
        return length();
    }

    constexpr auto max_size() const noexcept -> size_type {
        return std::numeric_limits<size_type>::max() / sizeof(code_unit);
    }

    // Modifiers

    constexpr auto swap(basic_string_view& sv) noexcept -> void {
        std::swap(m_begin, sv.m_begin);
        std::swap(m_end, sv.m_end);
    }

    constexpr auto remove_prefix(const_iterator new_begin) -> void {
        auto new_begin_ptr = new_begin.address();
        
        if (new_begin_ptr >= m_begin && new_begin_ptr <= m_end) {
            m_begin = new_begin_ptr;
        } else {
            throw std::out_of_range {"new begin iterator is out of range"};
        }
    }

    constexpr auto remove_suffix(const_iterator new_end) -> void {
        auto new_end_ptr = new_end.address();

        if (new_end_ptr >= m_begin && new_end_ptr <= m_end) {
            m_end = new_end_ptr;
        } else {
            throw std::out_of_range {"new end iterator is out of range"};
        }
    }

    // Operations

    constexpr auto substring(const_iterator begin, const_iterator end) const
        -> basic_string_view
    {
        auto begin_ptr = begin.address();
        auto end_ptr = end.address();

        if (
            begin_ptr >= m_begin && begin_ptr <= m_end
            && end_ptr >= m_begin && end_ptr <= m_end
            && end_ptr >= begin_ptr
        ) {
            auto substr = basic_string_view {};
            substr.m_begin = begin_ptr;
            substr.m_end = end_ptr;
            return substr;
        } else {
            throw std::out_of_range {"invalid substring"};
        }
    }

  private:
    template<unicode::encoding>
    friend class basic_string;

    const_pointer m_begin = nullptr;
    const_pointer m_end = nullptr;
};

} // namespace bigj
