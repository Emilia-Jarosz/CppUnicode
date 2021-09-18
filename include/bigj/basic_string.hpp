#pragma once

#include "basic_string_view.hpp"
#include "unicode/config.hpp"
#include "unicode/detail/endian.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <iterator>
#include <limits>
#include <new>
#include <ranges>
#include <stdexcept>

#include <cassert>
#include <cstddef>

namespace bigj {

template<unicode::encoding E>
    requires unicode::detail::big_or_little<std::endian::native>
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

    constexpr basic_string() noexcept {
        small_size(0);
    }

    basic_string(const_pointer begin, const_pointer end) : basic_string {} {
        assert(begin <= end);
        
        unicode::detail::validate_string<E>(begin, end);

        auto size = end - begin;
        auto data = init(size);

        std::copy(begin, end, data);
    }

    basic_string(const_iterator begin, const_iterator end)
        : basic_string {begin.address(), end.address()} {}

    basic_string(const_pointer ptr, size_type size)
        : basic_string {ptr, ptr + size} {}

    template<unicode::encoding F>
    basic_string(basic_string_view<F> sv) {
        auto size = size_type {0};

        for (auto cp : sv.code_points()) {
            size += E::encoded_size(cp);
        }

        auto data = init(size);
        auto end = data + size;

        for (auto cp : sv.code_points()) {
            data = E::encode(cp, data);
        }

        assert(data == end);
    }

    template<unicode::encoding F>
        requires std::same_as<E, F>
    basic_string(basic_string_view<F> sv)
        : basic_string {sv.begin(), sv.end()} {}

    template<unicode::encoding F>
    basic_string(const basic_string<F>& other)
        : basic_string {static_cast<basic_string_view<F>>(other)} {}

    basic_string(const basic_string& other) noexcept {
        m_bytes = other.m_bytes;
        if (is_large()) (*m_large.refs)++;
    }

    constexpr basic_string(basic_string&& other) noexcept {
        m_bytes = other.m_bytes;
        other.abandon();
    }

    basic_string& operator=(basic_string other) noexcept {
        swap(other);
        return *this;
    }

    ~basic_string() noexcept {
        destroy();
    }

    // Iterators

    constexpr auto begin() const noexcept -> const_iterator {
        if (is_large()) {
            return const_iterator {m_large.begin};
        } else {
            return const_iterator {m_small.data()};
        }
    }

    constexpr auto end() const noexcept -> const_iterator {
        if (is_large()) {
            return const_iterator {m_large.end};
        } else {
            return const_iterator {m_small.data() + small_size()};
        }
    }

    constexpr auto rbegin() const noexcept -> const_reverse_iterator {
        if (is_large()) {
            return const_reverse_iterator {m_large.end, m_large.begin};
        } else {
            return const_reverse_iterator {m_small.data() + small_size(), m_small.data()};
        }
    }

    constexpr auto rend() const noexcept -> const_reverse_iterator {
        if (is_large()) {
            return const_reverse_iterator {m_large.begin, m_large.begin};
        } else {
            return const_reverse_iterator {m_small.data(), m_small.data()};
        }
    }

    constexpr auto cbegin() const noexcept -> const_iterator {
        return begin();
    }

    constexpr auto cend() const noexcept -> const_iterator {
        return end();
    }

    constexpr auto crbegin() const noexcept -> const_reverse_iterator {
        return rbegin();
    }

    constexpr auto crend() const noexcept -> const_reverse_iterator {
        return rend();
    }

    // Element access

    constexpr auto front() const noexcept -> value_type {
        return *begin();
    }

    constexpr auto back() const noexcept -> value_type {
        return *rbegin();
    }

    constexpr auto code_points() const noexcept -> std::ranges::subrange<const_iterator> {
        return {begin(), end()};
    }

    constexpr auto code_units() const noexcept -> std::ranges::subrange<const_pointer> {
        if (is_large()) {
            return {m_large.begin, m_large.end};
        } else {
            return {m_small.data(), m_small.data() + small_size()};
        }
    }

    constexpr auto c_str() const noexcept -> const_pointer
        requires unicode::config::null_terminators
    {
        if (is_large()) {
            return m_large.begin;
        } else {
            return m_small.data();
        }
    }

    constexpr operator basic_string_view<E>() const noexcept {
        return basic_string_view<E> {begin(), end()};
    }

