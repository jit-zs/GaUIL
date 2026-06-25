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


    /// @brief Initializes the UI and makes sure that all callbacks have been set
    /// @note Only one of the font loaders callbacks need to be set
    void init();
    void cleanup();

    /// @brief Load a font from a file and adds it to the font list
    /// @return True on success
    bool loadFont(const std::string& file, const std::string& referenceName);
    /// @brief Loads a font from memory and adds it to the font list
    /// @returns True on success
    bool loadFont(const uint8_t* data, size_t length, const std::string& referenceName);

    /// @brief Loads a nine sliced image from memory and adds it to the nine slice list
    /// @param data 
    /// @param length 
    /// @param referenceName The name that will be used to reference it in the style sheet
    /// @returns True on success
    bool loadNineSlice(const uint8_t* data, size_t length, const URect& sliceRect, const std::string& referenceName);

    /// @brief Loads Simple Style Sheet from string
    /// @returns true on success else false
    bool loadStyle(const std::string& string);

    /// @brief Prepares the GUI system for a new frame
    void update();

    /// @brief Draws all cached UI elements to the screen
    void draw();

    /// @brief Draws a label
    /// @param position 
    /// @param size 
    /// @param text 
    void label(const std::string& text, const Layout2D& position, const  Layout2D& size);

    /// @brief Draws a panel
    void panel(const Layout2D& position, const Layout2D& size, const std::string& style = "");

    /// @brief Draws a button
    /// @param position 
    /// @param size 
    /// @param text 
    /// @return Whether the button was released over the button
    bool button(const std::string& text, const Layout2D& position, const Layout2D& size);



    /// @brief 
    /// @param value 
    /// @param style 
    /// @param position 
    /// @param size 
    /// @returns if the box was clicked this frame 
    bool checkBox(bool* value, const std::string& style, const Layout2D& position, const Layout2D& size);


    void slider(float* value, const std::string& style, float min, float max, const Layout2D& position, const Layout2D& size);

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