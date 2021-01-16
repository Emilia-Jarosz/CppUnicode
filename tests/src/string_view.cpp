#include "detail/error_code.hpp"
#include "detail/random_string_generator.hpp"

#include <bigj/string_view.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_range.hpp>

using namespace bigj;

TEST_CASE("Default string view is empty", "[string_view]") {
    constexpr auto str = string_view {};

    STATIC_REQUIRE(str.empty());
    STATIC_REQUIRE(str.size() == 0);
    STATIC_REQUIRE(str.length() == 0);

    STATIC_REQUIRE(str.cbegin() == str.cend());
    STATIC_REQUIRE(str.crbegin() == str.crend());

    STATIC_REQUIRE(str.data() == nullptr);
}

TEST_CASE("String view properties", "[string_view]") {
    auto length = GENERATE(range<size_t>(1, 100));
    auto data = GENERATE_COPY(take(100, random_string<unicode::utf8>(length)));

    auto str = string_view {};
    REQUIRE_NOTHROW(str = string_view {data.begin(), data.end()});

    CHECK_FALSE(str.empty());
    CHECK(str.size() == data.size());
    REQUIRE(str.length() == length);

    CHECK(std::distance(str.cbegin(), str.cend()) == (ptrdiff_t)length);
    CHECK(std::distance(str.crbegin(), str.crend()) == (ptrdiff_t)length);

    CHECK(str.front() == *str.begin());
    CHECK(str.front() == *(--str.rend()));
    CHECK(str.back() == *str.rbegin());
    CHECK(str.back() == *(--str.end()));
    REQUIRE(str.data() == data.data());
}

TEST_CASE("String view validating constructors", "[string_view]") {
    auto length = GENERATE(range<size_t>(1, 100));
    auto data = GENERATE_COPY(take(100, random_string<unicode::utf8>(length)));

    SECTION("bad begin iterator") {
        auto begin = data.data();
        auto end = data.data() + data.size();
        auto it = begin + 1;

        while (it != end && utf8::validate(it, end) == error_code::ok) ++it;

        if (it != end) {
            auto str = string_view {};
            REQUIRE_THROWS_AS((str = string_view {it, end}), unicode::parse_error);
        }
    }

    SECTION("bad end iterator") {
        auto begin = data.data();
        auto end = data.data() + data.size();
        auto it = end - 1;

        while (it != begin && utf8::validate(it, end) == error_code::ok) --it;

        if (it != begin) {
            auto str = string_view {};
            REQUIRE_THROWS_AS((str = string_view {begin, it}), unicode::parse_error);
        }
    }

    SECTION("bad encoding") {
        auto i = GENERATE_COPY(take(1, random<size_t>(1, length)));
        data[i] = 0xFF;

        auto str = string_view {};
        REQUIRE_THROWS_AS((str = string_view {data.begin(), data.end()}), unicode::parse_error);
    }
}
