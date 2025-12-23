#ifndef GAUIL_MATH_HPP
#define GAUIL_MATH_HPP

#include <type_traits>
#include <cmath>
namespace gauil {
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    T lerp(T from, T to, T point) {
        return from + point * (to - from);
    }
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    T clamp(T min, T max, T val) {
        return std::max(min, std::min(max, val));
    }

    inline constexpr float DEG2RAD = M_PI / 180.0;
    inline constexpr float RAD2DEG = 180.0 / M_PI;
}

#endif