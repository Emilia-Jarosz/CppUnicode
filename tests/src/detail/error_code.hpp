#pragma once

#include <bigj/unicode/detail/error_code.hpp>

#include <catch2/catch_test_macros.hpp>

CATCH_REGISTER_ENUM(bigj::unicode::error_code,
    bigj::unicode::error_code::ok,
    bigj::unicode::error_code::incomplete_sequence,
    bigj::unicode::error_code::unexpected_code_unit,
    bigj::unicode::error_code::invalid_code_unit,
    bigj::unicode::error_code::overlong_encoding,
    bigj::unicode::error_code::invalid_code_point
)
