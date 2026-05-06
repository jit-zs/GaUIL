#ifndef GAUIL_RECT_HPP
#define GAUIL_RECT_HPP

#include <GaUIL/Vector2.hpp>

#if __has_include(<SFML/Graphics/Rect.hpp>)
#include <SFML/Graphics/Rect.hpp>
#endif

namespace gauil {

    template<typename T>
    struct Rect {
        union {
            struct {
                T x;
                T y;
                T w;
                T h;
            };
            struct {
                T left;
                T top;
                T right;
                T bottom;
            };
            struct {
                Vector2<T> position;
                Vector2<T> size;
            };
        };
        constexpr Rect(T x, T y, T w, T h) : x(x), y(y), w(w), h(h) {}
        constexpr Rect(const Vector2<T>& position, const Vector2<T>& size) : position(position), size(size) {}
        constexpr Rect() {};


#if __has_include(<SFML/Graphics/Rect.hpp>)
        constexpr Rect(const sf::Rect<T>& rect) : x(rect.left), y(rect.top), w(rect.width), h(rect.height) {}
        constexpr operator sf::Rect<T>() const {
            return sf::Rect<T>(x, y, w, h);
        }
#endif
        [[nodiscard]] constexpr bool contains(const Vector2<T>& point) const {
            return point.x >= x && point.x <= x + w && point.y >= y && point.y <= y + h;
        }
        // returns the position of the center of the rect
        [[nodiscard]] constexpr Vector2<T> getCenter() const{
            return position + size / static_cast<T>(2);
        }
        [[nodiscard]] constexpr static Rect<T> makeFromCenter(const Vector2<T>& position, const Vector2<T>& size) {
            return { position - size / static_cast<T>(2), size };
        }


    };

    typedef Rect<float> FRect;
    typedef Rect<int> IRect;
    typedef Rect<unsigned int> URect;

}
#endif