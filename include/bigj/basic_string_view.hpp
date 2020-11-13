#pragma once

#include "unicode/detail/exceptions.hpp"
#include "unicode/iterator.hpp"

#include <iterator>

namespace bigj {

template<unicode::encoding E>
struct basic_string_view {

    using code_unit = E::code_unit;
    using value_type = unicode::code_point;
    using const_pointer = const code_unit*;
    using pointer = const_pointer;
    using const_iterator = unicode::iterator<E>;
    using iterator = const_iterator;
    using const_reverse_iterator = std::reverse_iterator<iterator>;
    using reverse_iterator = const_reverse_iterator;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    constexpr basic_string_view() noexcept {}

    constexpr basic_string_view(const_pointer ptr, const_pointer end) {
        if (ptr >= end) return;

        size_type length = 0;

        for (auto it = ptr; it != end; it = E::next_code_point(it), ++length) {
            if (E::validate(it, end) != unicode::error_code::ok) [[unlikely]] {
                throw unicode::parse_error{ec};
            }
        }

        m_ptr = ptr;
        m_size = end - ptr;
        m_length = length;
    }

    constexpr basic_string_view(const_pointer ptr, size_type size)
        : basic_string_view{ptr, ptr + size} {}

    constexpr basic_string_view(const basic_string_view&) noexcept = default;
    constexpr basic_string_view(basic_string_view&&) noexcept = default;

    constexpr basic_string_view& operator=(const basic_string_view&) noexcept = default;
    constexpr basic_string_view& operator=(basic_string_view&&) noexcept = default;

    // Iterators

    constexpr auto begin() const noexcept {
        return const_iterator{m_ptr};
    }

    constexpr auto cbegin() const noexcept {
        return const_iterator{m_ptr};
    }

    constexpr auto end() const noexcept {
        return const_iterator{m_ptr + m_size};
    }

    constexpr auto cend() const noexcept {
        return const_iterator{m_ptr + m_size};
    }

    constexpr auto rbegin() const noexcept {
        return const_reverse_iterator{end()};
    }

    constexpr auto crbegin() const noexcept {
        return const_reverse_iterator{cend()};
    }

    constexpr auto rend() const noexcept {
        return const_reverse_iterator{begin()};
    }

    constexpr auto crend() const noexcept {
        return const_reverse_iterator{cbegin()};
    }

    // Element access

    constexpr auto front() const noexcept -> value_type {
        return *begin();
    }

    constexpr auto back() const noexcept -> value_type {
        return *rbegin();
    }

    constexpr auto data() const noexcept -> const_pointer {
        return m_ptr;
    }

    // Capacity

    constexpr auto size() const noexcept -> size_type {
        return m_size;
    }

    constexpr auto length() const noexcept -> size_type {
        return m_length;
    }

    constexpr auto max_size() const noexcept -> size_type {
        return std::numeric_limits<size_type>::max() / sizeof(code_unit);
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return m_size == 0;
    }

  private:
    const code_unit* m_ptr = nullptr;
    size_type m_size = 0;
    size_type m_length = 0;
};

} // namespace bigj