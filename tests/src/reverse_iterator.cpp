#include "detail/random_string_generator.hpp"

#include <bigj/unicode/reverse_iterator.hpp>
#include <bigj/unicode/encoding/utf8.hpp>
#include <bigj/unicode/encoding/utf32.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include <array>

using namespace bigj;
using unicode::utf8;
using iterator = unicode::iterator<utf8>;
using reverse_iterator = unicode::reverse_iterator<utf8>;

TEST_CASE("Default reverse iterator is equal to default iterator", "[reverse_iterator]") {
    constexpr auto it1 = iterator {};
    constexpr auto it2 = reverse_iterator {};

    STATIC_REQUIRE(it2.base() == it1);
}

TEST_CASE("Reverse iterator properties", "[reverse_iterator]") {
    auto code_points = GENERATE(
        take(1000, random_string<unicode::utf32<std::endian::native>>(2))
    );

    auto data = std::array<utf8::code_unit, 8> {};
    auto length_1 = utf8::encoded_size(code_points[0]);
    auto length_2 = utf8::encoded_size(code_points[1]);

    utf8::encode(code_points[0], data.data());
    utf8::encode(code_points[1], data.data() + length_1);

    auto it_1 = iterator {data.data()};
    auto it_2 = iterator {data.data() + length_1};
    auto end = iterator {data.data() + length_1 + length_2};

    auto rit_1 = reverse_iterator {end, it_1};
    auto rit_2 = reverse_iterator {it_2, it_1};
    auto rend = reverse_iterator {it_1, it_1};

    SECTION("dereference") {
        CHECK(*rit_1 == code_points[1]);
        REQUIRE(*rit_2 == code_points[0]);
    }

    SECTION("base") {
        CHECK(rit_1.base() == end);
        CHECK(rit_2.base() == it_2);
        REQUIRE(rend.base() == it_1);
    }

    SECTION("comparison") {
        CHECK(rit_1 != rit_2);
        CHECK(rit_1 < rit_2);
        REQUIRE(rit_2 > rit_1);
    }

    SECTION("pre-increment") {
        auto rit_3 = rit_1;

        CHECK(rit_3 == rit_1);
        CHECK(++rit_3 == rit_2);
        CHECK(rit_3 == rit_2);
        REQUIRE(rit_3.base() == it_2);
    }

    SECTION("post-increment") {
        auto rit_3 = rit_1;

        CHECK(rit_3 == rit_1);
        CHECK(rit_3++ == rit_1);
        CHECK(rit_3 == rit_2);
        REQUIRE(rit_3.base() == it_2);
    }

    SECTION("pre-decrement") {
        auto rit_3 = rit_2;

        CHECK(rit_3 == rit_2);
        CHECK(--rit_3 == rit_1);
        CHECK(rit_3 == rit_1);
        REQUIRE(rit_3.base() == end);
    }

    SECTION("post-decrement") {
        auto rit_3 = rit_2;

        CHECK(rit_3 == rit_2);
        CHECK(rit_3-- == rit_2);
        CHECK(rit_3 == rit_1);
        REQUIRE(rit_3.base() == end);
    }
}
