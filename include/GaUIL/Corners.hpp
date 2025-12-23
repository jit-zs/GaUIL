#ifndef RBUR_CORNERS_HPP
#define RBUR_CORNERS_HPP

#include <type_traits>

namespace gauil{
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    struct Corners{
        T topRight = 0;
        T topLeft = 0;
        T bottomRight = 0;
        T bottomLeft = 0;
        constexpr Corners(T tr, T tl, T br, T bl) : topRight (tr), topLeft(tl), bottomRight(br), bottomLeft(bl){}
        constexpr Corners(T all) : topRight(all), topLeft(all), bottomRight(all), bottomLeft(all){}
        constexpr Corners() = default;
        constexpr bool isZero() const{
            return !topRight && !topLeft && !bottomRight && !bottomLeft;
        }
    };
    typedef Corners<float> FCorners;

}


#endif