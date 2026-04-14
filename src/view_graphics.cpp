#include "view/view_graphics.hpp"
#include <SFML/Graphics.hpp>
#include <memory>

namespace snakes {

struct GraphicsView::Impl {
    sf::RenderWindow window;
    sf::RectangleShape shape;
    float pixels_per_cell = 10;

    Impl();
    ~Impl();

};

// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
GraphicsView::GraphicsView(uint32_t width, uint32_t height) : impl_(std::make_unique<Impl>()) {}
GraphicsView::~GraphicsView() = default;

GraphicsView::Impl::Impl() {
    window.create(sf::VideoMode(600, 600), "Snakes");
    window.setFramerateLimit(60);
    shape.setFillColor( sf::Color::Green );
}

GraphicsView::Impl::~Impl()  {
    window.close();
}


void GraphicsView::render(GameModel& model) {
    if (!impl_->window.isOpen()) return;

    impl_->window.clear();

    // тута что-то нужно делать
    sf::RectangleShape cell({
        impl_->pixels_per_cell - 1.f,
        impl_->pixels_per_cell - 1.f
    });

    // for (const auto& rabbit : model.rabbits) {
    //     impl_->shape.setSize({impl_->pixels_per_cell, impl_->pixels_per_cell});
    //     impl_->shape.setPosition(rabbit.position().x * impl_->pixels_per_cell, rabbit.position().y * impl_->pixels_per_cell);
    //     impl_->shape.setFillColor(sf::Color::Red);
    //     impl_->window.draw(impl_->shape);
    // }
    //
    for (const Snake& snake : model.snakes) {
        cell.setFillColor(sf::Color::Green); // потом подставишь цвет змеи
        for (const Point& p : snake.body()) {
            cell.setPosition(
                p.x * impl_->pixels_per_cell,
                p.y * impl_->pixels_per_cell
            );
            impl_->window.draw(cell);
        }
    }

    cell.setFillColor(sf::Color::White);
    for (const Rabbit& rabbit : model.rabbits) {
        Point p = rabbit.position();
        cell.setPosition(
            p.x * impl_->pixels_per_cell,
            p.y * impl_->pixels_per_cell
        );
        impl_->window.draw(cell);
    }

    impl_->window.display();
}



std::optional<Event> GraphicsView::poll_event() {
    sf::Event event;
    while (impl_->window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            impl_->window.close();
            return Event{KeyEvents::exit};
        }

        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::W: return Event{KeyEvents::up_1};
                case sf::Keyboard::S: return Event{KeyEvents::down_1};
                case sf::Keyboard::A: return Event{KeyEvents::left_1};
                case sf::Keyboard::D: return Event{KeyEvents::right_1};

                case sf::Keyboard::Up:    return Event{KeyEvents::up_2};
                case sf::Keyboard::Down:  return Event{KeyEvents::down_2};
                case sf::Keyboard::Left:  return Event{KeyEvents::left_2};
                case sf::Keyboard::Right: return Event{KeyEvents::right_2};

                case sf::Keyboard::Q: return Event{KeyEvents::exit};
                case sf::Keyboard::P: return Event{KeyEvents::pause};
                case sf::Keyboard::R: return Event{KeyEvents::restart};
                default: break;
            }
        }
    }

    return std::nullopt;
}

} // namespace snakes