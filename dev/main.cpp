#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <GaUIL/GaUIL.hpp>

#include <fstream>
#include <stack>


namespace dev {
    template <sf::PrimitiveType T>
    void drawTriangles(const gauil::Vertex* vertices, size_t vertexCount, const gauil::Color& color, const std::any& userData) {
        sf::VertexArray vertArr = sf::VertexArray(T, vertexCount);
        for (size_t i = 0; i < vertexCount; ++i) {
            vertArr[i].position = vertices[i].position;
            vertArr[i].color = color;
            vertArr[i].texCoords = vertices[i].texCoords;
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
        sf::Text text(*std::any_cast<sf::Font*>(font), string);
        text.setCharacterSize(fontSize);

        text.setScale({ scale, scale });
        text.setPosition(pos - text.getGlobalBounds().position);
        text.setFillColor(color);
        sf::RenderWindow* window = std::any_cast<sf::RenderWindow*>(userData);
        window->draw(text);
    }

    gauil::Vector2f measureText(const std::string& string, uint fontSize, const std::any& font, const std::any& userData) {
        sf::Text text(*std::any_cast<sf::Font*>(font), string);
        text.setCharacterSize(fontSize);

        sf::FloatRect bounds = text.getLocalBounds();
        return bounds.size;
    }

}
sf::RenderWindow* window;


std::stack<void(*)()> uiStack;

void mainMenu();
void options();

void mainMenu() {
    using namespace gauil::literals;
    gauil::label("Dev", { 10_percent, 10_percent }, { 80_percent, 30_percent });

    if (gauil::button("Start", { 45_percent, 50_percent }, { 10_percent, 4_percent })) {
        printf("Start\n");
    }

    if (gauil::button("Options", { 45_percent, 55_percent }, { 10_percent, 4_percent })) {
        uiStack.push(options);
    }
    if (gauil::button("Quit", { 45_percent, 60_percent }, { 10_percent, 4_percent })) {
        window->close();
    }
}

void options() {
    using namespace gauil::literals;
    if (gauil::button("Back", { 0, 0 }, { 5_percent, 5_percent })) {
        uiStack.pop();
    }
}

void initCallbacks() {
    gauil::setTriangleListDrawFn(dev::drawTriangles<sf::PrimitiveType::Triangles>);
    gauil::setTriangleFanDrawFn(dev::drawTriangles<sf::PrimitiveType::TriangleFan>);
    gauil::setTriangleStripDrawFn(dev::drawTriangles<sf::PrimitiveType::TriangleStrip>);

    gauil::setRectDrawFn(dev::drawRect);

    gauil::setTextDrawFn(dev::drawString);

    gauil::setMeasureTextFn(dev::measureText);

    gauil::setWindowSizeFn([] {
        return window->getSize();
        });
    gauil::setMouseDownFn([] {
        return window->hasFocus() && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
        });
    gauil::setMousePositionFn([] {
        return sf::Mouse::getPosition(*window);
        });

    gauil::setDrawUserData(static_cast<sf::RenderWindow*>(window));


}

int main() {
    using namespace gauil::literals;



    auto fullscreenModes = sf::VideoMode::getFullscreenModes();

    sf::ContextSettings windowSettings;
    windowSettings.antiAliasingLevel = 0;
    window = new sf::RenderWindow();
    window->create(fullscreenModes[0], "Dev", sf::State::Windowed, windowSettings);



    sf::Font font;
    if (!font.openFromFile("/usr/share/fonts/truetype/noto/NotoMono-Regular.ttf"));

    gauil::setDefaultFont(static_cast<sf::Font*>(&font));

    initCallbacks();

    gauil::init();

    gauil::Style style;
    //style.button.backgroundColor = gauil::Color(22, 66, 144);
    gauil::setStyle(style);



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
        }

        gauil::update();


        uiStack.top()();

        gauil::priv::roundRect({ 200, 300, 300, 300 }, gauil::FCorners(1000, 10, 0, 10), gauil::FEdges(10, 0, 10, 10), gauil::color::DARK_MODE_BACKGROUND, gauil::color::DARK_MODE_BORDER);
        // gauil::priv::roundRect({600, 200, 200, 200}, {}, gauil::FEdges(10), gauil::color::DARK_MODE_BACKGROUND, gauil::color::DARK_MODE_BORDER);

        gauil::draw();


        window->display();
        window->clear(sf::Color::Green);

    }

    return 0;
}