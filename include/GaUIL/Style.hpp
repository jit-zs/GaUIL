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
        static constexpr float DEFAULT_PADDING = 3;
        static constexpr float DEFAULT_BORDER_WIDTH = 2;
        static constexpr float DEFAULT_BORDER_RADIUS = 5;

        static constexpr Color DEFAULT_TEXT_COLOR{ 255, 255, 255, 255 };

        static constexpr Color DEFAULT_CHECK_BACKGROUND_COLOR = WHITE;
        static constexpr Color DEFAULT_CHECK_BORDER_COLOR = WHITE;

        

        struct Label {
            Color color = WHITE;
            std::string font = "";

            VerticalAlignment verticalAlignment = VerticalAlignment::Center;
            HorizontalAlignment horizontalAlignment = HorizontalAlignment::Center;
            FEdges padding = FEdges(DEFAULT_PADDING);
        };
        struct Panel {
            FEdges border = FEdges(DEFAULT_BORDER_WIDTH);
            FEdges padding = FEdges(DEFAULT_PADDING);

            FCorners borderRadius = FCorners(DEFAULT_BORDER_RADIUS);


            Color borderColor = DARK_MODE_BORDER;
            Color backgroundColor = DARK_MODE_BACKGROUND;
        };

        struct Slider : Panel{
            Panel handle;
        };
        struct CheckBox : Panel {
            Panel checkMark;
        };

        struct Button : Panel {
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