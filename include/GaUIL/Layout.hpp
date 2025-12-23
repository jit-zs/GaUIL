#ifndef GAUIL_LAYOUT_HPP
#define GAUIL_LAYOUT_HPP

#include <GaUIL/Vector2.hpp>
#include <GaUIL/Rect.hpp>

namespace gauil {
    struct Layout {
        enum class Type {
            Pixels,
            Percent,
        } type = Type::Pixels;
        float value = 0;
        constexpr Layout(float value, Type type = Type::Pixels) : type(type), value(value) {}
        constexpr Layout() = default;
        constexpr Layout operator-() const {
            return Layout(-value, type);
        }
    };
    struct Layout2D {
        Layout x;
        Layout y;

        /// @brief returns the layout as a pixel vector based on the given size
        /// @return  
        [[nodiscard]] constexpr Vector2f getPixels(const Vector2f& containerSize) const {
            return {
                x.type == Layout::Type::Pixels ? x.value : (x.value / 100.0f) * containerSize.x,
                y.type == Layout::Type::Pixels ? y.value : (y.value / 100.0f) * containerSize.y
            };
        }
        constexpr Layout2D(Layout x, Layout y) : x(x), y(y) {}
        constexpr Layout2D(const Vector2f& vec) : x(vec.x), y(vec.y) {}
        constexpr Layout2D() = default;
    };

    namespace literals {
        constexpr Layout operator "" _px(unsigned long long v) {
            return Layout(static_cast<float>(v),  Layout::Type::Pixels);
        }
        constexpr Layout operator "" _px(long double v) {
            return Layout(static_cast<float>(v),  Layout::Type::Pixels);
        }

        constexpr Layout operator "" _percent(unsigned long long v) {
            return Layout( static_cast<float>(v), Layout::Type::Percent);
        }
        constexpr Layout operator "" _percent(long double v) {
            return Layout( static_cast<float>(v),  Layout::Type::Percent);
        }
    }
}

#endif