    // Capacity

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return is_empty();
    }

    auto length() const noexcept -> size_type {
        if (is_large()) {
            if (m_large.length()) {
                return m_large.length();
            } else {
                auto new_length = std::distance(begin(), end());
                m_large.length(new_length);
                return new_length;
            }
        } else {
            return std::distance(begin(), end());
        }
    }

    auto size() const noexcept -> size_type {
        return length();
    }

    constexpr auto max_size() const noexcept -> size_type {
        constexpr auto max_bytes = std::numeric_limits<size_type>::max();
        constexpr auto max_str_bytes = max_bytes - sizeof(ref_count);
        constexpr auto max_code_units = max_str_bytes / sizeof(code_unit);
        return std::min(max_code_units, max_length);
    }

    // Modifiers

    auto clear() noexcept -> void {
        destroy();
    }

    constexpr auto swap(basic_string& other) noexcept -> void {
        std::swap_ranges(m_bytes.begin(), m_bytes.end(), other.m_bytes.begin());
    }

  private:

    using ref_count = std::atomic_size_t;

    static constexpr auto null_terminator = (code_unit) 0;

    auto init(size_type size) -> pointer {
        if (size <= max_small_capacity) {
            return init_small(size);
        } else if (size <= max_size()) {
            return init_large(size);
        } else {
            throw std::length_error {"string is too long"};
        }
    }

    auto init_small(size_type size) noexcept -> pointer {
        assert(size <= max_small_capacity);
        
        small_size(size);

        if constexpr (unicode::config::null_terminators) {
            *(m_small.data() + size) = null_terminator;
        }

        return m_small.data();
    }

    auto init_large(size_type size) -> pointer {
        assert(size <= max_size());

        auto code_unit_count = size;

        if constexpr (unicode::config::null_terminators) {
            code_unit_count += 1;
        }

        auto allocation_size = sizeof(ref_count) + sizeof(code_unit) * code_unit_count;
        auto ptr = operator new(allocation_size);
        auto str_ptr = reinterpret_cast<pointer>(
            static_cast<std::byte*>(ptr) + sizeof(ref_count));

        m_large.refs = new(ptr) ref_count {1};
        m_large.begin = str_ptr;
        m_large.end = m_large.begin + size;
        m_large.length(0);

        if constexpr (unicode::config::null_terminators) {
            *m_large.end = null_terminator;
        }

        return m_large.begin;
    }

    auto destroy() noexcept -> void {
        if (is_large() && (*m_large.refs)-- == 1) {
            m_large.refs->~ref_count();
            operator delete(static_cast<void*>(m_large.refs));
        }
        
        small_size(0);
    }

    constexpr auto abandon() noexcept -> void {
        small_size(0);
    }

    static constexpr auto is_big_endian = std::endian::native == std::endian::big;
    static constexpr auto byte_bits = std::numeric_limits<unsigned char>::digits;
    static constexpr auto size_msb = (size_type) 1 << (byte_bits * sizeof(size_type) - 1);
    static constexpr auto byte_msb = (std::byte) 1 << (byte_bits - 1);

    static constexpr auto max_length = size_msb - 1;

    struct large_str {

        auto length() const noexcept -> size_type {
            auto tmp = is_big_endian ? std::rotr(not_length, byte_bits) : not_length;
            return tmp ^ size_msb;
        }

        auto length(size_type new_length) const noexcept -> void {
            assert(new_length <= max_length);
            auto tmp = new_length | size_msb;
            not_length = is_big_endian ? std::rotl(tmp, byte_bits) : tmp;
        }

        pointer begin;
        pointer end;
        ref_count* refs;
        mutable size_type not_length;
    };

    static constexpr auto byte_count = offsetof(large_str, not_length) + sizeof(size_type);
    static constexpr auto code_unit_capacity = byte_count / sizeof(code_unit);
    static constexpr auto max_small_capacity = static_cast<size_type>(code_unit_capacity - 1);

    constexpr auto is_empty() const noexcept -> bool {
        return static_cast<size_type>(m_bytes.back()) == max_small_capacity;
    }

    constexpr auto is_large() const noexcept -> bool {
        return static_cast<bool>(m_bytes.back() & byte_msb);
    }

    constexpr auto small_size() const noexcept -> size_type {
        return max_small_capacity - static_cast<size_type>(m_bytes.back());
    }

    constexpr auto small_size(size_type new_size) noexcept -> void {
        assert(new_size <= max_small_capacity);
        m_bytes.back() = static_cast<std::byte>(max_small_capacity - new_size);
    }

    union {
        std::array<std::byte, byte_count> m_bytes;
        std::array<code_unit, code_unit_capacity> m_small;
        large_str m_large;
    };
};

} // namespace bigj
