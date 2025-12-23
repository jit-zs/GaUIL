#include <GaUIL/GUI.hpp>
#include <GaUIL/Assert.hpp>
#include <GaUIL/Draw.hpp>
#include <GaUIL/Style.hpp>

#include <algorithm>
#include <any>
#include <map>
#include <stack>
#include <string>
#include <vector>



namespace gauil {


    struct DrawCall {
        enum Type {
            None,
            TriangleList,
            TriangleStrip,
            TriangleFan,
            Rect,
            Text
        } type = None;
        FRect rect;
        Vector2f textPos;
        uint fontSize;
        Color backgroundColor;
        Color borderColor;
        float textScale = 1;
        std::string text;
        std::any font;
        FCorners borderRadius;
        FEdges borderWidth;
        std::vector<Vertex> vertices;
    };



    static Vector2u windowSize;
    static Vector2i mousePos;
    static bool uiActive = false;

    static bool mouseDownLastFrame = false;
    static bool mouseDown = false;


    static Style style;
    static std::stack<FRect> subRectStack;
    static Vector2f subRectOffset;


    static std::vector<DrawCall> cachedDrawCalls; // All elements to be drawn this frame

    static std::function<GetWindowSizeCallback> windowSizeFn;
    static std::function<GetMousePositionCallback> mousePosFn;
    static std::function<IsMousePressedCallback> mousePressedFn;

    inline Vector2f getCurrentOffset() {
        return subRectOffset;
    }
    inline Vector2f layoutToPixel(const Layout2D& layout) {
        return layout.getPixels(subRectStack.top().size);
    }

    /// @brief Same as layout to size but adds subRectOffset to the result
    /// @param layout 
    /// @note Only use this function in non raw UI elem functions
    /// @return 
    inline Vector2f layoutToPosition(const Layout2D& layout) {
        return layout.getPixels(subRectStack.top().size) + subRectOffset;
    }
    void queueTriangleList(const std::vector<Vertex>& vertices, const Color& color) {
        DrawCall drawCall;
        drawCall.type = DrawCall::TriangleList;
        drawCall.vertices = vertices;
        drawCall.backgroundColor = color;
        cachedDrawCalls.push_back(drawCall);
    }

    void queueTriangleStrip(const std::vector<Vertex>& vertices, const Color& color) {
        DrawCall drawCall;
        drawCall.type = DrawCall::TriangleStrip;
        drawCall.vertices = vertices;
        drawCall.backgroundColor = color;
        cachedDrawCalls.push_back(drawCall);
    }

    void queueTriangleFan(const std::vector<Vertex>& vertices, const Color& color) {
        DrawCall drawCall;
        drawCall.type = DrawCall::TriangleFan;
        drawCall.vertices = vertices;
        drawCall.backgroundColor = color;
        cachedDrawCalls.push_back(drawCall);
    }

    void queueRect(const FRect& rect, const FCorners& borderRadius, const FEdges& borderWidth, const Color& backgroundColor, const Color& borderColor) {
        DrawCall drawCall;
        drawCall.type = DrawCall::Rect;
        drawCall.rect = rect;
        drawCall.backgroundColor = backgroundColor;
        drawCall.borderColor = borderColor;
        drawCall.borderRadius = borderRadius;
        drawCall.borderWidth = borderWidth;
        cachedDrawCalls.push_back(drawCall);
    }



    void queueText(const Vector2f& pos, uint fontSize, float scale, const std::string& text, const std::any& font, const Color& color) {
        DrawCall drawCall;
        drawCall.type = DrawCall::Text;
        drawCall.textPos = pos;
        drawCall.fontSize = fontSize;
        drawCall.text = text;
        drawCall.textScale = scale;
        drawCall.font = font;
        drawCall.backgroundColor = color;
        cachedDrawCalls.push_back(drawCall);
    }

