# UI

## Elements
- ``void label(const std::string& text, const Layout2D& position, const  Layout2D& size)``
- ``void panel(const Layout2D& position, const Layout2D& size, const std::string& style = "")``
- ``bool button(const std::string& text, const Layout2D& position, const Layout2D& size)``
- ``bool checkBox(bool* value, const std::string& style, const Layout2D& position, const Layout2D& size)``
  - Returns true if it was clicked during the current frame 
- ``void slider(float* value, const std::string& style, float min, float max, const Layout2D& position, const Layout2D& size)``
## Modifiers
- ``void pushSubRect(const Layout2D& position, const Layout2D& size)``
  - All scaling and positioning will be relative to this subrect like making a subwindow 
  - The subrect made is based off of the last subrect pushed. If no subrect has been pushed it'll use the window size
- ``void popSubRect()``
  - Pops the last rect off of the subrect stack
## Reference Variables
Reference variables are objects loaded through the library that can be referenced through a style sheet
  - ``bool loadStyle(const std::string& string)``
    - Loads a style from a string
  - Font Loaders
    - ``bool loadFont(const std::string& file, const std::string& referenceName)``
      - Loads font from file
        - referenceName: The name used to reference the font inside the style sheet
    - ``bool loadFont(const uint8_t* data, size_t length, const std::string& referenceName)``
      - Loads font from memory
        - referenceName: The name used to reference the font inside the style sheet  
