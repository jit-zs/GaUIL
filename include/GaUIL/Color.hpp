#ifndef GAUIL_COLOR_HPP
#define GAUIL_COLOR_HPP

#include <array>
#include <cstdint>


#if __has_include(<SFML/Graphics/Color.hpp>)
#include <SFML/Graphics/Color.hpp>
#endif

namespace gauil {
    struct Color {
        union {
            struct {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                uint8_t a;
            };
            std::array<uint8_t, 4> array;
        };
        static const Color GREEN;
        static const Color WHITE;
        static const Color BLACK;


        static const Color DARK_MODE_BORDER;
        static const Color DARK_MODE_BACKGROUND;

        /// @brief Just the no texture harsh magenta
        static const Color FALLBACK;

        constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
        constexpr Color(const std::array<uint8_t, 4>& array) : array(array) {}
        constexpr Color() : r(0), g(0), b(0), a(255) {};
#if __has_include(<SFML/Graphics/Color.hpp>)
        constexpr Color(const sf::Color& color) : r(color.r), g(color.g), b(color.b), a(color.a) {}
        constexpr operator sf::Color() const {
            return sf::Color(r, g, b, a);
        }
#endif

    };
    inline const Color Color::GREEN = Color(55, 255, 55);
    inline const Color Color::WHITE = Color(255, 255, 255);
    inline const Color Color::BLACK = Color(0, 0, 0);
    inline const Color Color::DARK_MODE_BORDER = Color(32, 32, 32);
    inline const Color Color::DARK_MODE_BACKGROUND = Color(64, 64, 64);
    inline const Color Color::FALLBACK = Color(255, 0, 255);


}

#endif