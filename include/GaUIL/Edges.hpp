#ifndef RBUR_BORDERS_HPP
#define RBUR_BORDERS_HPP

#include <type_traits>
#include <math.h>


#include <GaUIL/Math.hpp>
#include <GaUIL/Vector2.hpp>

namespace gauil {
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    struct Edges {
        union {
            struct {
                T right = 0;
                T bottom = 0;
                T left = 0;
                T top = 0;
            };
            struct {
                T edges[4];
            };
        };

        constexpr Edges(T top, T bottom, T left, T right) : top(top), bottom(bottom), left(left), right(right) {}
        constexpr Edges(T all) : top(all), bottom(all), left(all), right(all) {}
        constexpr Edges() = default;
        /// @brief uses linear interpolation to find the proper border radius for the selected angle
        constexpr T angleToBorderWidth(T radians) const {
            int ang = int(radians * RAD2DEG);
            ang %= 360;
            float t = (ang % 90) / 90.0f;

            int primaryEdge = (ang) / 90;
            int secondaryEdge = (ang) / 90 + 1;
            secondaryEdge = secondaryEdge == 4 ? 0 : secondaryEdge;

            T a = edges[primaryEdge];
            T b = edges[secondaryEdge];
            return lerp<T>(a, b, t);
        }
        constexpr T getBorderFromAngle(T radians) const {
            return edges[(int(radians * RAD2DEG) % 360) / 90];
        }
        /// @returns Vector2 where x = left and y = top
        constexpr Vector2<T> getOffset() const {
            return { left, top };
        }
        /// @returns Vector2 where x = right and y = bottom
        constexpr Vector2<T> getBounds() const {
            return { right, bottom };
        }
        constexpr bool isZero() const {
            return !top && !bottom && !left && !right;
        }

    };
    typedef Edges<float> FEdges;
}



#endif