#define GAUIL_C_HEADER_IMPL
#include <GaUIL/GaUIL.h>
#include <GaUIL/GaUIL.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
gauil::Layout::Type toCpp(gauil_LayoutType type) {
    switch (type) {
    case GAUIL_LAYOUT_PIXELS:
        return gauil::Layout::Type::Pixels;
    case GAUIL_LAYOUT_PERCENT:
        return gauil::Layout::Type::Percent;
    case GAUIL_LAYOUT_OTHER_LAYOUT:
        return gauil::Layout::Type::OtherLayout;
    }
    return gauil::Layout::Type::Pixels;
}
gauil::Layout2D toCpp(const gauil_Layout2D& layout) {
    gauil::Layout2D result;
    result.x.value = layout.x.value;
    result.x.type = toCpp(layout.x.type);
    result.x.scaleWithOpposingAxis = layout.x.scaleWithOpposingAxis;

    result.y.value = layout.y.value;
    result.y.type = toCpp(layout.y.type);
    result.y.scaleWithOpposingAxis = layout.y.scaleWithOpposingAxis;
    return result;
}
gauil::Color toCpp(const gauil_Color& color) {
    return gauil::Color(color.r, color.g, color.b, color.a);
}
gauil_Color toC(const gauil::Color& color) {
    return { color.r, color.g, color.b, color.a };
}
gauil::FRect toCpp(const gauil_FRect& rect) {
    return { rect.x, rect.y, rect.w, rect.h };
}
gauil_FRect toC(const gauil::FRect& rect) {
    return { rect.x, rect.y, rect.w, rect.h };
}
gauil::Vector2f toCpp(const gauil_Vector2f& vec) {
    return { vec.x, vec.y };
}
gauil_Vector2f toC(const gauil::Vector2f& vec) {
    return { vec.x, vec.y };
}
gauil::Vector2i toCpp(const gauil_Vector2i& vec) {
    return { vec.x, vec.y };
}
gauil_Vector2i toC(const gauil::Vector2i& vec) {
    return { vec.x, vec.y };
}
gauil::Vector2u toCpp(const gauil_Vector2u& vec) {
    return { vec.x, vec.y };
}
gauil_Vector2u toC(const gauil::Vector2u& vec) {
    return { vec.x, vec.y };
}

std::function<gauil::TriangleDrawCallback> toCpp(gauil_TriangleDrawCallback* callback) {
    return [=](const gauil::Vertex* vertices, size_t vertexCount, const gauil::Color& color, const std::any& userData) {
        callback((const  gauil_Vertex*)vertices, vertexCount, toC(color), userData.has_value() ? std::any_cast<void*>(userData) : NULL);
        };
}
extern "C" void gauil_setTriangleListDrawFn(gauil_TriangleDrawCallback* callback) {
    gauil::setTriangleListDrawFn(toCpp(callback));
}

extern "C" void gauil_setTriangleStripDrawFn(gauil_TriangleDrawCallback* callback) {
    gauil::setTriangleStripDrawFn(toCpp(callback));
}

extern "C" void gauil_setTriangleFanDrawFn(gauil_TriangleDrawCallback* callback) {
    gauil::setTriangleFanDrawFn(toCpp(callback));
}
extern "C" void gauil_setRectDrawFn(gauil_RectDrawCallback* callback) {
    gauil::setRectDrawFn([=](const gauil::FRect& rect, const gauil::Color& color, const std::any& userData) {
        callback(toC(rect), toC(color), userData.has_value() ? std::any_cast<void*>(userData) : NULL);
        });
}


extern "C" void gauil_setWindowSizeFn(gauil_GetWindowSizeCallback* callback) {
    gauil::setWindowSizeFn([=]() { return toCpp(callback());});
}

