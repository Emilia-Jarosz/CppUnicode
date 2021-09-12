#include "detail/random_string_generator.hpp"

#include <bigj/unicode/iterator.hpp>
#include <bigj/unicode/encoding/utf8.hpp>
#include <bigj/unicode/encoding/utf16.hpp>
#include <bigj/unicode/encoding/utf32.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include <compare>
#include <iterator>

using namespace bigj;

TEMPLATE_TEST_CASE("code point iterator", "[iterator][forward_iterator]",
    unicode::utf8,
    unicode::utf16<std::endian::native>,
    unicode::utf32<std::endian::native>
) {
    using E = TestType;
    using iterator = unicode::iterator<E>;

    static_assert(std::forward_iterator<iterator>);
    static_assert(std::three_way_comparable<iterator, std::strong_ordering>);

    auto code_points = GENERATE(
        take(100, random_string<E>(2))
    );

    auto first = code_points.data();
    auto second = E::next_code_point(first);
    auto end = E::next_code_point(second);

    auto first_it = iterator {first};
    auto second_it = iterator {second};
    auto end_it = iterator {end};

    SECTION("address") {
        REQUIRE(first_it.address() == first);
        REQUIRE(second_it.address() == second);
        REQUIRE(end_it.address() == end);
    }

    SECTION("dereference") {
        REQUIRE(*first_it == E::decode(first));
        REQUIRE(*second_it == E::decode(second));
    }

    SECTION("pre-increment") {
        auto it = first_it;

        REQUIRE(it == first_it);
        REQUIRE(++it == second_it);
        REQUIRE(it == second_it);
        REQUIRE(++it == end_it);
        REQUIRE(it == end_it);
    }

    SECTION("post-increment") {
        auto it = first_it;

        REQUIRE(it == first_it);
        REQUIRE(it++ == first_it);
        REQUIRE(it == second_it);
        REQUIRE(it++ == second_it);
        REQUIRE(it == end_it);
    }
}
