#pragma once

#include <bigj/unicode/encoding/utf32.hpp>

#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include <vector>

template<bigj::unicode::encoding E>
class RandomStringGenerator final
    : public Catch::Generators::IGenerator<std::vector<typename E::code_unit>>
{
    using code_unit = typename E::code_unit;
    using code_point = bigj::unicode::code_point;

    auto make_rng() {
        using namespace bigj::unicode;
        using namespace Catch::Generators;
        using utf32 = utf32<std::endian::native>;

        return filter(
            [](code_point cp) {
                return utf32::validate(&cp, &cp + 1) == error_code::ok;
            },
            random<code_point>(0, 0x10FFFF)
        );
    }

  public:

    RandomStringGenerator(size_t length) : m_code_point_rng {make_rng()}, m_length {length} {
        next();
    }

    auto get() const -> const std::vector<code_unit>& override {
        return m_string;
    }

    auto next() -> bool override {
        auto str = std::vector<code_unit> {};
        str.reserve(m_length);

        for (size_t i = 0; i < m_length; i++) {
            auto cp = m_code_point_rng.get();
            m_code_point_rng.next();

            auto prev_size = str.size();
            str.resize(str.size() + E::encoded_size(cp));
            E::encode(cp, &str[prev_size]);
        }

        m_string = std::move(str);
        return true;
    }

  private:
    Catch::Generators::GeneratorWrapper<code_point> m_code_point_rng;
    const size_t m_length;
    std::vector<code_unit> m_string;
};

template<bigj::unicode::encoding E>
Catch::Generators::GeneratorWrapper<std::vector<typename E::code_unit>>
random_string(size_t length) {
    using Catch::Generators::GeneratorWrapper;

    return GeneratorWrapper<std::vector<typename E::code_unit>>(
        Catch::Detail::make_unique<RandomStringGenerator<E>>(length)
    );
}
