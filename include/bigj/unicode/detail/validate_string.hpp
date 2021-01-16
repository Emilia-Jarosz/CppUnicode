#pragma once

#include "../encoding.hpp"
#include "exceptions.hpp"

namespace bigj {
namespace unicode {
namespace detail {

template<encoding E>
constexpr auto validate_string(
    const typename E::code_unit* ptr,
    const typename E::code_unit* end
) -> size_t {
    size_t length = 0;

    for (auto it = ptr; it != end; it = E::next_code_point(it), ++length) {
        auto ec = E::validate(it, end);

        if (ec != unicode::error_code::ok) [[unlikely]] {
            throw unicode::parse_error{ec};
        }
    }

    return length;
}

} // namespace detail
} // namespace unicode
} // namespace bigj
