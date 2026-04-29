#include <stdio.h>
#include <GaUIL/GaUIL.h>
#include <raylib.h>
bool running = true;
void triangleListCallback(const gauil_Vertex* vertices, size_t vertexCount, gauil_Color color, void* userData) {
    Vector2* verts =(Vector2*)vertices;
    for (int i = 0; i < vertexCount; i += 3) {
        DrawTriangle(verts[i], verts[i + 1], verts[i + 2], (Color) { color.r, color.g, color.b, color.a });
    }
}
void triangleStripCallback(const gauil_Vertex* vertices, size_t vertexCount, gauil_Color color, void* userData) {
    DrawTriangleStrip((Vector2*)vertices, vertexCount, (Color) { color.r, color.g, color.b, color.a });
}
void triangleFanCallback(const gauil_Vertex* vertices, size_t vertexCount, gauil_Color color, void* userData) {
    DrawTriangleFan((Vector2*)vertices, vertexCount, (Color) { color.r, color.g, color.b, color.a });
}
void rectDrawCallback(gauil_FRect rect, gauil_Color color, void* userData) {
    DrawRectangle(rect.x, rect.y, rect.w, rect.h, (Color) { color.r, color.g, color.b, color.a });
}

void* loadTextureCallback(const uint8_t* pixels, int width, int height) {
    Image temp;
    temp.data = (void*)pixels;
    temp.width = width;
    temp.height = height;
    temp.mipmaps = 1;
    temp.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    Texture2D* result = (Texture2D*)malloc(sizeof(Texture2D));
    *result = LoadTextureFromImage(temp);


    return result;
}

typedef struct TextureFreeListNode {
    Texture2D* texture;
    struct TextureFreeListNode* next;
} TextureFreeListNode;
TextureFreeListNode* root = NULL;
void unloadTextureCallback(void* texture) {
    TextureFreeListNode* node = (TextureFreeListNode*)malloc(sizeof(TextureFreeListNode));
    node->texture = (Texture2D*)texture;
    node->next = NULL;


    if (root) {
        TextureFreeListNode* last = root;
        while (last->next) {
            last = last->next;
        }
        last->next = node;
    }
    else {
        root = node;
    }

}
void dumpFreeList() {
    TextureFreeListNode* node = root;
    while (node) {
        TextureFreeListNode* next = node->next;
        UnloadTexture(*node->texture);
        free(node->texture);
        free(node);
        node = next;
    }
    root = NULL;
}
void drawTextureCallback(void* texture, gauil_Vector2f position, gauil_Vector2f scale, gauil_Color color, void* userData) {
    Texture2D* tex = (Texture2D*)texture;
    DrawTexturePro(*tex, (Rectangle) { 0, 0, tex->width, tex->height }, (Rectangle) { position.x, position.y, tex->width* scale.x, tex->height* scale.y }, (Vector2) { 0, 0 }, 0, (Color) { color.r, color.g, color.b, color.a });
}


gauil_Vector2u windowSizeCallback() {
    return (gauil_Vector2u) { GetScreenWidth(), GetScreenHeight() };
}
bool isMouseDownCallback() {
    return IsMouseButtonDown(MOUSE_BUTTON_LEFT);
}
gauil_Vector2i mousePositionCallback() {
    return (gauil_Vector2i) { GetMouseX(), GetMouseY() };
}

typedef struct UIStack {
    void (*screen)();
    struct UIStack* next;
} UIStack;
UIStack* uiStackRoot = NULL;
void pushUI(void(*elem)()){
    if (uiStackRoot == NULL){
        uiStackRoot = (UIStack*)malloc(sizeof(UIStack));
        uiStackRoot->screen = elem;
        uiStackRoot->next = NULL;
    }
    else {
        UIStack* newNode = (UIStack*)malloc(sizeof(UIStack));
        newNode->screen = elem;
        newNode->next = uiStackRoot;
        uiStackRoot = newNode;
    }
}
void popUI() {
    if (uiStackRoot) {
        UIStack* next = uiStackRoot->next;
        free(uiStackRoot);
        uiStackRoot = next;
    }
}


