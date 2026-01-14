#ifndef GAUIL_VECTOR2_HPP
#define GAUIL_VECTOR2_HPP

#include <type_traits>

#if __has_include(<SFML/System/Vector2.hpp>)
#include <SFML/System/Vector2.hpp>
#endif

#include <cmath>
namespace gauil {
    template <typename T, std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
    struct Vector2 {
        T x = 0;
        T y = 0;

        inline static constexpr Vector2 ZERO = { 0, 0 };
        constexpr Vector2(T x, T y) : x(x), y(y) {}
        constexpr Vector2() = default;
#if __has_include(<SFML/System/Vector2.hpp>)
        constexpr Vector2(const sf::Vector2<T>& vec) : x(vec.x), y(vec.y) {}
        constexpr operator sf::Vector2<T>() const {
            return sf::Vector2<T>(x, y);
        }
#endif
        template <typename U, std::enable_if_t<std::is_arithmetic_v<U>, bool> = true>
        constexpr explicit operator Vector2<U>() const {
            return Vector2<U>(static_cast<U>(x), static_cast<U>(y));
        }


        [[nodiscard]] constexpr Vector2 operator+(const Vector2& other) const noexcept {
            return Vector2(x + other.x, y + other.y);
        }
        [[nodiscard]] constexpr Vector2 operator-(const Vector2& other) const noexcept {
            return Vector2(x - other.x, y - other.y);
        }
        [[nodiscard]] constexpr Vector2 operator*(const Vector2& other) const noexcept {
            return Vector2(x * other.x, y * other.y);
        }
        [[nodiscard]] constexpr Vector2 operator/(const Vector2& other) const {
            return Vector2(x / other.x, y / other.y);
        }
        constexpr Vector2& operator+=(const Vector2& other) noexcept {
            x += other.x;
            y += other.y;
            return *this;
        }
        constexpr Vector2& operator-=(const Vector2& other) noexcept {
            x -= other.x;
            y -= other.y;
            return *this;
        }
        constexpr Vector2& operator*=(const Vector2& other) noexcept {
            x *= other.x;
            y *= other.y;
            return *this;
        }
        constexpr Vector2& operator/=(const Vector2& other) {
            x /= other.x;
            y /= other.y;
            return *this;
        }
        [[nodiscard]] constexpr Vector2 operator*(T scalar) const noexcept {
            return Vector2(x * scalar, y * scalar);
        }
        [[nodiscard]] constexpr Vector2 operator/(T scalar) const {
            return Vector2(x / scalar, y / scalar);
        }
        constexpr Vector2& operator*=(T scalar) noexcept {
            x *= scalar;
            y *= scalar;
            return *this;
        }
        constexpr Vector2& operator/=(T scalar) {
            x /= scalar;
            y /= scalar;
            return *this;
        }

        [[nodiscard]] constexpr friend Vector2 operator*(T scalar, const Vector2& vec) noexcept {
            return Vector2(scalar * vec.x, scalar * vec.y);
        }
        [[nodiscard]] constexpr friend Vector2 operator/(T scalar, const Vector2& vec) {
            return Vector2(scalar / vec.x, scalar / vec.y);
        }
        // Returns which component is the smallest
        [[nodiscard]] constexpr T min() const {
            return x < y ? x : y;
        }
        // Returns which component is the greatest
        [[nodiscard]] constexpr T max() const {
            return x > y ? x : y;
        }

        [[nodiscard]] constexpr Vector2 magnitude() const {
            return std::sqrt(x * x + y * y);
        }

        [[nodiscard]] constexpr Vector2 normalized() const {
            return *this / magnitude();
        }
        [[nodiscard]] constexpr Vector2 ceil() const {
            return { std::ceil(x), std::ceil(y) };
        }
        /// @returns { ceil(abs(x)) * sign(x), ceil(abs(y)) * sign(y) }
        [[nodiscard]] constexpr Vector2 ceilWithAbs(){
            return {std::ceil(std::abs(x)) * (std::signbit(x) ? -1 : 1), std::ceil(std::abs(y)) * (std::signbit(y) ? -1 : 1)};
        }
        [[nodiscard]]static constexpr Vector2<T> fromAngle(float radians) {
            return Vector2<T>{std::cos(radians), std::sin(radians)};
        }
    };
    typedef Vector2<int> Vector2i;
    typedef Vector2<float>  Vector2f;
    typedef Vector2<unsigned int> Vector2u;





}

#endif