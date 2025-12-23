#ifndef GAUIL_GUI_HPP
#define GAUIL_GUI_HPP

#include <GaUIL/Layout.hpp>
#include <GaUIL/Rect.hpp>
#include <GaUIL/Style.hpp>


#include <bit>
#include <bitset>
#include <functional>
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

    /// @brief Initializes the UI and makes sure that all callbacks have been initialized
    void init();
    void cleanup();

    /// @brief Swaps the currently used style sheet 
    /// @param style The new style (IS COPIED)
    void setStyle(const Style& style);

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

    void setWindowSizeFn(const std::function<GetWindowSizeCallback>& fn);
    [[nodiscard]] std::function<GetWindowSizeCallback> getWindowSizeFn();

    void setMousePositionFn(const std::function<GetMousePositionCallback>& fn);
    [[nodiscard]] std::function<GetMousePositionCallback> getMousePositionFn();

    void setMouseDownFn(const std::function<IsMousePressedCallback>& fn);
    [[nodiscard]] std::function<IsMousePressedCallback> getMouseDownFn();

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