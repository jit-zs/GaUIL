#ifndef GAUIL_COLOR_HPP
#define GAUIL_COLOR_HPP

#include <cstdint>

#if __has_include(<SFML/Graphics/Color.hpp>)
#include <SFML/Graphics/Color.hpp>
#endif

namespace gauil {
    struct Color {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 255;

        constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
        constexpr Color() = default;
#if __has_include(<SFML/Graphics/Color.hpp>)
        constexpr Color(const sf::Color& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}
        constexpr operator sf::Color() const {
            return sf::Color(r, g, b, a);
        }
#endif

    };

    namespace color {
        constexpr Color WHITE = Color(255, 255, 255);
        constexpr Color BLACK = Color(0, 0, 0);
        constexpr Color GREEN = Color(55, 255, 55);

        constexpr Color DARK_MODE_BORDER = Color(32, 32, 32);
        constexpr Color DARK_MODE_BACKGROUND = Color(64, 64, 64);

        /// @brief Just the no texture harsh magenta
        constexpr Color FALLBACK = Color(255, 0, 255);
    }

}

#endif