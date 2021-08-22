#pragma once

#include "basic_string_view.hpp"

#include <utility>

#include <cstring>

namespace bigj {

template<unicode::encoding E>
struct basic_string {

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

    basic_string() noexcept {}

    basic_string(const_pointer ptr, const_pointer end) {
        if (ptr >= end) return;
        
        auto length = unicode::detail::validate_string<E>(ptr, end);

        auto new_ptr = new code_unit[end - ptr];
        std::memcpy(new_ptr, ptr, (end - ptr) * sizeof(code_unit));

        m_ptr = new_ptr;
        m_size = end - ptr;
        m_length = length;
    }

    template<std::contiguous_iterator It, std::sized_sentinel_for<It> End>
        requires std::same_as<typename std::iterator_traits<It>::value_type, code_unit>
            && (not std::convertible_to<End, size_type>)
            && (not std::same_as<It, pointer>)
            && (not std::same_as<It, const_pointer>)
    basic_string(It begin, End end)
        : basic_string{std::to_address(begin), std::to_address(end)} {}

    basic_string(const_pointer ptr, size_type size)
        : basic_string{ptr, ptr + size} {}

    template<unicode::encoding F>
    basic_string(basic_string_view<F> other) {
        if (other.empty()) return;

        if constexpr (std::same_as<E, F>) {
            auto new_ptr = new code_unit[other.m_size];
            std::memcpy(new_ptr, other.m_ptr, other.m_size * sizeof(code_unit));

            m_ptr = new_ptr;
            m_size = other.m_size;
            m_length = other.m_length;
        } else {
            auto size = 0;

            for (auto cp : other) {
                size += E::encoded_size(cp);
            }

            auto ptr = new code_unit[size];

            m_ptr = ptr;
            m_size = size;
            m_length = other.m_length;

            for (auto cp : other) {
                ptr = E::encode(cp, ptr);
            }
        }
    }

    template<unicode::encoding F>
    basic_string(const basic_string<F>& other)
        : basic_string{(basic_string_view<F>) other} {}

    basic_string(const basic_string& other) noexcept {
        auto new_ptr = new code_unit[other.m_size];
        std::memcpy(new_ptr, other.m_ptr, other.m_size * sizeof(code_unit));

        m_ptr = new_ptr;
        m_size = other.m_size;
        m_length = other.m_length;
    }

    basic_string(basic_string&& other) noexcept
        : m_ptr {std::exchange(other.m_ptr, nullptr)}
        , m_size {std::exchange(other.m_size, 0)}
        , m_length {std::exchange(other.m_length, 0)} {}

    basic_string& operator=(basic_string other) {
        swap(other);
        return *this;
    }

    ~basic_string() noexcept {
        if (not empty()) {
            delete[] m_ptr;
        }
    }

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

    constexpr operator basic_string_view<E>() const noexcept {
        auto view = basic_string_view<E> {};

        view.m_ptr = m_ptr;
        view.m_size = m_size;
        view.m_length = m_length;

        return view;
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

    // Modifiers

    constexpr auto swap(basic_string& other) noexcept {
        std::swap(m_ptr, other.m_ptr);
        std::swap(m_size, other.m_size);
        std::swap(m_length, other.m_length);
    }

  private:
    const_pointer m_ptr = nullptr;
    size_type m_size = 0;
    size_type m_length = 0;
};

} // namespace bigj
