#pragma once

#include "unicode/detail/validate_string.hpp"
#include "unicode/iterator.hpp"
#include "unicode/reverse_iterator.hpp"

#include <memory>

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

    constexpr basic_string_view() noexcept {}

    constexpr basic_string_view(const_pointer ptr, const_pointer end) {
        if (ptr >= end) return;

        auto length = unicode::detail::validate_string<E>(ptr, end);

        m_ptr = ptr;
        m_size = end - ptr;
        m_length = length;
    }

    template<std::contiguous_iterator It, std::sized_sentinel_for<It> End>
        requires std::same_as<typename std::iterator_traits<It>::value_type, code_unit>
            && (not std::convertible_to<End, size_type>)
            && (not std::same_as<It, pointer>)
            && (not std::same_as<It, const_pointer>)
    constexpr basic_string_view(It begin, End end)
        : basic_string_view{std::to_address(begin), std::to_address(end)} {}

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

    constexpr auto end() const noexcept {
        return const_iterator{m_ptr + m_size};
    }

    constexpr auto rbegin() const noexcept {
        return const_reverse_iterator{m_ptr + m_size, m_ptr};
    }

    constexpr auto rend() const noexcept {
        return const_reverse_iterator{m_ptr, m_ptr};
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
    const_pointer m_ptr = nullptr;
    size_type m_size = 0;
    size_type m_length = 0;
};

} // namespace bigj
