#include "../detail/error_code.hpp"

#include <bigj/unicode/encoding/utf8.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include <array>
#include <ranges>
#include <tuple>

using namespace bigj::unicode;

using code_unit = uint8_t;

using cp_iota_view = std::ranges::iota_view<code_point, code_point>;
using cu_iota_view = std::ranges::iota_view<code_unit, code_unit>;

using cp_range = std::array<code_point, 2>;

TEST_CASE("UTF-8 encoding / decoding / iteration", "[utf][utf8]") {
    auto [range, size, mask, test] = GENERATE(
        values<
            std::tuple<
                cp_iota_view,
                size_t,
                std::array<code_unit, 4>,
                std::array<code_unit, 4>
            >
        >({
            {{0x000000, 0x000080}, 1, {0x80, 0xFF, 0xFF, 0xFF}, {0x00, 0x00, 0x00, 0x00}},
            {{0x000080, 0x000800}, 2, {0xE0, 0xC0, 0xFF, 0xFF}, {0xC0, 0x80, 0x00, 0x00}},
            {{0x000800, 0x00D800}, 3, {0xF0, 0xC0, 0xC0, 0xFF}, {0xE0, 0x80, 0x80, 0x00}},
            {{0x00E000, 0x010000}, 3, {0xF0, 0xC0, 0xC0, 0xFF}, {0xE0, 0x80, 0x80, 0x00}},
            {{0x010000, 0x110000}, 4, {0xF8, 0xC0, 0xC0, 0xC0}, {0xF0, 0x80, 0x80, 0x80}},
        })
    );

    for (auto cp : range) {
        auto data = std::array<code_unit, 4> {};

        REQUIRE(utf8::encoded_size(cp) == size);
        REQUIRE(utf8::encode(cp, data.data()) == data.data() + size);
        REQUIRE(utf8::validate(data.data(), data.data() + size) == error_code::ok);
        REQUIRE(utf8::decode(data.data()) == cp);
        REQUIRE(utf8::next_code_point(data.data()) == data.data() + size);
        REQUIRE(utf8::prev_code_point(data.data() + size) == data.data());

        for (size_t i = 0; i < size; i++) {
            REQUIRE((data[i] & mask[i]) == test[i]);
        }
    }
}

TEST_CASE("UTF-8 validation", "[utf][utf8]") {
    SECTION("incomplete sequence") {
        auto range = GENERATE(values<cp_range>({
            {0x000080, 0x000800},
            {0x000800, 0x010000},
            {0x010000, 0x200000},
        }));

        auto cp = GENERATE_COPY(take(100, random(range[0], range[1])));

        auto data = std::array<code_unit, 4> {};
        auto size = utf8::encoded_size(cp);

        utf8::encode(cp, data.data());

        for (size_t i = 1; i < size; i++) {
            REQUIRE(
                utf8::validate(data.data(), data.data() + (size - i))
                == error_code::incomplete_sequence
            );
        }
    }

    SECTION("unexpected code unit (too few continuation bytes)") {
        auto range_1 = GENERATE(values<cp_range>({
            {0x000080, 0x000800},
            {0x000800, 0x010000},
            {0x010000, 0x200000},
        }));

        auto range_2 = GENERATE_COPY(values<cp_range>({
            {0x000000, 0x000080},
            {0x000080, 0x000800},
            {0x000800, 0x010000},
            {0x010000, 0x200000},
        }));

        auto cp_1 = GENERATE_COPY(take(100, random(range_1[0], range_1[1])));
        auto cp_2 = GENERATE_COPY(take(100, random(range_2[0], range_2[1])));

        auto data = std::array<code_unit, 8> {};
        auto size = utf8::encoded_size(cp_1);

        utf8::encode(cp_1, data.data());

        for (size_t i = 1; i < size; i++) {
            utf8::encode(cp_2, data.data() + (size - i));

            REQUIRE(
                utf8::validate(data.data(), data.data() + data.size())
                == error_code::unexpected_code_unit
            );
        }
    }

    SECTION("unexpected code unit (too many continuation bytes)") {
        auto range = GENERATE(values<cp_range>({
            {0x000000, 0x000080},
            {0x000080, 0x000800},
            {0x000800, 0x010000},
            {0x010000, 0x200000},
        }));

        auto cp = GENERATE_COPY(take(100, random(range[0], range[1])));

        for (auto cu : cu_iota_view {0x80, 0xC0}) {
            auto data = std::array<code_unit, 8> {};

            data[0] = cu;
            utf8::encode(cp, data.data() + 1);

            REQUIRE(
                utf8::validate(data.data(), data.data() + data.size())
                == error_code::unexpected_code_unit
            );
        }
    }

    SECTION("invalid code unit") {
        auto range = GENERATE(values<cp_range>({
            {0x000000, 0x000080},
            {0x000080, 0x000800},
            {0x000800, 0x010000},
            {0x010000, 0x200000},
        }));

        auto cp = GENERATE_COPY(take(100, random(range[0], range[1])));

        for (auto cu : cu_iota_view {0xF8, 0xFF}) {
            auto original_data = std::array<code_unit, 8> {};
            auto size = utf8::encoded_size(cp);

            utf8::encode(cp, original_data.data());

            for (size_t i = 1; i <= size; i++) {
                auto data = original_data;
                data[size - i] = cu;

                REQUIRE(
                    utf8::validate(data.data(), data.data() + data.size())
                    == error_code::invalid_code_unit
                );
            }
        }
    }
    
    SECTION("overlong encoding") {
        auto [range, mask] = GENERATE(
            values<
                std::tuple<
                    cp_range,
                    std::array<code_unit, 4>
                >
            >({
                {{0x000080, 0x000800}, {0xE1, 0xFF, 0xFF, 0xFF}},
                {{0x000800, 0x00D800}, {0xF0, 0xDF, 0xFF, 0xFF}},
                {{0x00E000, 0x010000}, {0xF0, 0xDF, 0xFF, 0xFF}},
                {{0x010000, 0x110000}, {0xF8, 0xCF, 0xFF, 0xFF}},
            })
        );

        auto cp = GENERATE_COPY(take(100, random(range[0], range[1])));
        auto data = std::array<code_unit, 4> {};

        utf8::encode(cp, data.data());

        for (size_t i = 0; i < data.size(); i++) {
            data[i] &= mask[i];
        }

        REQUIRE(
            utf8::validate(data.data(), data.data() + data.size())
            == error_code::overlong_encoding
        );
    }

    SECTION("invalid code point") {
        auto range = GENERATE(values<cp_range>({
            {0x00D800, 0x00E000},
            {0x110000, 0x200000},
        }));

        auto cp = GENERATE_COPY(take(100, random(range[0], range[1])));
        auto data = std::array<code_unit, 4> {};

        utf8::encode(cp, data.data());

        REQUIRE(
            utf8::validate(data.data(), data.data() + data.size())
            == error_code::invalid_code_point
        );
    }
}
