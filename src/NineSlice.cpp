#include <GaUIL/NineSlice.hpp>
#include <stb_image.h>

namespace gauil{
    NineSlice* loadNineSlice(const uint8_t* bytes, size_t length, const URect& sliceRect){
        if (!bytes || length == 0){
            printf("Invalid image file\n");
            return nullptr;
        }
        int width, height;
        uint8_t* image = stbi_load_from_memory(bytes, length, &width, &height, NULL, 4);
        if (!image){
            printf("Failed to load iamge\n");
            return nullptr;
        }
        
        if(sliceRect.x + sliceRect.w <= width - 1 && sliceRect.y + sliceRect.h <= height - 1){
            printf("Slice rect is out of bounds of the source image\n");
            return nullptr;
        }
        NineSlice* result = new NineSlice(image, width, height, sliceRect);
        stbi_image_free(image);
        return result;
    }
}