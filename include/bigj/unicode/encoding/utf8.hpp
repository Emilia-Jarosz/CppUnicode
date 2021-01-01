#pragma once

#include "encoding.hpp"

#include <bit>

namespace bigj {
namespace unicode {

struct utf8 {

    using code_unit = uint8_t;
    using pointer = code_unit*;
    using const_pointer = const code_unit*;

    static constexpr auto encoded_size(code_point p) noexcept -> size_t {
        if (p <= 0x00007F) {
            return 1;
        } else if (p <= 0x0007FF) {
            return 2;
        } else if (p <= 0x00FFFF) {
            return 3;
        } else {
            return 4;
        }
    }

    static constexpr auto encode(code_point p, pointer it) noexcept -> pointer {
        if (p <= 0x00007F) {
            *it++ = p; // 0xxxxxxx
            return it;
        } else if (p <= 0x0007FF) {
            *it++ = (p & 0x0007C0) >> 6 | 0xC0; // 110xxxxx
            *it++ = (p & 0x00003F) | 0x80; // 10xxxxxx
            return it;
        } else if (p <= 0x00FFFF) {
            *it++ = (p & 0x00F000) >> 12 | 0xE0; // 1110xxxx
            *it++ = (p & 0x000FC0) >> 6 | 0x80; // 10xxxxxx
            *it++ = (p & 0x00003F) | 0x80; // 10xxxxxx
            return it;
        } else {
            *it++ = (p & 0x1C0000) >> 18 | 0xF0; // 11110xxx
            *it++ = (p & 0x03F000) >> 12 | 0x80; // 10xxxxxx
            *it++ = (p & 0x000FC0) >> 6 | 0x80; // 10xxxxxx
            *it++ = (p & 0x00003F) | 0x80; // 10xxxxxx
            return it;
        }
    }

    static constexpr auto decode(const_pointer it) noexcept -> code_point {
        if (*it <= 0x7F) {
            return *it;
        } else [[unlikely]] {
            auto length = std::countl_one(*it);

            if (length == 2) {
                return static_cast<code_point>(it[0] & 0x1F) << 6
                    | static_cast<code_point>(it[1] & 0x3F);
            } else if (length == 3) {
                return static_cast<code_point>(it[0] & 0x0F) << 12
                    | static_cast<code_point>(it[1] & 0x3F) << 6
                    | static_cast<code_point>(it[2] & 0x3F);
            } else {
                return static_cast<code_point>(it[0] & 0x07) << 18
                    | static_cast<code_point>(it[1] & 0x3F) << 12
                    | static_cast<code_point>(it[2] & 0x3F) << 6
                    | static_cast<code_point>(it[3] & 0x3F);
            }
        }
    }

    static constexpr auto validate(
        const_pointer it,
        const_pointer end
    ) noexcept -> error_code {
        if (*it <= 0x7F) {
            return error_code::ok;
        } else [[unlikely]] {
            auto length = std::countl_one(*it);

            if (it + length <= end) {
                if (length == 2) {
                    if ((it[1] & 0xC0) == 0x80) {
                        if (it[0] & 0x1E) {
                            return error_code::ok;
                        } else [[unlikely]] {
                            return error_code::overlong_encoding;
                        }
                    } else [[unlikely]] {
                        if (std::countl_one(it[1]) <= 4) {
                            return error_code::unexpected_code_unit;
                        } else {
                            return error_code::invalid_code_unit;
                        }
                    }
                } else if (length == 3) {
                    if (
                        ((static_cast<uint16_t>(it[1]) << 8
                        | it[2]) & 0xC0C0) == 0x8080
                    ) {
                        if (
                            ((it[0] & 0x0F)
                            | (it[1] & 0x20))
                        ) {
                            if (
                                !((it[0] & 0x0F) == 0x0D
                                && (it[1] & 0x20))
                            ) {
                                return error_code::ok;
                            } else [[unlikely]] {
                                return error_code::invalid_code_point;
                            }
                        } else [[unlikely]] {
                            return error_code::overlong_encoding;
                        }
                    } else [[unlikely]] {
                        if (
                            std::countl_one(it[1]) <= 4
                            && std::countl_one(it[2]) <= 4
                        ) {
                            return error_code::unexpected_code_unit;
                        } else {
                            return error_code::invalid_code_unit;
                        }
                    }
                } else if (length == 4) {
                    if (
                        ((static_cast<uint32_t>(it[0]) << 24
                        | static_cast<uint32_t>(it[1]) << 16
                        | static_cast<uint32_t>(it[2]) << 8
                        | it[3]) & 0x00C0C0C0) == 0x00808080
                    ) {
                        if (
                            ((it[0] & 0x07)
                            | (it[1] & 0x30))
                        ) {
                            if (
                                !((it[0] & 0x04)
                                && ((it[0] & 0x03) | (it[1] & 0x30)))
                            ) {
                                return error_code::ok;
                            } else [[unlikely]] {
                                return error_code::invalid_code_point;
                            }
                        } else [[unlikely]] {
                            return error_code::overlong_encoding;
                        }
                    } else [[unlikely]] {
                        if (
                            std::countl_one(it[1]) <= 4
                            && std::countl_one(it[2]) <= 4
                            && std::countl_one(it[3]) <= 4
                        ) {
                            return error_code::unexpected_code_unit;
                        } else {
                            return error_code::invalid_code_unit;
                        }
                    }
                } else [[unlikely]] {
                    return error_code::invalid_code_unit;
                }
            } else [[unlikely]] {
                return error_code::incomplete_sequence;
            }
        }
    }

    static constexpr auto next_code_point(const_pointer it) noexcept -> const_pointer {
        auto length = std::countl_one(*it);
        return length ? (it + length) : (it + 1);
    }

    static constexpr auto prev_code_point(const_pointer it) noexcept -> const_pointer {
        while ((*(--it) & 0xC0) == 0x80);
        return it;
    }
};

static_assert(encoding<utf8>);

} // namespace unicode
} // namespace bigj
