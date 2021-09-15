#pragma once

#include <bigj/unicode/encoding/utf32.hpp>

#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

inline auto random_code_points(size_t size)
    -> Catch::Generators::GeneratorWrapper<std::vector<uint32_t>>
{
    using namespace Catch::Generators;
    using enum bigj::unicode::error_code;
    using utf32 = bigj::unicode::utf32<std::endian::native>;

    return chunk(size, filter(
        [](uint32_t cp) { return utf32::validate(&cp, &cp + 1) == ok; },
        random<uint32_t>(0, 0x10FFFF)
    ));
}

template<bigj::unicode::encoding E>
auto random_string(size_t length)
    -> Catch::Generators::GeneratorWrapper<std::vector<typename E::code_unit>>
{
    using namespace Catch::Generators;

    return map(
        [](const auto& code_points) {
            auto size = size_t {0};
            for (auto cp : code_points) size += E::encoded_size(cp);

            auto str = std::vector<typename E::code_unit> {};
            str.resize(size);

            auto data = str.data();
            for (auto cp : code_points) data = E::encode(cp, data);

            return str;
        },
        random_code_points(length)
    );
}
