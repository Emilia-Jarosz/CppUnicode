#include "detail/random_string_generator.hpp"

#include <bigj/unicode/iterator.hpp>
#include <bigj/unicode/encoding/utf8.hpp>
#include <bigj/unicode/encoding/utf32.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include <array>

using namespace bigj;

using unicode::utf8;
using iterator = unicode::iterator<utf8>;

TEST_CASE("Default iterator is equal to nullptr", "[iterator]") {
    constexpr auto it = iterator {};

    STATIC_REQUIRE(it.base() == nullptr);
}

TEST_CASE("Iterator properties", "[iterator]") {
    auto code_points = GENERATE(
        take(1000, random_string<unicode::utf32<std::endian::native>>(2))
    );

    auto data = std::array<utf8::code_unit, 8> {};
    auto length_1 = utf8::encoded_size(code_points[0]);

    utf8::encode(code_points[0], data.data());
    utf8::encode(code_points[1], data.data() + length_1);

    auto it_1 = iterator {data.data()};
    auto it_2 = iterator {data.data() + length_1};

    SECTION("dereference") {
        CHECK(*it_1 == code_points[0]);
        REQUIRE(*it_2 == code_points[1]);
    }

    SECTION("base") {
        CHECK(it_1.base() == data.data());
        REQUIRE(it_2.base() == data.data() + length_1);
    }

    SECTION("comparison") {
        CHECK(it_1 != it_2);
        CHECK(it_1 < it_2);
        REQUIRE(it_2 > it_1);
    }

    SECTION("pre-increment") {
        auto it_3 = it_1;

        CHECK(it_3 == it_1);
        CHECK(++it_3 == it_2);
        CHECK(it_3 == it_2);
        REQUIRE(it_3.base() == data.data() + length_1);
    }

    SECTION("post-increment") {
        auto it_3 = it_1;

        CHECK(it_3 == it_1);
        CHECK(it_3++ == it_1);
        CHECK(it_3 == it_2);
        REQUIRE(it_3.base() == data.data() + length_1);
    }

    SECTION("pre-decrement") {
        auto it_3 = it_2;

        CHECK(it_3 == it_2);
        CHECK(--it_3 == it_1);
        CHECK(it_3 == it_1);
        REQUIRE(it_3.base() == data.data());
    }

    SECTION("post-decrement") {
        auto it_3 = it_2;

        CHECK(it_3 == it_2);
        CHECK(it_3-- == it_2);
        CHECK(it_3 == it_1);
        REQUIRE(it_3.base() == data.data());
    }
}
