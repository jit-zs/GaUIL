#include <GaUIL/Text.hpp>
#include <GaUIL/Draw.hpp>

#include <stb_truetype.h>



namespace gauil {
    IFont::~IFont() {}

    class Font : public IFont {
        stbtt_fontinfo mFont;
        uint8_t* mFileBytes = nullptr;
    public:
        /// @brief 
        /// @param text 
        /// @param textScale 
        /// @returns The maximum ascent of the characters in the first line
        float getHeaderHeight(const std::string& _text, float textScale) const {
            std::string text = _text.substr(0, _text.find('\n'));


            float maxLineHeight = 0;
            const char* currChar = text.data();
            while ((*currChar)) {
                int xStart, yStart, xEnd, yEnd;
                stbtt_GetCodepointBitmapBox(&mFont, *currChar, textScale, textScale, &xStart, &yStart, &xEnd, &yEnd);
                if (abs(yStart) > maxLineHeight) {
                    maxLineHeight = abs(yStart);
                }
                currChar++;
            }
            return maxLineHeight;
        }
        /// @brief 
        /// @param text 
        /// @return The maximum descent of the characters in the last line 
        float getFooterDrop(const std::string& _text, float textScale) const {
            size_t cutPos = _text.find_last_of('\n');
            cutPos = cutPos == std::string::npos ? 0 : cutPos + 1;
            std::string text = _text.substr(cutPos);

            float maxLineDrop = 0;
            const char* currChar = text.data();
            while (*currChar) {
                int xStart, yStart, xEnd, yEnd;
                stbtt_GetCodepointBitmapBox(&mFont, *currChar, textScale, textScale, &xStart, &yStart, &xEnd, &yEnd);
                if (abs(yEnd) > maxLineDrop) {
                    maxLineDrop = yEnd;
                }
                currChar++;
            }
            return maxLineDrop;
        }
        void draw(const gauil::Vector2f& position, const std::string& text, uint fontSize, float scale, const gauil::Color& color) const override {
            gauil::Vector2f march{ 0, 0 };
            float textScale = stbtt_ScaleForPixelHeight(&mFont, fontSize) * scale;
            int ascent, descent, lineGap;
            stbtt_GetFontVMetrics(&mFont, &ascent, &descent, &lineGap);
            float maxLineHeight = getHeaderHeight(text, textScale); // Primarily used for the top row of characters
            float maxLineDrop = getFooterDrop(text, textScale); // Primarily used for the bottom row of characters
            march.y += maxLineHeight;
            for (size_t i = 0; i < text.length(); i++) {
                if (text[i] == '\n') {
                    march.y += (ascent - descent) * textScale;
                    march.x = 0;
                    continue;
                }
                int width, height, xOff, yOff;
                uint8_t* bitmap = stbtt_GetCodepointBitmap(&mFont, textScale, textScale, text[i], &width, &height, &xOff, &yOff);
                if (bitmap) {
                    std::vector<gauil::Color> pixels(width * height);
                    for (size_t i = 0; i < width * height; i++) {
                        pixels[i] = { 255, 255, 255, bitmap[i] };
                    }

                    std::optional<std::any> texture = priv::loadTexture((uint8_t*)pixels.data(), width, height);
                    if (texture) {
                        priv::drawTexture(texture.value(), position + march + Vector2f{ (float)(xOff), (float)(yOff) }, Vector2f{ 1, 1 }, color);
                        priv::unloadTexture(texture.value());

                    }
                    stbtt_FreeBitmap(bitmap, NULL);
                }


                int xAdv, xBearing;
                stbtt_GetCodepointHMetrics(&mFont, text[i], &xAdv, &xBearing);
                xAdv *= textScale;
                xBearing *= textScale;
                march.x += (xAdv) ;
                if (text[i + 1] != '\n' && text[i + 1] != '\0' && stbtt_GetKerningTableLength(&mFont) > 0) {
                    int kern = stbtt_GetCodepointKernAdvance(&mFont, text[i], text[i + 1]);
                    march.x += (kern) * textScale;
                }
                


            }
        }
        gauil::Vector2f measureText(const std::string& text, uint fontSize) const override {
            float currentXMarch = 0;
            float maxXMarch = 0;
            float yMarch = 0;
            float textScale = stbtt_ScaleForPixelHeight(&mFont, fontSize);



            int ascent, descent, lineGap;
            stbtt_GetFontVMetrics(&mFont, &ascent, &descent, &lineGap);


            float maxLineHeight = getHeaderHeight(text, textScale); // Primarily used for the top row of characters
            float maxLineDrop = getFooterDrop(text, textScale); // Primarily used for the bottom row of characters
            yMarch += maxLineHeight;
            for (size_t i = 0; i < text.length(); i++) {
                if (text[i] == '\n') {
                    yMarch += (ascent - descent) * textScale;
                    maxXMarch = std::max(maxXMarch, currentXMarch);
                    currentXMarch = 0;
                    continue;
                }
                int width, height, xStart, yStart, xEnd, yEnd;
                stbtt_GetCodepointBitmapBox(&mFont, text[i], textScale, textScale, &xStart, &yStart, &xEnd, &yEnd);

                if (text.data()[i + 1] != '\n' && text.data()[i + 1] != '\0') {
                    int xAdv, xBearing;
                    stbtt_GetCodepointHMetrics(&mFont, text[i], &xAdv, &xBearing);
                    xAdv *= textScale;
                    xBearing *= textScale;
                    currentXMarch += xAdv;
                    if (i < text.length() - 1 && stbtt_GetKerningTableLength(&mFont) > 0) {
                        int kern = stbtt_GetCodepointKernAdvance(&mFont, text[i], text[i + 1]);
                        currentXMarch += kern * textScale;
                    }
                }
                else {
                    currentXMarch += xEnd;
                }





            }
            maxXMarch = std::max(maxXMarch, currentXMarch);
            yMarch += maxLineDrop;

            return { maxXMarch, yMarch };
        }
        Font(const Font&) = delete;
        Font& operator=(const Font&) = delete;

        ~Font() {
            delete[] mFileBytes;
        }

    private:
        Font(stbtt_fontinfo font, uint8_t* data) {
            mFont = font;
            mFileBytes = data; 
        }
        friend IFont* loadFont(const uint8_t* data, size_t length);
    };


    IFont* loadFont(const uint8_t* data, size_t length) {
        stbtt_fontinfo info;
        uint8_t* fileBytes = new uint8_t[length];
        std::copy(data, data + length, fileBytes);
        if (stbtt_InitFont(&info, fileBytes, 0))
            return new Font(info, fileBytes);
        delete[] fileBytes;
        return nullptr;
    }
    void unloadFont(IFont* font) {
        delete font;
    }
}