void options() {
    if (gauil_button("Back[@@]back",(gauil_Layout2D){ 0, 0 }, (gauil_Layout2D){ GAUIL_PERCENT(5), GAUIL_PERCENT(5) })) {
        popUI();
    }

    static bool vsync = false;
    gauil_label("VSync[@@]options", (gauil_Layout2D){ GAUIL_PERCENT(0), GAUIL_PERCENT(10) }, (gauil_Layout2D){ GAUIL_PERCENT(15), gauil_scaleWithOpposingAxis(GAUIL_PERCENT(5)) });
    gauil_checkBox(&vsync, "", (gauil_Layout2D){ GAUIL_PERCENT(40), GAUIL_PERCENT(10) }, (gauil_Layout2D){ GAUIL_PERCENT(5), GAUIL_OTHER_LAYOUT });

    static float volume = 50.f;
    gauil_label("Volume[@@]options", (gauil_Layout2D){ GAUIL_PERCENT(0), GAUIL_PERCENT(20) }, (gauil_Layout2D){ GAUIL_PERCENT(15), gauil_scaleWithOpposingAxis(GAUIL_PERCENT(5)) });
    gauil_slider(&volume, "", 0.f, 100.f, (gauil_Layout2D){ GAUIL_PERCENT(40), GAUIL_PERCENT(20) }, (gauil_Layout2D){ GAUIL_PERCENT(30), gauil_scaleWithOpposingAxis(GAUIL_PERCENT(5)) });

}


void mainMenu() {
    gauil_panel((gauil_Layout2D) { GAUIL_PERCENT(10), GAUIL_PERCENT(10) }, (gauil_Layout2D) { GAUIL_PERCENT(80), GAUIL_PERCENT(30) }, "panel");
    gauil_label("Dev[$$]Hi[@@]bear", (gauil_Layout2D) { GAUIL_PERCENT(10), GAUIL_PERCENT(10) }, (gauil_Layout2D) { GAUIL_PERCENT(80), GAUIL_PERCENT(30) });

    if (gauil_button("Start", (gauil_Layout2D) { GAUIL_PERCENT(45), GAUIL_PERCENT(50) }, (gauil_Layout2D) { GAUIL_PERCENT(10), GAUIL_PERCENT(4) })) {
        printf("Start\n");
    }

    if (gauil_button("Options", (gauil_Layout2D) { GAUIL_PERCENT(45), GAUIL_PERCENT(55) }, (gauil_Layout2D) { GAUIL_PERCENT(10), GAUIL_PERCENT(4) })) {
       pushUI(options);
    }
    if (gauil_button("Quit[$$]X[@@]", (gauil_Layout2D) { GAUIL_PERCENT(45), GAUIL_PERCENT(60) }, (gauil_Layout2D) { GAUIL_PERCENT(10), GAUIL_PERCENT(4) })) {
       running = false;
    }
}
int main(int argc, char** argv) {
    InitWindow(800, 600, "GaUIL Dev C");
    SetWindowState(FLAG_WINDOW_RESIZABLE);

    gauil_setTriangleListDrawFn(triangleListCallback);
    gauil_setTriangleStripDrawFn(triangleStripCallback);
    gauil_setTriangleFanDrawFn(triangleFanCallback);
    gauil_setRectDrawFn(rectDrawCallback);
    gauil_setWindowSizeFn(windowSizeCallback);
    gauil_setMouseDownFn(isMouseDownCallback);
    gauil_setMousePositionFn(mousePositionCallback);
    gauil_setLoadTextureFn(loadTextureCallback);
    gauil_setUnloadTextureFn(unloadTextureCallback);
    gauil_setDrawTextureFn(drawTextureCallback);

    gauil_setDefaultFontFromFile("/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf");
    gauil_init();

    char* style = LoadFileText("theme.simss");
    gauil_loadStyle(style);
    UnloadFileText(style);

    gauil_loadFontFromFile("orange-juice/orange juice 2.0.ttf", "Orange Juice");
    gauil_loadFontFromFile("Wedgie Regular.ttf", "Wedgie");

    SetTraceLogLevel(LOG_ERROR);
    pushUI(mainMenu);
    while (!WindowShouldClose() ) {
        PollInputEvents();
        BeginDrawing();
        ClearBackground(RED);
        gauil_update();

        uiStackRoot->screen();

        gauil_draw();
        EndDrawing();
        dumpFreeList();
        if (!running){
            CloseWindow();
        }
    }
   
}