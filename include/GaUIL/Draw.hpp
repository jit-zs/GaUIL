#ifndef GAUIL_DRAW_HPP
#define GAUIL_DRAW_HPP

#include <any>
#include <functional>
#include <cstdint>
#include <string>

#include <GaUIL/Color.hpp>
#include <GaUIL/Corners.hpp>
#include <GaUIL/Edges.hpp>
#include <GaUIL/Rect.hpp>
#include <GaUIL/Vector2.hpp>


namespace gauil {
    struct Vertex {
        Vector2f position;
        Vector2f texCoords;
    };
    typedef void(TriangleDrawCallback)(const Vertex* vertices, size_t vertexCount, const Color& color, const std::any& userData);
    typedef void(RectDrawCallback)(const FRect& rect, const Color& color, const std::any& userData);
    typedef void(TextDrawCallback)(const Vector2f& pos, uint fontSize, float scale, const std::string& text, const Color& color, const std::any& font, const std::any& userData);
    typedef Vector2f(MeasureTextCallback)(const std::string& string, uint fontSize, const std::any& font, const std::any& userData);

    void setTriangleListDrawFn(const std::function<TriangleDrawCallback>& fn);
    std::function<TriangleDrawCallback> getTriangleListDrawFn();
    void setTriangleFanDrawFn(const std::function<TriangleDrawCallback>& fn);
    std::function<TriangleDrawCallback> getTriangleFanDrawFn();
    void setTriangleStripDrawFn(const std::function<TriangleDrawCallback>& fn);
    std::function<TriangleDrawCallback> getTriangleStripDrawFn();

    void setRectDrawFn(const std::function<RectDrawCallback>& fn);
    std::function<RectDrawCallback> getRectDrawFn();

    void setMeasureTextFn(const std::function<MeasureTextCallback>& fn);
    std::function<MeasureTextCallback> getMeasureTextFn();

    void setTextDrawFn(const std::function<TextDrawCallback>& fn);
    std::function<TextDrawCallback> getTextDrawFn();

    void setDrawUserData(const std::any& userData);
    std::any getDrawUserData();

    // Default font should be set before initialization
    void setDefaultFont(const std::any& font);
    std::any getDefaultFont();

    namespace priv {
        void drawTriangleList(const Vertex* vertices, size_t vertexCount, const Color& color);
        void drawTriangleFan(const Vertex* vertices, size_t vertexCount, const Color& color);
        void drawTriangleStrip(const Vertex* vertices, size_t vertexCount, const Color& color);

        void drawRect(const FRect& rect, const Color& color);
        void drawText(const Vector2f& pos, uint fontSize, float scale, const std::string& text, const Color& color, const std::any& font);

        Vector2f measureText(const std::string& string, uint fontSize, const std::any& font);

        void roundRect(const FRect& rect, const FCorners& cornerRadius, const FEdges& edges, const Color& backgroundColor, const Color& borderColor);
    }
}


#endif
