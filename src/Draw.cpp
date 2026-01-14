#include <GaUIL/Draw.hpp>
#include <GaUIL/Assert.hpp>
#include <GaUIL/Corners.hpp>
#include <GaUIL/Math.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <string>

namespace gauil {
    static std::function<TriangleDrawCallback> triangleListDrawFn = nullptr;
    static std::function<TriangleDrawCallback> triangleFanDrawFn = nullptr;
    static std::function<TriangleDrawCallback> triangleStripDrawFn = nullptr;

    static std::function<RectDrawCallback> rectDrawFn = nullptr;

    static std::function<TextDrawCallback> textDrawFn = nullptr;

    static std::function<MeasureTextCallback> measureTextFn = nullptr;

    static std::any drawUserData;
    static std::any defaultFont;
    static std::map<std::string, std::any> fonts;

    void setTriangleListDrawFn(const std::function<TriangleDrawCallback>& fn) {
        triangleListDrawFn = fn;
    }
    std::function<TriangleDrawCallback> getTriangleListDrawFn() {
        return triangleListDrawFn;
    }

    void setTriangleFanDrawFn(const std::function<TriangleDrawCallback>& fn) {
        triangleFanDrawFn = fn;
    }
    std::function<TriangleDrawCallback> getTriangleFanDrawFn() {
        return triangleFanDrawFn;
    }

    void setTriangleStripDrawFn(const std::function<TriangleDrawCallback>& fn) {
        triangleStripDrawFn = fn;
    }
    std::function<TriangleDrawCallback> getTriangleStripDrawFn() {
        return triangleStripDrawFn;
    }


    void setRectDrawFn(const std::function<RectDrawCallback>& fn) {
        rectDrawFn = fn;
    }
    std::function<RectDrawCallback> getRectDrawFn() {
        return rectDrawFn;
    }

    void setTextDrawFn(const std::function<TextDrawCallback>& fn) {
        textDrawFn = fn;
    }
    std::function<TextDrawCallback> getTextDrawFn() {
        return textDrawFn;
    }

    void setMeasureTextFn(const std::function<MeasureTextCallback>& fn) {
        measureTextFn = fn;
    }
    std::function<MeasureTextCallback> getMeasureTextFn() {
        return measureTextFn;
    }


    void setDrawUserData(const std::any& userData) {
        drawUserData = userData;
    }
    std::any getDrawUserData() {
        return drawUserData;
    }

    void setDefaultFont(const std::any& font) {
        defaultFont = font;
    }
    std::any getDefaultFont() {
        return defaultFont;
    }

    namespace priv {
        void drawTriangleList(const Vertex* vertices, size_t vertexCount, const Color& color) {
            GAUIL_ASSERT(triangleListDrawFn, "No triangle list draw function declared");
            triangleListDrawFn(vertices, vertexCount, color, getDrawUserData());
        }
        void drawTriangleFan(const Vertex* vertices, size_t vertexCount, const Color& color) {
            GAUIL_ASSERT(triangleFanDrawFn, "No triangle fan draw function declared");
            triangleFanDrawFn(vertices, vertexCount, color, getDrawUserData());
        }
        void drawTriangleStrip(const Vertex* vertices, size_t vertexCount, const Color& color) {
            GAUIL_ASSERT(triangleStripDrawFn, "No triangle strip draw function declared");
            triangleStripDrawFn(vertices, vertexCount, color, getDrawUserData());
        }

        void drawRect(const FRect& rect, const Color& color) {
            GAUIL_ASSERT(rectDrawFn, "No rect draw function declared");
            rectDrawFn(rect, color, getDrawUserData());
        }
        void drawText(const Vector2f& pos, uint fontSize, float scale, const std::string& text, const Color& color, const std::any& font) {
            GAUIL_ASSERT(textDrawFn, "No text draw function declared");
            textDrawFn(pos, fontSize, scale, text, color, font, getDrawUserData());
        }

        Vector2f measureText(const std::string& string, uint fontSize, const std::any& font) {
            GAUIL_ASSERT(measureTextFn, "No measure text function declared");
            return measureTextFn(string, fontSize, font, getDrawUserData());
        }
        // TODO: Borders
        void roundRect(const FRect& rect, const FCorners& radius, const FEdges& borders, const Color& backgroundColor, const Color& borderColor) {
            if (radius.isZero()) {
                drawRect(rect, borderColor);
                drawRect({ rect.position + Vector2f(borders.left, borders.top), rect.size - Vector2f(borders.left + borders.right, borders.top + borders.bottom) }, backgroundColor);
                return;
            }


            static constexpr int DETAIL = 16; // Amount of vertices on each corner
            std::vector<Vertex> vertices(DETAIL * 4 + 2);
            std::vector<Vertex> borderVertices(DETAIL * 4 + 2);
            vertices[0].position = rect.getCenter();
            borderVertices[0].position = rect.getCenter() + borders.getOffset() - borders.getBounds();
            for (size_t i = 0; i < DETAIL * 4 + 1; i++) {
                Vector2f quadrant;
                int quad = i / DETAIL;
                float cornerRadius;
                switch (quad) {
                case 0:
                case 4:
                    cornerRadius = radius.bottomRight;
                    break;
                case 1:
                    cornerRadius = radius.bottomLeft;
                    break;
                case 2:
                    cornerRadius = radius.topLeft;
                    break;
                case 3:
                    cornerRadius = radius.topRight;
                    break;
                };
                const float angle = float(i) / float(DETAIL * 4) * M_PI * 2;

                quadrant = Vector2f::fromAngle(angle);
                quadrant = quadrant.ceilWithAbs();


                cornerRadius = std::max(0.f, std::min(rect.h * .5f, std::min(rect.w * .5f, cornerRadius)));
                Vertex& vertex = vertices[i + 1];
                Vertex& borderVertex = borderVertices[i + 1];

                float borderLerp = borders.angleToBorderWidth(angle);
                float border = borders.getBorderFromAngle(angle);
                FRect innerRect = rect;
                innerRect.position += borders.getOffset();
                innerRect.size -= borders.getOffset() + borders.getBounds();
                vertex.position = innerRect.getCenter() + ((innerRect.size - (Vector2f(cornerRadius, cornerRadius) ) * 2.f) / 2.f * quadrant) + Vector2f::fromAngle(angle) * cornerRadius;
                if (!borders.isZero())
                    borderVertex.position = (rect.getCenter()) + ((rect.size - (Vector2f(cornerRadius, cornerRadius)) * 2.f) / 2.f * quadrant) + Vector2f::fromAngle(angle) * cornerRadius;
            }
            // drawRect(rect, color::FALLBACK);
            if (!borders.isZero())
                drawTriangleFan(borderVertices.data(), borderVertices.size(), borderColor);
            drawTriangleFan(vertices.data(), vertices.size(), backgroundColor);


        }

    }
}