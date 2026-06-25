#include <GaUIL/GUI.hpp>
#include <GaUIL/Assert.hpp>
#include <GaUIL/Draw.hpp>
#include <GaUIL/NineSlice.hpp>
#include <GaUIL/Style.hpp>
#include <GaUIL/Text.hpp>

#include <algorithm>
#include <any>
#include <deque>
#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <stack>
#include <string>
#include <vector>

#include <SimpleSS/SimpleSS.hpp>

using namespace std::string_literals;
namespace gauil {
    static std::map<std::string, std::map<std::string, std::any>> auxiliaryUIData;
    static std::set<std::string> drawnUIElements;

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
        IFont* font;
        FCorners borderRadius;
        FEdges borderWidth;
        std::vector<Vertex> vertices;
    };

    class LabelData {
        std::string mText, mId, mStyle;

      public:
        LabelData() = delete;
        LabelData(const std::string& raw) {
            static const std::string textDelimiter = "[$$]";
            static const std::string styleDelimiter = "[@@]";

            bool useStyleDelimOnText = raw.find(textDelimiter) == std::string::npos;
            mText = std::string(raw.begin(), std::search(raw.begin(), raw.end(), std::boyer_moore_searcher(useStyleDelimOnText ? styleDelimiter.begin() : textDelimiter.begin(), useStyleDelimOnText ? styleDelimiter.end() : textDelimiter.end())), std::allocator<char>());
            mId = std::string(raw.begin(), std::search(raw.begin(), raw.end(), std::boyer_moore_searcher(styleDelimiter.begin(), styleDelimiter.end())), std::allocator<char>());



            mStyle = std::string(std::search(raw.begin(), raw.end(), std::boyer_moore_searcher(styleDelimiter.begin(), styleDelimiter.end())), raw.end(), std::allocator<char>());
            if (!mStyle.empty())
                mStyle.erase(mStyle.begin(), mStyle.begin() + 4);

            drawnUIElements.insert(mId);
            // printf("Label Text: %s\n", mText.c_str());
            // printf("ID: %s\n", mId.c_str());
            // printf("Style: %s\n", mStyle.c_str());
        }
        /// @returns all characters before '[$$]'
        const std::string& getText() {
            return mText;
        }
        /// @returns all characters before '[@@]'
        const std::string& getId() {
            return mId;
        }
        /// @returns all characters after '[@@]' if any else it just returns an empty string
        const std::string& getStyle() {
            return mStyle;
        }
    };

    static std::map<std::string, IFont*> fonts;
    static std::map<std::string, NineSlice*> nineSliceImages;

    static Vector2u windowSize;
    static Vector2i mousePos;
    static bool uiActive = false;

    static bool mouseDownLastFrame = false;
    static bool mouseDown = false;


    // static Style style;
    static simss::StyleSheet style;
    static struct {
      private:
        std::deque<LayoutRect> mSubRectStack;

      public:
        Vector2f getOffset() {
            Vector2f size = (Vector2f)priv::windowSize();
            Vector2f position = Vector2f::ZERO;
            for (size_t i = 0; i < mSubRectStack.size(); i++) {
                position += mSubRectStack[i].position.getPixels(size);
                size = mSubRectStack[i].size.getPixels(size);
            }
            return position;
        }
        Vector2f getSize() {
            Vector2f size = (Vector2f)priv::windowSize();
            for (size_t i = 0; i < mSubRectStack.size(); i++) {
                size = mSubRectStack[i].size.getPixels(size);
            }
            return size;
        }
        size_t pushSubRect(const Layout2D& position, const Layout2D& size) {
            mSubRectStack.push_back(LayoutRect {position.x, position.y, size.x, size.y});
            return mSubRectStack.size() - 1;
        }
        void popSubRect() {
            mSubRectStack.pop_back();
        }
        size_t size() const {
            return mSubRectStack.size();
        }
    } subRects;



    static std::vector<DrawCall> cachedDrawCalls; // All elements to be drawn this frame

    static std::function<GetWindowSizeCallback> windowSizeFn;
    static std::function<GetMousePositionCallback> mousePosFn;
    static std::function<IsMousePressedCallback> mousePressedFn;