extern "C" void gauil_setMousePositionFn(gauil_GetMousePositionCallback* callback) {
    gauil::setMousePositionFn([=]() { return toCpp(callback());});
}
extern "C" void gauil_setMouseDownFn(gauil_IsMousePressedCallback* callback) {
    gauil::setMouseDownFn([=]() { return callback();});
}
extern "C" void gauil_setLoadTextureFn(gauil_LoadTextureCallback* callback) {
    gauil::setLoadTextureFn([=](const uint8_t* pixels, int width, int height) -> std::optional<std::any> {
        void* result = callback(pixels, width, height);
        if (result)
            return result;
        else
            return std::nullopt;
        });
}
extern "C" void gauil_setUnloadTextureFn(gauil_UnloadTextureCallback* callback) {
    gauil::setUnloadTextureFn([=](const std::any& texture) {
            callback(std::any_cast<void*>(texture));
        });
}
extern "C" void gauil_setDrawTextureFn(gauil_DrawTextureCallback* callback) {
    gauil::setDrawTextureFn([=](const std::any& texture, const gauil::Vector2f& position, const gauil::Vector2f& scale, const gauil::Color& color, const std::any& userData) {
            callback(std::any_cast<void*>(texture), toC(position), toC(scale), toC(color), userData.has_value() ? std::any_cast<void*>(userData) : NULL);
        });
}

extern "C" void gauil_setDrawUserData(void* userData) {
    gauil::setDrawUserData(userData);
}
extern "C" void gauil_setDefaultFontFromFile(const char* file) {
    gauil::setDefaultFont(file);
}
extern "C" void gauil_setDefaultFontFromMemory(const uint8_t* data, size_t size) {
    gauil::setDefaultFont(data, size);
}
extern "C" void gauil_init() {
    gauil::init();
    GAUIL_ASSERT(gauil::getUnloadTextureFn(), "The unload texture function is mandatory for the C API")
}
extern "C" void gauil_cleanup() {
    gauil::cleanup();
}
extern "C" void gauil_update() {
    gauil::update();
}
extern "C" void gauil_draw() {
    gauil::draw();
}
extern "C" bool gauil_loadStyle(const char* string) {
    return gauil::loadStyle(string);
}
extern "C" bool gauil_loadFontFromFile(const char* file, const char* referenceName) {
    std::ifstream stream(file, std::ios::binary);
    if (!stream)
        return false;
        size_t filesize = std::filesystem::file_size(file);
        std::vector<uint8_t> buf(filesize);
        stream.read((char*)buf.data(), filesize);
        stream.close();
        
    return gauil::loadFont(buf.data(), buf.size(), referenceName);
}
extern "C" bool gauil_loadFontFromMemory(const uint8_t* data, size_t length, const char* referenceName) {
    return gauil::loadFont(data, length, referenceName);
}
extern "C" gauil_Layout gauil_scaleWithOpposingAxis(gauil_Layout layout) {
    layout.scaleWithOpposingAxis = true;
    return layout;
}
extern "C" void gauil_pushSubRect(gauil_Layout2D position, gauil_Layout2D size) {
    gauil::pushSubRect(toCpp(position), toCpp(size));
}
extern "C" void gauil_popSubRect() {
    gauil::popSubRect();
}
extern "C" bool gauil_button(const char* text, gauil_Layout2D position, gauil_Layout2D size) {
    return gauil::button(text, toCpp(position), toCpp(size));
}
extern "C" void gauil_panel(gauil_Layout2D position, gauil_Layout2D size, const char* style) {
    gauil::panel(toCpp(position), toCpp(size), style);
}
extern "C" void gauil_label(const char* text, gauil_Layout2D position, gauil_Layout2D size) {
    gauil::label(text, toCpp(position), toCpp(size));
}
extern "C" void gauil_checkBox(bool* value, const char* style, gauil_Layout2D position, gauil_Layout2D size) {
    gauil::checkBox(value, style, toCpp(position), toCpp(size));
}
extern "C" void gauil_slider(float* value, const char* style, float min, float max, gauil_Layout2D position, gauil_Layout2D size) {
    gauil::slider(value, style, min, max, toCpp(position), toCpp(size));
}