    void init() {
        GAUIL_ASSERT(getDefaultFont().has_value(), "No default font has been chosen");

        GAUIL_ASSERT(getWindowSizeFn(), "No window size function has been set");
        GAUIL_ASSERT(getMousePositionFn(), "No mouse position function has been set");
        GAUIL_ASSERT(getMouseDownFn(), "No mouse pressed function has been set");

        GAUIL_ASSERT(getTriangleListDrawFn(), "No triangle list draw function has been set");
        GAUIL_ASSERT(getTriangleFanDrawFn(), "No triangle fan draw function has been set");
        GAUIL_ASSERT(getTriangleStripDrawFn(), "No triangle strip draw function has been set");

        GAUIL_ASSERT(getRectDrawFn(), "No rect draw function has been set");
        GAUIL_ASSERT(getTextDrawFn(), "No text draw function has been set");
        GAUIL_ASSERT(getMeasureTextFn(), "No measure text function has been set");

        subRectStack.push({ {}, (Vector2f)windowSizeFn() });
        subRectOffset = Vector2f::ZERO;
    }
    void cleanup() {

    }

    void pushSubRect(const Layout2D& position, const Layout2D& size) {
        const FRect& rect = subRectStack.top();

        Vector2f offset = position.getPixels(rect.size);
        subRectStack.push({ offset, size.getPixels(rect.size) });
        subRectOffset += offset;
    }
    void popSubRect() {
        GAUIL_ASSERT(subRectStack.size() > 1, "Sub rect stack underflow");
        const FRect& rect = subRectStack.top();
        subRectOffset -= rect.position;
        subRectStack.pop();
    }

    void setStyle(const Style& style) {
        gauil::style = style;

    }

    void labelRaw(const std::string& text, const Layout2D& position, const Layout2D& size, const Style::Label& style) {
        const Vector2f pixPos = layoutToPixel(position);
        const Vector2f pixSize = layoutToPixel(size);

        const Vector2f textBox = priv::measureText(text, pixSize.min(), getDefaultFont());
        const FRect textRect(pixPos, pixSize);
        const Vector2f compressedDimensions = ((textRect.size) / textBox);
        const float scale = compressedDimensions.min();
        const Vector2f finalTextBox = textBox * scale;

        const Vector2f textBoxAlignment = [&](VerticalAlignment vAlign, HorizontalAlignment hAlign) {
            Vector2f result;
            switch (vAlign) {
            case VerticalAlignment::Top:
                result.y = 0;
                break;
            case VerticalAlignment::Center:
                result.y = (textRect.size.y - finalTextBox.y) / 2.f;
                break;
            case VerticalAlignment::Bottom:
                result.y = textRect.size.y - finalTextBox.y;
                break;
            }

            switch (hAlign) {
            case HorizontalAlignment::Left:
                result.x = 0;
                break;
            case HorizontalAlignment::Center:
                result.x = (textRect.size.x - finalTextBox.x) / 2.f;
                break;
            case HorizontalAlignment::Right:
                result.x = textRect.size.x - finalTextBox.x;
                break;
            }

            return result;
            }(style.verticalAlignment, style.horizontalAlignment);



        // queueRect(textRect, {}, {}, color::FALLBACK, {});
        queueText(textRect.position + textBoxAlignment, pixSize.min(), scale, text, getDefaultFont(), style.color); // Draw text
    }
    void label(const std::string& text, const Layout2D& position, const Layout2D& size) {
        labelRaw(text, position, size, style.label);
    }

