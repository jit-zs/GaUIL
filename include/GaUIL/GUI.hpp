#ifndef GAUIL_GUI_HPP
#define GAUIL_GUI_HPP

#include <GaUIL/Layout.hpp>
#include <GaUIL/Rect.hpp>
#include <GaUIL/Style.hpp>

#include <any>
#include <bit>
#include <bitset>
#include <functional>
#include <optional>
#include <string>

namespace gauil {

    typedef Vector2u(GetWindowSizeCallback)();
    /// @brief Callback to get the mouse position
    /// @note Make sure the position is relative to the window/surface you're drawing the UI to
    typedef Vector2i(GetMousePositionCallback)();
    /// @brief Callback to check if the mouse is pressed
    /// @note Use functions like sf::Mouse::isButtonPressed or SDL_GetMouseState to check the mouse. DO NOT USE EVENTS
    /// @note Make sure that this function returns false if the window is not focused
    typedef bool(IsMousePressedCallback)();

    /// @brief Callback to load a font from file
    /// @note The result of this function will be passed directly to the text draw and measure functions/callbacks   
    /// @returns A font object constructed from the given file path or an empty std::any on failure
    typedef std::any(LoadFontFromFileCallback)(const std::string& file);

    /// @brief Callback to load a font from memory
    /// @note The result of this function will be passed directly to the text draw and measure functions/callbacks   
    /// @returns A font object constructed from the given memory block or an empty std::any on failure
    typedef std::any(LoadFontFromMemoryCallback)(const void* data, size_t length);


    /// @brief Initializes the UI and makes sure that all callbacks have been set
    /// @note Only one of the font loaders callbacks need to be set
    void init();
    void cleanup();

    /// @brief Loads a font from file and adds it to the font list
    /// @returns True on success
    bool loadFont(const std::string& file, const std::string& name);
    /// @brief Loads a font from memory and adds it to the font list
    /// @returns True on success
    bool loadFont(const void* data, size_t length, const std::string& name);

    /// @brief Loads Simple Style Sheet from file
    /// @returns true on success else false
    bool loadStyle(const std::string& file);

    /// @brief Prepares the GUI system for a new frame
    void update();

    /// @brief Draws all cached UI elements to the screen
    void draw();

    /// @brief Draws a label
    /// @param position 
    /// @param size 
    /// @param text 
    void label(const std::string& text, const Layout2D& position, const  Layout2D& size);


    /// @brief Draws a button
    /// @param position 
    /// @param size 
    /// @param text 
    /// @return Whether the button was released over the button
    bool button(const std::string& text, const Layout2D& position, const Layout2D& size);


    
    /// @brief 
    /// @param _bool 
    /// @param style 
    /// @param position 
    /// @param size 
    /// @returns if the box was clicked this frame 
    bool checkBox(bool* _bool, const std::string& style, const Layout2D& position, const Layout2D& size);

    void setWindowSizeFn(const std::function<GetWindowSizeCallback>& fn);
    [[nodiscard]] std::function<GetWindowSizeCallback> getWindowSizeFn();

    void setMousePositionFn(const std::function<GetMousePositionCallback>& fn);
    [[nodiscard]] std::function<GetMousePositionCallback> getMousePositionFn();

    void setMouseDownFn(const std::function<IsMousePressedCallback>& fn);
    [[nodiscard]] std::function<IsMousePressedCallback> getMouseDownFn();

    void setFontFileLoaderFn(const std::function<LoadFontFromFileCallback>& fn);
    [[nodiscard]] std::function<LoadFontFromFileCallback> getFontFileLoaderFn();

    void setFontMemoryLoaderFn(const std::function<LoadFontFromMemoryCallback>& fn);
    [[nodiscard]] std::function<LoadFontFromMemoryCallback> getFontMemoryLoaderFn();

    
    void pushSubRect(const Layout2D& position, const Layout2D& size);
    void popSubRect();
    namespace priv {
        [[nodiscard]] bool isMouseDown();
        [[nodiscard]] bool isMouseHeld();
        [[nodiscard]] bool isMouseUp();

        [[nodiscard]] Vector2u windowSize();
        [[nodiscard]] Vector2i mousePos();
    }
}
#endif