#pragma region Style
    inline FEdges getBorder(const std::string& style, float _default) {
        FEdges result;
        float border = gauil::style.getValue(style, "border"s).asNumber(_default);
        result.top = gauil::style.getValue(style, "border_top"s).asNumber(border);
        result.bottom = gauil::style.getValue(style, "border_bottom"s).asNumber(border);
        result.left = gauil::style.getValue(style, "border_left"s).asNumber(border);
        result.right = gauil::style.getValue(style, "border_right"s).asNumber(border);

        return result;
    }

    inline FEdges getPadding(const std::string& style, float _default) {
        FEdges result;
        float padding = gauil::style.getValue(style, "padding"s).asNumber(_default);
        result.top = gauil::style.getValue(style, "padding_top"s).asNumber(padding);
        result.bottom = gauil::style.getValue(style, "padding_bottom"s).asNumber(padding);
        result.left = gauil::style.getValue(style, "padding_left"s).asNumber(padding);
        result.right = gauil::style.getValue(style, "padding_right"s).asNumber(padding);

        return result;
    }

    inline FCorners getBorderRadius(const std::string& style, float _default) {
        FCorners result;
        float radius = gauil::style.getValue(style, "radius"s).asNumber(_default);
        result.topLeft = gauil::style.getValue(style, "radius_top_left"s).asNumber(radius);
        result.topRight = gauil::style.getValue(style, "radius_top_right"s).asNumber(radius);
        result.bottomLeft = gauil::style.getValue(style, "radius_bottom_left"s).asNumber(radius);
        result.bottomRight = gauil::style.getValue(style, "radius_bottom_right"s).asNumber(radius);

        return result;
    }

    inline Style::Label getLabelStyle(const std::string& style) {
        std::string fullStyle = style + (style.empty() ? "" : ".") + "label";
        Style::Label label;
        label.padding = getPadding(fullStyle, Style::DEFAULT_PADDING);
        label.color = gauil::style.getValue(fullStyle, "text_color").asColor(Style::DEFAULT_TEXT_COLOR.array);
        label.font = gauil::style.getValue(fullStyle, "font").asString();
        std::string hzAlignment = gauil::style.getValue(fullStyle, "horizontal_alignment").asString("center");
        label.horizontalAlignment = hzAlignment == "left" ? HorizontalAlignment::Left : hzAlignment == "right" ? HorizontalAlignment::Right
                                                                                                               : HorizontalAlignment::Center;

        std::string vtAlignment = gauil::style.getValue(fullStyle, "vertical_alignment").asString("center");
        label.verticalAlignment = vtAlignment == "top" ? VerticalAlignment::Top : vtAlignment == "bottom" ? VerticalAlignment::Bottom
                                                                                                          : VerticalAlignment::Center;
        return label;
    }
    inline Style::Panel getPanelStyle(const std::string& style, const std::string& state, const std::string& alias = "panel") {
        std::string fullStyle = style + (style.empty() ? "" : ".") + alias+ state;
        Style::Panel panel;
        panel.backgroundColor = gauil::style.getValue(fullStyle, "background_color").asColor(DARK_MODE_BACKGROUND.array);
        panel.borderColor = gauil::style.getValue(fullStyle, "border_color").asColor(DARK_MODE_BORDER.array);

        panel.border = getBorder(fullStyle, Style::DEFAULT_BORDER_WIDTH);
        panel.padding = getPadding(fullStyle, Style::DEFAULT_PADDING);
        panel.borderRadius = getBorderRadius(fullStyle, Style::DEFAULT_BORDER_RADIUS);

        return panel;
    }
    inline Style::Button getButtonStyle(const std::string& style, const std::string& state) {
        std::string fullStyle = style + (style.empty() ? "" : ".") + "button" + state;
        Style::Button button;
        button.backgroundColor = gauil::style.getValue(fullStyle, "background_color").asColor(DARK_MODE_BACKGROUND.array);
        button.borderColor = gauil::style.getValue(fullStyle, "border_color").asColor(DARK_MODE_BORDER.array);

        button.border = getBorder(fullStyle, Style::DEFAULT_BORDER_WIDTH);
        button.padding = getPadding(fullStyle, Style::DEFAULT_PADDING);
        button.borderRadius = getBorderRadius(fullStyle, Style::DEFAULT_BORDER_RADIUS);

        button.label = getLabelStyle(fullStyle);
        return button;
    }
    inline Style::CheckBox getCheckBoxStyle(const std::string& style, const std::string& state) {
        std::string fullStyle = style + (style.empty() ? "" : ".") + "check_box" + state;
        Style::CheckBox checkBox;
        checkBox.backgroundColor = gauil::style.getValue(fullStyle, "background_color").asColor(DARK_MODE_BACKGROUND.array);
        checkBox.borderColor = gauil::style.getValue(fullStyle, "border_color").asColor(DARK_MODE_BORDER.array);

        checkBox.border = getBorder(fullStyle, Style::DEFAULT_BORDER_WIDTH);
        checkBox.padding = getPadding(fullStyle, Style::DEFAULT_PADDING);
        checkBox.borderRadius = getBorderRadius(fullStyle, Style::DEFAULT_BORDER_RADIUS);

        fullStyle = style + (style.empty() ? "" : ".") + "check" + state;

        checkBox.checkMark.backgroundColor = gauil::style.getValue(fullStyle, "background_color").asColor(Style::DEFAULT_CHECK_BACKGROUND_COLOR.array);
        checkBox.checkMark.borderColor = gauil::style.getValue(fullStyle, "border_color").asColor(Style::DEFAULT_CHECK_BORDER_COLOR.array);

        checkBox.checkMark.border = getBorder(fullStyle, Style::DEFAULT_BORDER_WIDTH);
        checkBox.checkMark.padding = getPadding(fullStyle, Style::DEFAULT_PADDING);
        checkBox.checkMark.borderRadius = getBorderRadius(fullStyle, Style::DEFAULT_BORDER_RADIUS);

        return checkBox;
    }
    inline Style::Slider getSliderStyle(const std::string& style, const std::string& state) {
        std::string fullStyle = style + (style.empty() ? "" : ".") + "slider" + state;
        Style::Slider slider;
        slider.backgroundColor = gauil::style.getValue(fullStyle, "background_color").asColor(DARK_MODE_BACKGROUND.array);
        slider.borderColor = gauil::style.getValue(fullStyle, "border_color").asColor(DARK_MODE_BORDER.array);

        slider.border = getBorder(fullStyle, Style::DEFAULT_BORDER_WIDTH);
        slider.padding = getPadding(fullStyle, Style::DEFAULT_PADDING);
        slider.borderRadius = getBorderRadius(fullStyle, Style::DEFAULT_BORDER_RADIUS);

        fullStyle = style + (style.empty() ? "" : ".") + "slider_handle" + state;

        slider.handle.backgroundColor = gauil::style.getValue(fullStyle, "background_color").asColor(Style::DEFAULT_CHECK_BACKGROUND_COLOR.array);
        slider.handle.borderColor = gauil::style.getValue(fullStyle, "border_color").asColor(Style::DEFAULT_CHECK_BORDER_COLOR.array);

        slider.handle.border = getBorder(fullStyle, Style::DEFAULT_BORDER_WIDTH);
        slider.handle.padding = getPadding(fullStyle, Style::DEFAULT_PADDING);
        slider.handle.borderRadius = getBorderRadius(fullStyle, Style::DEFAULT_BORDER_RADIUS);

        return slider;
    }
    Style::Window getWindowStyle(const std::string& style) {
        std::string fullStyle = style + (style.empty() ? "" : ".") + "window";
        Style::Window window;
        window.backgroundColor = gauil::style.getValue(fullStyle, "background_color").asColor(DARK_MODE_BACKGROUND.array);
        window.borderColor = gauil::style.getValue(fullStyle, "border_color").asColor(DARK_MODE_BORDER.array);

        window.border = getBorder(fullStyle, Style::DEFAULT_BORDER_WIDTH);
        window.padding = getPadding(fullStyle, Style::DEFAULT_PADDING);
        window.borderRadius = getBorderRadius(fullStyle, Style::DEFAULT_BORDER_RADIUS);

        window.titleBarHeight = ::gauil::style.getValue(style, "title_bar_height", Style::DEFAULT_TITLE_BAR_HEIGHT).asNumber();
       
        window.title = getLabelStyle(style);
        window.titleBar = getPanelStyle(style, "");
       
        return window;
    }
