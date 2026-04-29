#include <GaUIL/Draw.hpp>
#include <GaUIL/Assert.hpp>
#include <GaUIL/Corners.hpp>
#include <GaUIL/GUI.hpp>
#include <GaUIL/Math.hpp>
#include <GaUIL/Text.hpp>

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <map>
#include <string>

namespace gauil {
    static std::function<TriangleDrawCallback> triangleListDrawFn = nullptr;
    static std::function<TriangleDrawCallback> triangleFanDrawFn = nullptr;
    static std::function<TriangleDrawCallback> triangleStripDrawFn = nullptr;

    static std::function<RectDrawCallback> rectDrawFn = nullptr;

    static std::function<LoadTextureCallback> loadTextureFn = nullptr;
    static std::function<UnloadTextureCallback> unloadTextureFn = nullptr;
    static std::function<DrawTextureCallback> drawTextureFn = nullptr;

    static std::any drawUserData;
    static IFont* defaultFont;


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


    void setLoadTextureFn(const std::function<LoadTextureCallback>& fn) {
        loadTextureFn = fn;
    }
    std::function<LoadTextureCallback> getLoadTextureFn() {
        return loadTextureFn;
    }
    void setUnloadTextureFn(const std::function<UnloadTextureCallback>& fn) {
        unloadTextureFn = fn;
    }
    std::function<UnloadTextureCallback> getUnloadTextureFn() {
        return unloadTextureFn;
    }
    void setDrawTextureFn(const std::function<DrawTextureCallback>& fn) {
        drawTextureFn = fn;
    }
    std::function<DrawTextureCallback> getDrawTextureFn() {
        return drawTextureFn;
    }

    void setDrawUserData(const std::any& userData) {
        drawUserData = userData;
    }
    std::any getDrawUserData() {
        return drawUserData;
    }

    void setDefaultFont(const std::string& file) {
        GAUIL_ASSERT(getLoadTextureFn(), "No font file loader has been set");

        size_t length = std::filesystem::file_size(file);
        std::vector<uint8_t> buf(length);
        std::ifstream stream(file, std::ios::binary);
        stream.read((char*)buf.data(), length);
        stream.close();

        IFont* result = loadFont(buf.data(), length);
        GAUIL_ASSERT(result, "Failed to load default font");
        defaultFont = result;
    }
    void setDefaultFont(const uint8_t* data, size_t len) {
        GAUIL_ASSERT(getLoadTextureFn(), "No font memory loader has been set");
        IFont* result = loadFont(data, len);
        GAUIL_ASSERT(result, "Failed to load default font");
        defaultFont = result;
    }
    IFont* getDefaultFont() {
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
        void drawText(const Vector2f& pos, uint fontSize, float scale, const std::string& text, const Color& color, IFont* font) {
            GAUIL_ASSERT(getDrawTextureFn, "No text draw function declared");
            font->draw(pos, text, fontSize, scale, color);
        }

        Vector2f measureText(const std::string& string, uint fontSize, IFont* font) {
            GAUIL_ASSERT(getLoadTextureFn, "No measure text function declared");
            return font->measureText(string, fontSize);
        }
        std::optional<std::any> loadTexture(const uint8_t* pixels, int width, int height) {
            GAUIL_ASSERT(loadTextureFn, "No load texture function has been set");
            return loadTextureFn(pixels, width, height);
        }
        void unloadTexture(const std::any& texture) {
            if (unloadTextureFn)
                unloadTextureFn(texture);
        }
        void drawTexture(const std::any& texture, const gauil::Vector2f& position, const gauil::Vector2f& scale, const Color& color) {
            GAUIL_ASSERT(drawTextureFn, "No draw texture function has been set");
            drawTextureFn(texture, position, scale, color, getDrawUserData());
        }
        // TODO: Borders
        void roundRect(const FRect& rect, const FCorners& radius, const FEdges& borders, const Color& backgroundColor, const Color& borderColor) {

            static constexpr int DETAIL = 8; // Amount of vertices on each corner
            std::vector<Vertex> vertices(DETAIL * 4 + 3);
            std::vector<Vertex> borderVertices;
            vertices[0].position = rect.getCenter();
            for (size_t i = 0; i < DETAIL * 4 + 2; i++) {
                Vector2f quadrant;
                int quad = i / DETAIL;
                float cornerRadius = radius.topRight;
                switch (quad) {
                case 0:
                case 4:
                    cornerRadius = radius.bottomRight;
                    quadrant = { 1, 1 };
                    break;
                case 1:
                    cornerRadius = radius.bottomLeft;
                    quadrant = { -1, 1 };
                    break;
                case 2:
                    cornerRadius = radius.topLeft;
                    quadrant = { -1, -1 };
                    break;
                case 3:
                    cornerRadius = radius.topRight;
                    quadrant = { 1, -1 };
                    break;
                };
                const float angle = (float(i) / float(DETAIL * 4) * M_PI * 2);

                cornerRadius = std::max(0.f, std::min(rect.h * .5f, std::min(rect.w * .5f, cornerRadius)));
                Vertex& vertex = vertices[i + 1];


                FRect innerRect = rect;
                innerRect.position += borders.getOffset();
                innerRect.size -= borders.getOffset() + borders.getBounds();
                vertex.position = innerRect.getCenter() + ((innerRect.size - (Vector2f(cornerRadius, cornerRadius)) * 2.f) / 2.f * quadrant) + Vector2f::fromAngle(angle) * cornerRadius;
                if (!borders.isZero()) {
                    borderVertices.push_back(vertex);
                    borderVertices.push_back({ .position = (rect.getCenter()) + ((rect.size - (Vector2f(cornerRadius, cornerRadius)) * 2.f) / 2.f * quadrant) + Vector2f::fromAngle(angle) * cornerRadius });
                }

            }
            // For debugging purposes
            // drawRect(rect, FALLBACK);

            auto v = vertices[0];
            vertices.erase(vertices.begin());
            std::reverse(vertices.begin(), vertices.end());
            vertices.insert(vertices.begin(), v);

            v = borderVertices[0];
            borderVertices.erase(borderVertices.begin());
            std::reverse(borderVertices.begin(), borderVertices.end());
            borderVertices.insert(borderVertices.begin(), v);

            if (!borders.isZero())
                drawTriangleStrip(borderVertices.data(), borderVertices.size(), borderColor);
            drawTriangleFan(vertices.data(), vertices.size(), backgroundColor);


        }

    }
}