    bool button(const std::string& text, const Layout2D& position, const Layout2D& size) {
        Vector2f pixPos = layoutToPosition(position);
        Vector2f pixSize = layoutToPixel(size);

        const Style::Button& buttonStyle = [&] {
            FRect rect(pixPos, pixSize);
            if (rect.contains(static_cast<Vector2f>(mousePos)))
                return priv::isMouseHeld() ? style.clickedButton : style.hovererdButton;
            return style.button;
            }();

        queueRect(FRect{ pixPos, pixSize }, buttonStyle.borderRadius, buttonStyle.border, buttonStyle.backgroundColor, buttonStyle.borderColor); // Draw border

        Vector2f backgroundOffset = Vector2f(buttonStyle.border.left, buttonStyle.border.top);
        Vector2f backgroundBounds = Vector2f(buttonStyle.border.right, buttonStyle.border.bottom);

        Vector2f textOffset = Vector2f(buttonStyle.textPadding.left, buttonStyle.textPadding.top);
        Vector2f textBounds = Vector2f(buttonStyle.textPadding.right, buttonStyle.textPadding.bottom);

        const FRect textRect = FRect(pixPos + backgroundOffset + textOffset, pixSize - backgroundOffset - backgroundBounds - textOffset - textBounds);
        const Vector2f textBox = priv::measureText(text, textRect.size.min(), getDefaultFont());
        if (textBox.min() > 0) {
            labelRaw(text, textRect.position, textRect.size, buttonStyle.label);
        }
        FRect rect(pixPos, pixSize);
        return rect.contains((Vector2f)mousePos) && priv::isMouseUp();
    }



    void update() {
        GAUIL_ASSERT(!uiActive, "Each call to gauil::update must have a corresponding call to gauil::draw");
        uiActive = true;

        cachedDrawCalls.clear();


        mouseDown = mousePressedFn();
        windowSize = windowSizeFn();
        subRectStack.top() = { {}, (Vector2f)windowSize };
        mousePos = mousePosFn();
    }

    void draw() {
        GAUIL_ASSERT(uiActive, "gauil::update was not called before gauil::draw");
        GAUIL_ASSERT(subRectStack.size() == 1, "The child rect stack was not cleaned properly");
        uiActive = false;


        for (const DrawCall& elem : cachedDrawCalls) {
            switch (elem.type) {
            case DrawCall::TriangleList:
                priv::drawTriangleList(elem.vertices.data(), elem.vertices.size(), elem.backgroundColor);
                break;
            case DrawCall::TriangleFan:
                priv::drawTriangleFan(elem.vertices.data(), elem.vertices.size(), elem.backgroundColor);
                break;
            case DrawCall::TriangleStrip:
                priv::drawTriangleStrip(elem.vertices.data(), elem.vertices.size(), elem.backgroundColor);
                break;
            case DrawCall::Rect:
                priv::roundRect(elem.rect, elem.borderRadius, elem.borderWidth, elem.backgroundColor, elem.borderColor);
                break;
            case DrawCall::Text:
                priv::drawText(elem.textPos, elem.fontSize, elem.textScale, elem.text, elem.backgroundColor, elem.font);
                break;
            }
        }


        mouseDownLastFrame = mouseDown;
        mouseDown = false;
    }

    void setWindowSizeFn(const std::function<GetWindowSizeCallback>& fn) {
        windowSizeFn = fn;
    }
    std::function<GetWindowSizeCallback> getWindowSizeFn() {
        return windowSizeFn;
    }
    void setMousePositionFn(const std::function<GetMousePositionCallback>& fn) {
        mousePosFn = fn;
    }

    std::function<GetMousePositionCallback> getMousePositionFn() {
        return mousePosFn;
    }

    void setMouseDownFn(const std::function<bool()>& fn) {
        mousePressedFn = fn;
    }

    std::function<bool()> getMouseDownFn() {
        return mousePressedFn;
    }
    namespace priv {
        bool isMouseDown() {
            return mouseDown && !mouseDownLastFrame;
        }
        bool isMouseHeld() {
            return mouseDown && mouseDownLastFrame;
        }
        bool isMouseUp() {
            return !mouseDown && mouseDownLastFrame;
        }

        Vector2u windowSize() {
            return windowSizeFn();
        }
        Vector2i mousePos() {
            return mousePosFn();
        }
    }
}