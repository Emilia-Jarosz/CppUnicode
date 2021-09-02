#pragma once

namespace bigj {
namespace unicode {
namespace config {

#ifndef CPPUNICODE_NULL_TERMINATORS
    #define CPPUNICODE_NULL_TERMINATORS 0
#endif

constexpr bool null_terminators = CPPUNICODE_NULL_TERMINATORS;

} // namespace config
} // namespace unicode
} // namespace bigj
