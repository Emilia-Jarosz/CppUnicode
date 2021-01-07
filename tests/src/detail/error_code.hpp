#pragma once

#include <bigj/unicode/detail/error_code.hpp>

#include <catch2/catch_test_macros.hpp>

using namespace bigj::unicode;

CATCH_REGISTER_ENUM(error_code,
    error_code::ok,
    error_code::incomplete_sequence,
    error_code::unexpected_code_unit,
    error_code::invalid_code_unit,
    error_code::overlong_encoding,
    error_code::invalid_code_point
)
