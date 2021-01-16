#pragma once

#include "basic_string.hpp"
#include "unicode/encoding/utf8.hpp"
#include "unicode/encoding/utf16.hpp"
#include "unicode/encoding/utf32.hpp"

namespace bigj {

using utf8_string = basic_string<unicode::utf8>;
using utf16be_string = basic_string<unicode::utf16be>;
using utf32be_string = basic_string<unicode::utf32be>;
using utf16le_string = basic_string<unicode::utf16le>;
using utf32le_string = basic_string<unicode::utf32le>;

using string = utf8_string;

} // namespace bigj
