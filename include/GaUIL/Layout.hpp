#ifndef GAUIL_LAYOUT_HPP
#define GAUIL_LAYOUT_HPP

#include <GaUIL/Assert.hpp>
#include <GaUIL/Rect.hpp>
#include <GaUIL/Vector2.hpp>


namespace gauil {
    struct Layout {
        enum class Type {
            Pixels,
            Percent,
            OtherLayout, // gets the value of x if this layout is y and vice versa. Ignores all other values
        } type = Type::Pixels;
        float value = 0;
        bool scaleWithOpposingAxis = false;
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
            Vector2f result;
            GAUIL_ASSERT(!(x.type == y.type && y.type == Layout::Type::OtherLayout), "Both values of a layout cannot both be OtherLayout");
            switch (x.type) {
            case Layout::Type::Pixels:
                result.x = x.value;
                break;
            case Layout::Type::Percent:
                result.x = (x.value / 100.0f) * (x.scaleWithOpposingAxis ? containerSize.y : containerSize.x);
                break;
            }
            switch (y.type) {
            case Layout::Type::Pixels:
                result.y = y.value;
                break;
            case Layout::Type::Percent:
                result.y = (y.value / 100.0f) * (y.scaleWithOpposingAxis ? containerSize.x :  containerSize.y);
                break;
            case Layout::Type::OtherLayout:
                result.y = result.x;
                break;
            }
            if (x.type == Layout::Type::OtherLayout)
                result.x = result.y;
            return result;
        }
        constexpr Layout2D(Layout x, Layout y) : x(x), y(y) {}
        constexpr Layout2D(const Vector2f& vec) : x(vec.x), y(vec.y) {}
        constexpr Layout2D() = default;
    };
    inline static constexpr Layout OTHER_LAYOUT = { 0, Layout::Type::OtherLayout };
    constexpr Layout scaleWithOpposingAxis(const Layout& other) {
        Layout layout = other;
        layout.scaleWithOpposingAxis = true;
        return layout;
    }
    namespace literals {
        constexpr Layout operator "" _px(unsigned long long v) {
            return Layout(static_cast<float>(v), Layout::Type::Pixels);
        }
        constexpr Layout operator "" _px(long double v) {
            return Layout(static_cast<float>(v), Layout::Type::Pixels);
        }

        constexpr Layout operator "" _percent(unsigned long long v) {
            return Layout(static_cast<float>(v), Layout::Type::Percent);
        }
        constexpr Layout operator "" _percent(long double v) {
            return Layout(static_cast<float>(v), Layout::Type::Percent);
        }

    }
}

#endif