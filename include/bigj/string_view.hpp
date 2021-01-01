#pragma once

#include "basic_string_view.hpp"
#include "unicode/encoding/utf8.hpp"
#include "unicode/encoding/utf16.hpp"
#include "unicode/encoding/utf32.hpp"

namespace bigj {

using utf8_string_view = basic_string_view<unicode::utf8>;
using utf16be_string_view = basic_string_view<unicode::utf16be>;
using utf32be_string_view = basic_string_view<unicode::utf32be>;
using utf16le_string_view = basic_string_view<unicode::utf16le>;
using utf32le_string_view = basic_string_view<unicode::utf32le>;

using string_view = utf8_string_view;

} // namespace bigj
