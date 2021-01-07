#include "../detail/error_code.hpp"

#include <bigj/unicode/encoding/utf16.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include <array>
#include <ranges>
#include <tuple>

using namespace bigj::unicode;

using code_unit = uint16_t;

using cp_iota_view = std::ranges::iota_view<code_point, code_point>;
using cp_range = std::array<code_point, 2>;

TEMPLATE_TEST_CASE("UTF-16 encoding / decoding / iteration", "[utf][utf16]", utf16be, utf16le) {
    auto [range, size, mask, test] = GENERATE(
        values<
            std::tuple<
                cp_iota_view,
                size_t,
                std::array<code_unit, 2>,
                std::array<code_unit, 2>
            >
        >({
            {{0x000000, 0x00D800}, 1, {0x0000, 0xFFFF}, {0x0000, 0x0000}},
            {{0x00E000, 0x010000}, 1, {0x0000, 0xFFFF}, {0x0000, 0x0000}},
            {{0x010000, 0x110000}, 2, {0xFC00, 0xFC00}, {0xD800, 0xDC00}},
        })
    );

    for (auto cp : range) {
        auto data = std::array<code_unit, 2> {};

        REQUIRE(TestType::encoded_size(cp) == size);
        REQUIRE(TestType::encode(cp, data.data()) == data.data() + size);
        REQUIRE(TestType::validate(data.data(), data.data() + size) == error_code::ok);
        REQUIRE(TestType::decode(data.data()) == cp);
        REQUIRE(TestType::next_code_point(data.data()) == data.data() + size);
        REQUIRE(TestType::prev_code_point(data.data() + size) == data.data());

        for (size_t i = 0; i < size; i++) {
            if constexpr (std::same_as<TestType, utf16<std::endian::native>>) {
                REQUIRE((data[i] & mask[i]) == test[i]);
            } else {
                REQUIRE((detail::byte_swap(data[i]) & mask[i]) == test[i]);
            }
        }
    }
}

TEMPLATE_TEST_CASE("UTF-16 validation", "[utf][utf16]", utf16be, utf16le) {
    SECTION("incomplete sequence") {
        auto range = cp_range {0x010000, 0x110000};

        auto cp = GENERATE_COPY(take(100, random(range[0], range[1])));
        auto data = std::array<code_unit, 2> {};

        TestType::encode(cp, data.data());

        REQUIRE(
            TestType::validate(data.data(), data.data() + 1)
            == error_code::incomplete_sequence
        );
    }

    SECTION("unexpected code unit (missing high surrogate)") {
        auto range = cp_range {0x010000, 0x110000};

        auto cps = GENERATE_COPY(take(100, chunk(2, random(range[0], range[1]))));
        auto data = std::array<code_unit, 3> {};

        TestType::encode(cps[0], data.data() + 1);
        TestType::encode(cps[1], data.data());

        REQUIRE(
            TestType::validate(data.data() + 1, data.data() + data.size())
            == error_code::unexpected_code_unit
        );
    }

    SECTION("unexpected code unit (missing low surrogate)") {
        auto range = cp_range {0x010000, 0x110000};

        auto cps = GENERATE_COPY(take(100, chunk(2, random(range[0], range[1]))));
        auto data = std::array<code_unit, 3> {};

        TestType::encode(cps[0], data.data());
        TestType::encode(cps[1], data.data() + 1);

        REQUIRE(
            TestType::validate(data.data(), data.data() + data.size())
            == error_code::unexpected_code_unit
        );
    }
}
