#pragma once

#include "code_point.hpp"
#include "detail/error_code.hpp"

#include <cstddef>

namespace bigj {
namespace unicode {

template<typename T>
concept encoding =
    requires { typename T::code_unit; }
    && std::integral<typename T::code_unit>
    && requires (
        code_point value,
        const typename T::code_unit* input_it,
        const typename T::code_unit* end_it,
        typename T::code_unit* output_it
    ) {
        { T::encoded_size(value) } noexcept -> std::same_as<size_t>;
        { T::encode(value, output_it) } noexcept -> std::same_as<decltype(output_it)>;
        { T::decode(input_it) } noexcept -> std::same_as<code_point>;
        { T::validate(input_it, end_it) } noexcept -> std::same_as<error_code>;
        { T::next_code_point(input_it) } noexcept -> std::same_as<decltype(input_it)>;
        { T::prev_code_point(input_it) } noexcept -> std::same_as<decltype(input_it)>;
    };

} // namespace unicode
} // namespace bigj