#pragma endregion
    inline Vector2f getCurrentOffset() {
        return subRects.getOffset();
    }
    inline Vector2f layoutToSize(const Layout2D& layout) {
        return layout.getPixels(subRects.getSize());
    }

    /// @brief Same as layout to size but adds subRectOffset to the result
    /// @param layout
    /// @note Only use this function in non raw UI elem functions
    /// @return
    inline Vector2f layoutToPosition(const Layout2D& layout) {
        return layout.getPixels(subRects.getSize()) + subRects.getOffset();
    }
    /// @brief Returns a style state depending on the mouses position
    /// @returns "|active" if mouse is down on the specified, "|hover" if mouse is hovering or nothing
    std::string getStateFromMouse(const Layout2D& position, const Layout2D& size) {
        FRect rect(layoutToPosition(position), layoutToSize(size));
        if (rect.contains(static_cast<Vector2f>(mousePos)))
            return priv::isMouseHeld() ? "|active" : "|hover";
        return "";
    }
#pragma region Drawing
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



    void queueText(const Vector2f& pos, uint fontSize, float scale, const std::string& text, IFont* font, const Color& color) {
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
#pragma endregion
    void init() {
        GAUIL_ASSERT(getDefaultFont(), "No default font has been chosen");

        GAUIL_ASSERT(getWindowSizeFn(), "No window size function has been set");
        GAUIL_ASSERT(getMousePositionFn(), "No mouse position function has been set");
        GAUIL_ASSERT(getMouseDownFn(), "No mouse pressed function has been set");

        GAUIL_ASSERT(getTriangleListDrawFn(), "No triangle list draw function has been set");
        GAUIL_ASSERT(getTriangleFanDrawFn(), "No triangle fan draw function has been set");
        GAUIL_ASSERT(getTriangleStripDrawFn(), "No triangle strip draw function has been set");

        GAUIL_ASSERT(getRectDrawFn(), "No rect draw function has been set");


        GAUIL_ASSERT(getLoadTextureFn(), "No load texture function has been set");
        GAUIL_ASSERT(getDrawTextureFn(), "No draw texture function has been set");


        subRects.pushSubRect({}, (Vector2f)windowSizeFn());

        style = simss::StyleSheet();
    }
    void cleanup() {
    }


    void pushSubRect(const Layout2D& position, const Layout2D& size) {
        GAUIL_ASSERT(uiActive, "Can only push sub rects between update and draw");
        subRects.pushSubRect(position, size);
    }
    void popSubRect() {
        GAUIL_ASSERT(uiActive, "Can only pop sub rects between update and draw");
        GAUIL_ASSERT(subRects.size() > 1, "Sub rect stack underflow");
        subRects.popSubRect();
    }
#pragma region Loaders
    bool loadStyle(const std::string& styleSheet) {
        auto newStyle = simss::loadFromString(styleSheet);
        if (newStyle) {
            gauil::style = newStyle.getValue();
            return true;
        }

        for (auto err : newStyle.getErrors())
            printf("%s\n", err.c_str());
        return false;
    }
    bool loadFont(const std::string& file, const std::string& referenceName) {
        GAUIL_ASSERT(getLoadTextureFn(), "No texture loader function has been set");
        std::ifstream stream(file, std::ios::binary);
        if (!stream)
            return false;
        std::vector<uint8_t> buf(std::filesystem::file_size(file));
        stream.read((char*)buf.data(), buf.size());
        stream.close();
        return loadFont(buf.data(), buf.size(), referenceName);
    }
    bool loadFont(const uint8_t* data, size_t length, const std::string& referenceName) {
        GAUIL_ASSERT(getLoadTextureFn(), "No texture loader function has been set");
        IFont* font = loadFont(data, length);
        if (font) {
            fonts[referenceName] = font;
            return true;
        }
        return false;
    }
    bool loadNineSlice(const uint8_t* data, size_t length, const URect& sliceRect, const std::string& referenceName) {
        GAUIL_ASSERT(getLoadTextureFn(), "No texture loader function has been set");
        NineSlice* nineSlice = loadNineSlice(data, length, sliceRect);
        if (nineSlice) {
            nineSliceImages[referenceName] = nineSlice;
            return true;
        }
        return false;
    }
#pragma endregion
#pragma region UIElements
    void labelRaw(const std::string& text, const Vector2f& position, const Vector2f& size, const Style::Label& style) {
        IFont* font = fonts.count(style.font) ? fonts.at(style.font) : getDefaultFont();
        const Vector2f textBox = priv::measureText(text, size.min(), font);
        const FRect textRect(position + style.padding.getOffset(), size - style.padding.getOffset() - style.padding.getBounds());
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


        // TODO: Add debug draw option for layout
        // queueRect(textRect, {}, {}, FALLBACK, {});
        queueText(textRect.position + textBoxAlignment, size.min(), scale, text, font, style.color); // Draw text
    }
    void label(const std::string& text, const Layout2D& position, const Layout2D& size) {
        LabelData data(text);
        drawnUIElements.insert(data.getId());
        labelRaw(data.getText(), layoutToPosition(position), layoutToSize(size), getLabelStyle(data.getStyle()));
    }

    void panelRaw(const Vector2f& position, const Vector2f& size, const Style::Panel& style) {
        queueRect(FRect(position, size), style.borderRadius, style.border, style.backgroundColor, style.borderColor);
    }
    void panel(const Layout2D& position, const Layout2D& size, const std::string& style) {
        std::string state = getStateFromMouse(position, size);
        panelRaw(layoutToPosition(position), layoutToSize(size), getPanelStyle(style, state));
    }
    bool buttonRaw(const std::string& text, const Vector2f& position, const Vector2f& size, const Style::Button& buttonStyle) {

        queueRect(FRect(position, size), buttonStyle.borderRadius, buttonStyle.border, buttonStyle.backgroundColor, buttonStyle.borderColor);

        Vector2f backgroundOffset = Vector2f(buttonStyle.border.left, buttonStyle.border.top);
        Vector2f backgroundBounds = Vector2f(buttonStyle.border.right, buttonStyle.border.bottom);

        Vector2f textOffset = Vector2f(buttonStyle.padding.left, buttonStyle.padding.top);
        Vector2f textBounds = Vector2f(buttonStyle.padding.right, buttonStyle.padding.bottom);

        const FRect textRect = FRect(position + backgroundOffset + textOffset, size - backgroundOffset - backgroundBounds - textOffset - textBounds);
        const Vector2f textBox = priv::measureText(text, textRect.size.min(), getDefaultFont());
        if (textBox.min() > 0) {
            labelRaw(text, textRect.position, textRect.size, buttonStyle.label);
        }
        FRect rect(position, size);
        return rect.contains((Vector2f)mousePos) && priv::isMouseUp();
    }
    bool button(const std::string& text, const Layout2D& position, const Layout2D& size) {
        LabelData data(text);
        drawnUIElements.insert(data.getId());
        Vector2f pixPos = layoutToPosition(position);
        Vector2f pixSize = layoutToSize(size);
        std::string state = getStateFromMouse(position, size);

        return buttonRaw(data.getText(), pixPos, pixSize, getButtonStyle(data.getStyle(), state));
    }


    bool checkBoxRaw(bool* _bool, const std::string& style, const Vector2f& position, const Vector2f& size, const Style::CheckBox& checkBox) {
        queueRect(FRect {position, size}, checkBox.borderRadius, checkBox.border, checkBox.backgroundColor, checkBox.borderColor);
        Vector2f backgroundOffset = Vector2f(checkBox.border.left, checkBox.border.top);
        Vector2f backgroundBounds = Vector2f(checkBox.border.right, checkBox.border.bottom);

        Vector2f textOffset = Vector2f(checkBox.padding.left, checkBox.padding.top);
        Vector2f textBounds = Vector2f(checkBox.padding.right, checkBox.padding.bottom);

        Vector2f totalOffset = backgroundOffset + textOffset;
        Vector2f totalBounds = backgroundBounds + textBounds;

        bool clicked = priv::isMouseUp() && FRect(position, size).contains((Vector2f)mousePos);
        if (clicked)
            *_bool = !*_bool;
        if (*_bool)
            queueRect(FRect {position + totalOffset, size - totalOffset - totalBounds}, checkBox.checkMark.borderRadius, checkBox.checkMark.border, checkBox.checkMark.backgroundColor, checkBox.checkMark.borderColor);
        return clicked;
    }
    bool checkBox(bool* _bool, const std::string& style, const Layout2D& position, const Layout2D& size) {
        auto pixPos = layoutToPosition(position);
        auto pixSize = layoutToSize(size);
        std::string state = getStateFromMouse(position, size);
        return checkBoxRaw(_bool, style, pixPos, pixSize, getCheckBoxStyle(style, state));
    }

    void sliderRaw(float* value, float min, float max, const Vector2f& position, const Vector2f& size, const Style::Slider& style) {
        GAUIL_ASSERT(max != min, "Slider max and min cannot be equal");
        queueRect(FRect {position, size}, style.borderRadius, style.border, style.backgroundColor, style.borderColor);



        Vector2f handleSize = Vector2f(size.y, size.y) - style.padding.getOffset() - style.padding.getBounds() - style.border.getOffset() - style.border.getBounds();
        Vector2f handlePosition = position + style.padding.getOffset() + style.border.getOffset() + Vector2f(*value / (max - min) * (size.x - style.padding.getOffset().x - style.padding.getBounds().x - style.border.getOffset().x - style.border.getBounds().x - handleSize.x), 0);
        if ((priv::isMouseDown() || priv::isMouseHeld()) && FRect {position, size}.contains((Vector2f)mousePos)) {
            FRect rect(position + style.padding.getOffset(), size - style.padding.getOffset() - style.padding.getBounds());
            if (rect.contains((Vector2f)mousePos)) {
                float relativeX = (mousePos.x - handleSize.x * 0.5 - rect.position.x - style.padding.getOffset().x - style.border.getOffset().x) / (rect.size.x - style.padding.getOffset().x - style.padding.getBounds().x - style.border.getOffset().x - style.border.getBounds().x - handleSize.x);
                *value = min + (relativeX * (max - min));
                *value = std::clamp(*value, min, max);
            }
        }
        queueRect(FRect {handlePosition, handleSize}, style.handle.borderRadius, style.handle.border, style.handle.backgroundColor, style.handle.borderColor);
    }
    void slider(float* value, const std::string& style, float min, float max, const Layout2D& position, const Layout2D& size) {
        auto pixPos = layoutToPosition(position);
        auto pixSize = layoutToSize(size);
        std::string state = [&] {
            FRect rect;
            rect.position = pixPos + Vector2f(lerp(min, max, *value) * (pixSize.x - pixSize.y), 0);
            rect.size = {pixSize.y, pixSize.y};
            if (rect.contains(static_cast<Vector2f>(mousePos)))
                return priv::isMouseHeld() ? "|active" : "|hover";
            return "";
        }();
        sliderRaw(value, min, max, pixPos, pixSize, getSliderStyle(style, state));
    }
#pragma endregion


    void update() {
        GAUIL_ASSERT(!uiActive, "Each call to gauil::update must have a corresponding call to gauil::draw");
        uiActive = true;
        cachedDrawCalls.clear();

        drawnUIElements.clear();


        mouseDown = mousePressedFn();
        windowSize = windowSizeFn();
        mousePos = mousePosFn();
        subRects.popSubRect();
        subRects.pushSubRect({}, (Vector2f)windowSize);
    }

    void draw() {
        GAUIL_ASSERT(uiActive, "gauil::update was not called before gauil::draw");
        GAUIL_ASSERT(subRects.size() == 1, "The child rect stack was not cleaned properly");
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