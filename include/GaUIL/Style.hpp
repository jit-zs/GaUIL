#ifndef GAUIL_STYLE_HPP
#define GAUIL_STYLE_HPP

#include <GaUIL/Edges.hpp>
#include <GaUIL/Color.hpp>
#include <GaUIL/Corners.hpp>

#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <string>

namespace gauil {
    enum class VerticalAlignment {
        Top, Center, Bottom
    };
    enum class HorizontalAlignment {
        Left, Center, Right
    };
    struct Style {
        static constexpr float DEFAULT_TEXT_PADDING = 3;
        static constexpr float DEFAULT_BORDER_WIDTH = 2;
        static constexpr float DEFAULT_BORDER_RADIUS = 5;

        static constexpr Color DEFAULT_TEXT_COLOR {255, 255, 255, 255};

        struct Label {
            Color color = color::WHITE;
            std::string font = "";

            VerticalAlignment verticalAlignment = VerticalAlignment::Center;
            HorizontalAlignment horizontalAlignment = HorizontalAlignment::Center;
        };
        struct ButtonBase {
            FEdges border = FEdges(DEFAULT_BORDER_WIDTH);
            FEdges textPadding = FEdges(DEFAULT_TEXT_PADDING);

            FCorners borderRadius = FCorners(DEFAULT_BORDER_RADIUS);


            Color borderColor = color::DARK_MODE_BORDER;
            Color backgroundColor = color::DARK_MODE_BACKGROUND;


            virtual ~ButtonBase() = 0;

        };


        struct Button : ButtonBase {
            Label label;
        };

        Button button;
        Button hovererdButton;
        Button clickedButton;

        Label label;

        Style();
    private:
    };

};

#endif