#ifndef GAUIL_NINE_SLICE_HPP
#define GAUIL_NINE_SLICE_HPP

#include <GaUIL/Assert.hpp>
#include <GaUIL/Draw.hpp>

namespace gauil {
    class NineSlice {
        std::any mTopLeft, mTop, mTopRight, mLeft, mCenter, mRight, mBottomLeft, mBottom, mBottomRight;
        URect mSliceRect;
        /// @note This creates a texture using the load texture callback
        std::any _makeSlice(const uint8_t* bytes, uint width, uint height, const URect& cut) {
            GAUIL_ASSERT(bytes && width > 0 && height > 0, "Invalid image data");
            GAUIL_ASSERT(cut.x + cut.w <= width && cut.y + cut.h <= height, "Cut rect is out of bounds of the source image");
            uint8_t* pixels = new uint8_t[cut.w * cut.h * 4];
            for (size_t i = 0; i < cut.w; i++) {
                for (size_t j = 0; j < cut.h; j++) {
                    size_t searchX = cut.x + i;
                    size_t searchY = cut.y + j;
                    size_t idx = (searchY * width + searchX) * 4;
                    pixels[(j * cut.w + i) * 4] = bytes[idx];
                    pixels[(j * cut.w + i) * 4 + 1] = bytes[idx + 1];
                    pixels[(j * cut.w + i) * 4 + 2] = bytes[idx + 2];
                    pixels[(j * cut.w + i) * 4 + 3] = bytes[idx + 3];
                }
            }
            auto image = priv::loadTexture(pixels, cut.w, cut.h);
            delete[] pixels;
            GAUIL_ASSERT(image, "Check your texture loading callback");
            return image.value();


        }
    public:
        NineSlice(const NineSlice&) = delete;
        NineSlice& operator=(const NineSlice&) = delete;
        NineSlice(const uint8_t* bytes, uint width, uint height, const URect& sliceRect) : mSliceRect(sliceRect) {
            GAUIL_ASSERT(bytes && width > 0 && height > 0, "Invalid image data");
            GAUIL_ASSERT(sliceRect.x + sliceRect.w <= width - 1 && sliceRect.y + sliceRect.h <= height - 1, "Slice rect is out of bounds of the source image");
            mTopLeft = _makeSlice(bytes, width, height, { 0, 0, sliceRect.x, sliceRect.y });
            mTop = _makeSlice(bytes, width, height, { sliceRect.x, 0, sliceRect.w, sliceRect.y });
            mTopRight = _makeSlice(bytes, width, height, { sliceRect.x + sliceRect.w, 0, width - (sliceRect.x + sliceRect.w), sliceRect.y });
            mLeft = _makeSlice(bytes, width, height, { 0, sliceRect.y, sliceRect.x, sliceRect.h });
            mCenter = _makeSlice(bytes, width, height, { sliceRect.x, sliceRect.y, sliceRect.w, sliceRect.h });
            mRight = _makeSlice(bytes, width, height, { sliceRect.x + sliceRect.w, sliceRect.y, width - (sliceRect.x + sliceRect.w), sliceRect.h });
            mBottomLeft = _makeSlice(bytes, width, height, { 0, sliceRect.y + sliceRect.h, sliceRect.x, height - (sliceRect.y + sliceRect.h) });
            mBottom = _makeSlice(bytes, width, height, { sliceRect.x, sliceRect.y + sliceRect.h, sliceRect.w, height - (sliceRect.y + sliceRect.h) });
            mBottomRight = _makeSlice(bytes, width, height, { sliceRect.x + sliceRect.w ,sliceRect.y + sliceRect.h ,width - (sliceRect.x + sliceRect.w), height - (sliceRect.y + sliceRect.h) });
        }
        ~NineSlice() {
            priv::unloadTexture(mTopLeft);
            priv::unloadTexture(mTop);
            priv::unloadTexture(mTopRight);
            priv::unloadTexture(mLeft);
            priv::unloadTexture(mCenter);
            priv::unloadTexture(mRight);
            priv::unloadTexture(mBottomLeft);
            priv::unloadTexture(mBottom);
            priv::unloadTexture(mBottomRight);
        }
        void draw(const Vector2f& position, const Vector2f& scale, const Color& color) {
            priv::drawTexture(mTopLeft, position, Vector2f::ONE, color);

            priv::drawTexture(mBottomRight, position + Vector2f(mSliceRect.x + mSliceRect.w, mSliceRect.y + mSliceRect.h) * scale, Vector2f::ONE, color);
        }
    };
    /// @note FOR INTERNAL USE ONLY
    NineSlice* loadNineSlice(const uint8_t* bytes, size_t length, const URect& sliceRect) ;
}
#endif