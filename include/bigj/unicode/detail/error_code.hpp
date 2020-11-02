#pragma once

namespace bigj {
namespace unicode {

enum class error_code {
    ok,
    incomplete_sequence,
    unexpected_code_unit,
    invalid_code_unit,
    overlong_encoding,
    invalid_code_point
};

} // namespace unicode
} // namespace bigj
