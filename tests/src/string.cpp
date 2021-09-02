#include "detail/error_code.hpp"
#include "detail/random_string_generator.hpp"

#include <bigj/string.hpp>
#include <bigj/string_view.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_range.hpp>

using namespace bigj;

TEST_CASE("Default string is empty", "[string]") {
    auto str = string {};

    REQUIRE(str.empty());
    REQUIRE(str.length() == 0);

    REQUIRE(str.cbegin() == str.cend());
    REQUIRE(str.crbegin() == str.crend());
}

TEST_CASE("String properties", "[string]") {
    auto length = GENERATE(range<size_t>(1, 100));
    auto data = GENERATE_COPY(take(100, random_string<unicode::utf8>(length)));

    auto str = string {data.data(), data.data() + data.size()};

    CHECK_FALSE(str.empty());
    CHECK(str.code_units().size() == data.size());
    REQUIRE(str.length() == length);

    CHECK(std::distance(str.cbegin(), str.cend()) == (ptrdiff_t)length);
    CHECK(std::distance(str.crbegin(), str.crend()) == (ptrdiff_t)length);

    CHECK(str.front() == *str.begin());
    CHECK(str.front() == *(--str.rend()));
    CHECK(str.back() == *str.rbegin());
    CHECK(str.back() == *(--str.end()));
    REQUIRE(str.code_units().data() != data.data());
}

TEST_CASE("String validating constructors", "[string]") {
    auto length = GENERATE(range<size_t>(1, 100));
    auto data = GENERATE_COPY(take(100, random_string<unicode::utf8>(length)));

    SECTION("bad begin iterator") {
        auto begin = data.data();
        auto end = data.data() + data.size();
        auto it = begin + 1;

        while (it != end && utf8::validate(it, end) == error_code::ok) ++it;

        if (it != end) {
            auto str = string {};
            REQUIRE_THROWS_AS((str = string {it, end}), unicode::parse_error);
        }
    }

    SECTION("bad end iterator") {
        auto begin = data.data();
        auto end = data.data() + data.size();
        auto it = end - 1;

        while (it != begin && utf8::validate(it, end) == error_code::ok) --it;

        if (it != begin) {
            auto str = string {};
            REQUIRE_THROWS_AS((str = string {begin, it}), unicode::parse_error);
        }
    }

    SECTION("bad encoding") {
        auto i = GENERATE_COPY(take(1, random<size_t>(1, length)));
        data[i] = 0xFF;

        auto str = string {};
        REQUIRE_THROWS_AS((str = string {data.data(), data.data() + data.size()}), unicode::parse_error);
    }
}

TEST_CASE("String converting constructor", "[string]") {
    auto length = GENERATE(range<size_t>(1, 100));
    auto data = GENERATE_COPY(take(100, random_string<unicode::utf8>(length)));

    auto str_1 = string {data.data(), data.data() + data.size()};
    auto str_2 = utf16be_string {str_1};

    CHECK(str_1.length() == str_2.length());
    CHECK(str_1.front() == str_2.front());
    CHECK(str_1.back() == str_2.back());
}

TEST_CASE("String copy and move constructors", "[string]") {
    auto length = GENERATE(range<size_t>(1, 100));
    auto data = GENERATE_COPY(take(100, random_string<unicode::utf8>(length)));

    auto str_1 = string {data.data(), data.data() + data.size()};

    SECTION("copy constructor") {
        auto str_2 = string {str_1};

        CHECK(str_1.length() == str_2.length());
        CHECK(str_1.front() == str_2.front());
        CHECK(str_1.back() == str_2.back());
    }

    SECTION("move constructor") {
        auto size = str_1.code_units().size();
        auto length = str_1.length();

        auto str_2 = string {std::move(str_1)};

        CHECK(str_2.code_units().size() == size);
        CHECK(str_2.length() == length);

        CHECK(str_1.code_units().size() == 0);
        CHECK(str_1.length() == 0);
    }
}

TEST_CASE("String copy and move assignment", "[string]") {
    auto length = GENERATE(range<size_t>(1, 100));
    auto data = GENERATE_COPY(take(100, random_string<unicode::utf8>(length)));

    auto str_1 = string {data.data(), data.data() + data.size()};
    auto str_2 = string {};

    SECTION("copy assignment") {
        str_2 = str_1;

        CHECK(str_1.length() == str_2.length());
        CHECK(str_1.front() == str_2.front());
        CHECK(str_1.back() == str_2.back());
    }

    SECTION("move assignment") {
        auto size = str_1.code_units().size();
        auto length = str_1.length();

        str_2 = std::move(str_1);

        CHECK(str_2.code_units().size() == size);
        CHECK(str_2.length() == length);

        CHECK(str_1.code_units().size() == 0);
        CHECK(str_1.length() == 0);
    }
}

TEST_CASE("String to string view conversion operator", "[string]") {
    auto length = GENERATE(range<size_t>(1, 100));
    auto data = GENERATE_COPY(take(100, random_string<unicode::utf8>(length)));

    auto str = string {data.data(), data.data() + data.size()};
    auto sv = static_cast<string_view>(str);

    CHECK(str.code_units().data() == sv.code_units().data());
    CHECK(str.code_units().size() == sv.code_units().size());
    CHECK(str.length() == sv.length());
}

TEST_CASE("String swap", "[string]") {
    auto length = GENERATE(range<size_t>(1, 100));
    auto data = GENERATE_COPY(take(100, chunk(2, random_string<unicode::utf8>(length))));

    auto data_1 = data[0];
    auto data_2 = data[1];

    auto str_1 = string {data_1.data(), data_1.data() + data_1.size()};

    auto size_1 = str_1.code_units().size();
    auto length_1 = str_1.length();

    auto str_2 = string {data_2.data(), data_2.data() + data_2.size()};

    auto size_2 = str_2.code_units().size();
    auto length_2 = str_2.length();

    str_1.swap(str_2);

    CHECK(str_1.code_units().size() == size_2);
    CHECK(str_1.length() == length_2);

    CHECK(str_2.code_units().size() == size_1);
    CHECK(str_2.length() == length_1);
}
