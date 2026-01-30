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

        static constexpr Color DEFAULT_CHECK_BACKGROUND_COLOR = Color::WHITE;
        static constexpr Color DEFAULT_CHECK_BORDER_COLOR = Color::WHITE;

        

        struct Label {
            Color color = Color::WHITE;
            std::string font = "";

            VerticalAlignment verticalAlignment = VerticalAlignment::Center;
            HorizontalAlignment horizontalAlignment = HorizontalAlignment::Center;
            FEdges padding = FEdges(DEFAULT_PADDING);
        };
        struct Panel {
            FEdges border = FEdges(DEFAULT_BORDER_WIDTH);
            FEdges padding = FEdges(DEFAULT_PADDING);

            FCorners borderRadius = FCorners(DEFAULT_BORDER_RADIUS);


            Color borderColor = Color::DARK_MODE_BORDER;
            Color backgroundColor = Color::DARK_MODE_BACKGROUND;

        };
        struct ButtonBase : Panel {
            virtual ~ButtonBase() = 0;

        };
        struct CheckBox : ButtonBase {
            Panel checkMark;
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