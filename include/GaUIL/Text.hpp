#ifndef GAUIL_TEXT_HPP
#define GAUIL_TEXT_HPP
#include <algorithm>
#include <memory>
#include <string>
#include <GaUIL/Color.hpp>
#include <GaUIL/Vector2.hpp>

namespace gauil {
    /// @brief \b FOR \b INTERNAL \b USE \b ONLY
    class IFont {
    public:
        virtual void draw(const gauil::Vector2f& position, const std::string& text, uint fontSize, float scale, const gauil::Color& color) const = 0;
        virtual gauil::Vector2f measureText( const std::string& text, uint fontSize) const = 0;
        virtual ~IFont() = 0;
    };
    /// @brief Loads a font into a font object. \b FOR \b INTERNAL \b USE \b ONLY
    /// @param bytes 
    /// @param length 
    /// @returns A loaded font or nullptr on failure
    IFont* loadFont(const uint8_t* bytes, size_t length);
    void unloadFont(IFont* font);
}

#endif