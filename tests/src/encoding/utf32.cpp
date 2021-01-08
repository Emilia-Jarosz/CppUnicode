#include "../detail/error_code.hpp"

#include <bigj/unicode/encoding/utf32.hpp>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include <array>
#include <ranges>

using namespace bigj::unicode;

using code_unit = uint32_t;

using cp_iota_view = std::ranges::iota_view<uint32_t, uint32_t>;
using cp_range = std::array<uint32_t, 2>;

TEMPLATE_TEST_CASE("UTF-32 encoding / decoding / iteration", "[utf][utf32]", utf32be, utf32le) {
    auto range = GENERATE(values<cp_iota_view>({
        {0x000000, 0x00D800},
        {0x00E000, 0x110000},
    }));

    for (auto cp : range) {
        auto data = code_unit {};

        REQUIRE(TestType::encoded_size(cp) == 1);
        REQUIRE(TestType::encode(cp, &data) == (&data + 1));
        REQUIRE(TestType::validate(&data, &data + 1) == error_code::ok);
        REQUIRE(TestType::decode(&data) == cp);
        REQUIRE(TestType::next_code_point(&data) == &data + 1);
        REQUIRE(TestType::prev_code_point(&data + 1) == &data);

        if constexpr (std::same_as<TestType, utf32<std::endian::native>>) {
            REQUIRE(data == cp);
        } else {
            REQUIRE(data == detail::byte_swap(cp));
        }
    }
}

TEMPLATE_TEST_CASE("UTF-32 validation", "[utf][utf32]", utf32be, utf32le) {
    auto range = GENERATE(values<cp_range>({
        {0x0000D800, 0x0000E000},
        {0x00110000, 0xFFFFFFFF},
    }));

    auto cp = GENERATE_COPY(take(100, random(range[0], range[1])));
    auto data = code_unit {};

    TestType::encode(cp, &data);

    REQUIRE(
        TestType::validate(&data, &data + 1)
        == error_code::invalid_code_point
    );
}
