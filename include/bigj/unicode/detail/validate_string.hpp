#pragma once

#include "../encoding.hpp"
#include "exceptions.hpp"

namespace bigj {
namespace unicode {
namespace detail {

template<encoding E>
constexpr auto validate_string(
    const typename E::code_unit* begin,
    const typename E::code_unit* end
) -> void {
    for (auto it = begin; it != end; it = E::next_code_point(it)) {
        auto ec = E::validate(it, end);

        if (ec == unicode::error_code::ok) {
            continue;
        } else {
            throw unicode::parse_error {ec};
        }
    }
}

} // namespace detail
} // namespace unicode
} // namespace bigj
