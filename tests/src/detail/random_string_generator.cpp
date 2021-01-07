#include "error_code.hpp"
#include "random_string_generator.hpp"

#include <bigj/unicode/encoding/utf8.hpp>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_range.hpp>

using namespace bigj::unicode;

TEST_CASE("random_string_generator", "[.]") {
    auto length = GENERATE(range<size_t>(1, 100));
    const auto data = GENERATE_COPY(take(100, random_string<utf8>(length)));

    REQUIRE_FALSE(data.empty());

    auto begin = data.data();
    auto end = data.data() + data.size();

    auto distance = size_t {};

    for (auto ptr = begin; ptr != end; ptr = utf8::next_code_point(ptr), ++distance) {
        CAPTURE(*ptr);
        REQUIRE(utf8::validate(ptr, end) == error_code::ok);
    }

    REQUIRE(distance == length);
}
