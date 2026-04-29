#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <GaUIL/GaUIL.hpp>
#include <GaUIL/Draw.hpp>

#include <fstream>
#include <stack>

using namespace gauil::literals;
namespace dev {
    template <sf::PrimitiveType T>
    void drawTriangles(const gauil::Vertex* vertices, size_t vertexCount, const gauil::Color& color, const std::any& userData) {
        sf::VertexArray vertArr = sf::VertexArray(T, vertexCount);
        for (size_t i = 0; i < vertexCount; ++i) {
            vertArr[i].position = vertices[i].position;
            vertArr[i].color = color;
        }




        sf::RenderWindow* window = std::any_cast<sf::RenderWindow*>(userData);
        window->draw(vertArr);
    }


    void drawRect(const gauil::FRect& rect, const gauil::Color& color, const std::any& userData) {
        sf::RectangleShape box;
        box.setPosition(rect.position);
        box.setSize(rect.size);
        box.setFillColor(color);



        sf::RenderWindow* window = std::any_cast<sf::RenderWindow*>(userData);
        window->draw(box);
    }

    void drawString(const gauil::Vector2f& pos, uint fontSize, float scale, const std::string& string, const gauil::Color& color, const std::any& font, const std::any& userData) {
        sf::Text text(*std::any_cast<std::shared_ptr<sf::Font>>(font), string);
        text.setCharacterSize(fontSize);

        text.setScale({ scale, scale });
        text.setPosition(pos - text.getGlobalBounds().position);
        text.setFillColor(color);
        sf::RenderWindow* window = std::any_cast<sf::RenderWindow*>(userData);
        window->draw(text);
    }

    gauil::Vector2f measureText(const std::string& string, uint fontSize, const std::any& font, const std::any& userData) {
        sf::Text text(*std::any_cast<std::shared_ptr<sf::Font>>(font), string);
        text.setCharacterSize(fontSize);

        sf::FloatRect bounds = text.getLocalBounds();
        return bounds.size;
    }

    std::optional<std::any> loadFontFromFile(const std::string& file) {
        std::shared_ptr<sf::Font> font = std::make_shared<sf::Font>();
        if (!font->openFromFile(file))
            return {};
        return font;
    }

    std::optional<std::any> loadTexture(const uint8_t* pixels, int width, int height) {
        sf::Image image;
        image.resize({ width, height }, pixels);
        std::shared_ptr<sf::Texture> texture = std::make_shared<sf::Texture>();
        if (!texture->loadFromImage(image))
            return {};
        return texture;
    }
    void drawTexture(const std::any& texture, const gauil::Vector2f& position, const gauil::Vector2f& scale, const gauil::Color& color, const std::any& userData) {
        sf::Sprite sprite(*std::any_cast<std::shared_ptr<sf::Texture>>(texture));
        sprite.setPosition(position);
        sprite.setScale(scale);
        sprite.setColor(color);

        sf::RenderWindow* window = std::any_cast<sf::RenderWindow*>(userData);
        window->draw(sprite);
    }
}
sf::RenderWindow* window;


std::stack<void(*)()> uiStack;

void mainMenu();
void options();

void mainMenu() {
    gauil::panel({ 10_percent, 10_percent }, { 80_percent, 30_percent }, "panel");
    gauil::label("Orange Juice[$$]Hi[@@]bear", { 10_percent, 10_percent }, { 80_percent, 30_percent });

    if (gauil::button("Start", { 45_percent, 50_percent }, { 10_percent, 4_percent })) {
        printf("Start\n");
    }

    if (gauil::button("Options", { 45_percent, 55_percent }, { 10_percent, 4_percent })) {
        uiStack.push(options);
    }
    if (gauil::button("Quit[$$]X[@@]", { 45_percent, 60_percent }, { 10_percent, 4_percent })) {
        window->close();
    }
}

void options() {
    if (gauil::button("Back[@@]back", { 0, 0 }, { 5_percent, 5_percent })) {
        uiStack.pop();
    }

    static bool vsync = false;
    gauil::label("VSync[@@]options", { 0_percent, 10_percent }, { 15_percent, gauil::scaleWithOpposingAxis(5_percent) });
    gauil::checkBox(&vsync, "", { 40_percent, 10_percent }, { 5_percent, gauil::OTHER_LAYOUT });

    static float volume = 50.f;
    gauil::label("Volume[@@]options", { 0_percent, 20_percent }, { 15_percent, gauil::scaleWithOpposingAxis(5_percent) });
    gauil::slider(&volume, "", 0.f, 100.f, { 40_percent, 20_percent }, { 30_percent, gauil::scaleWithOpposingAxis(5_percent) });

}

void initCallbacks() {
    gauil::setTriangleListDrawFn(dev::drawTriangles<sf::PrimitiveType::Triangles>);
    gauil::setTriangleFanDrawFn(dev::drawTriangles<sf::PrimitiveType::TriangleFan>);
    gauil::setTriangleStripDrawFn(dev::drawTriangles<sf::PrimitiveType::TriangleStrip>);

    gauil::setRectDrawFn(dev::drawRect);


    gauil::setWindowSizeFn([] {
        return window->getSize();
        });
    gauil::setMouseDownFn([] {
        return window->hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        });
    gauil::setMousePositionFn([] {
        return sf::Mouse::getPosition(*window);
        });
    gauil::setLoadTextureFn(dev::loadTexture);
    gauil::setDrawTextureFn(dev::drawTexture);

    gauil::setDrawUserData(static_cast<sf::RenderWindow*>(window));

}

int main() {
    auto fullscreenModes = sf::VideoMode::getFullscreenModes();

    sf::ContextSettings windowSettings;
    windowSettings.antiAliasingLevel = 0;
    window = new sf::RenderWindow();
    window->create(fullscreenModes[0], "GaUIL Dev", sf::State::Windowed, windowSettings);



    initCallbacks();

    gauil::setDefaultFont("/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf");

    gauil::init();
    gauil::loadFont("orange-juice/orange juice 2.0.ttf", "Orange Juice");
    gauil::loadFont("Wedgie Regular.ttf", "Wedgie");


    std::string theme;
    std::ifstream stream("theme.simss");
    std::getline(stream, theme, '\0');
    gauil::loadStyle(theme);
    stream.close();


    stream.open("orange-juice/orange juice 2.0.ttf", std::ios::binary);
    std::vector<uint8_t> buf(std::filesystem::file_size("orange-juice/orange juice 2.0.ttf"));
    stream.read((char*)buf.data(), buf.size());
    stream.close();

    gauil::IFont* font = gauil::loadFont(buf.data(), buf.size());
    sf::Font f;
    f.openFromMemory(buf.data(), buf.size());

    uiStack.push(mainMenu);
    while (window->isOpen()) {
        while (auto ev = window->pollEvent()) {
            if (ev->is<sf::Event::Closed>()) {
                window->close();
            }
            else if (auto resized = ev->getIf<sf::Event::Resized>()) {
                sf::View view = window->getView();
                view.setSize((sf::Vector2f)resized->size);
                view.setCenter(view.getSize() / 2.f);
                window->setView(view);
            }
            else if (auto key = ev->getIf<sf::Event::KeyReleased>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    window->close();
                }
            }
        }

        gauil::update();

        uiStack.top()();

        gauil::draw();

        window->display();
        window->clear(sf::Color(30, 20, 60));

    }

    return 0;
}