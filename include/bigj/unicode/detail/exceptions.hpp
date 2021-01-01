#pragma once

#include "error_code.hpp"

#include <exception>

namespace bigj {
namespace unicode {

struct parse_error : std::exception {

    explicit parse_error(error_code ec) noexcept : code{ec} {}
    virtual ~parse_error() noexcept = default;

    virtual auto what() const noexcept -> const char* override {
        switch (code) {
          case error_code::incomplete_sequence:
            return "incomplete code unit sequence";
          case error_code::unexpected_code_unit:
            return "unexpected code unit";
          case error_code::invalid_code_unit:
            return "invalid code unit";
          case error_code::overlong_encoding:
            return "code point with overlong encoding";
          case error_code::invalid_code_point:
            return "invalid code point";
            
          default:
            return "unknown error";
        }
    }

    const error_code code;
};

} // namespace unicode
} // namespace bigj
