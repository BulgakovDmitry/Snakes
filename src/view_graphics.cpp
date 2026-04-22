#include "view/view_graphics.hpp"
#include "view/colors.hpp"
#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>

namespace snakes {

struct GraphicsView::Impl {
    sf::RenderWindow window;
    sf::RectangleShape shape;

    uint32_t field_width = 0;
    uint32_t field_height = 0;
    float pixels_per_cell = 20.f;

    Impl(uint32_t field_w, uint32_t field_h);
    ~Impl();

    void draw_snakes(const GameModel& model);
    void draw_rabbits(const GameModel& model);
    void draw_preview(const GameModel& model);
private:
};

// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
static sf::Color convert_to_sfml_color(int colorId);


GraphicsView::GraphicsView(uint32_t width, uint32_t height) : impl_(std::make_unique<Impl>(width, height)) {}
GraphicsView::~GraphicsView() = default;

GraphicsView::Impl::Impl(uint32_t field_w, uint32_t field_h) : field_width(field_w), field_height(field_h){
    window.create(sf::VideoMode((field_width) * pixels_per_cell, (field_height+4) * pixels_per_cell), "Snakes");
    window.setFramerateLimit(60);
    shape.setFillColor(sf::Color::Green);
}

GraphicsView::Impl::~Impl() {}

void GraphicsView::render(GameModel& model) {
    if (!impl_->window.isOpen()) return;

    model.width = impl_->field_width;
    model.height = impl_->field_height;

    impl_->window.clear();

    impl_->draw_preview(model);
    impl_->draw_snakes(model);
    impl_->draw_rabbits(model);

    impl_->window.display();
}

inline void GraphicsView::Impl::draw_snakes(const GameModel& model) {
    sf::RectangleShape cell({pixels_per_cell, pixels_per_cell});

    for (const Snake& snake : model.snakes) {
        cell.setFillColor(convert_to_sfml_color(snake.color())); 
        for (const Point& p : snake.body()) {
            cell.setPosition(
                p.x * pixels_per_cell,
                p.y * pixels_per_cell
            );
            window.draw(cell);   
        }
    }
}

inline void GraphicsView::Impl::draw_rabbits(const GameModel& model) {
    sf::RectangleShape cell({pixels_per_cell, pixels_per_cell});

    cell.setFillColor(sf::Color::White);
    for (const Rabbit& rabbit : model.rabbits) {
        Point p = rabbit.position();
        cell.setPosition(
            p.x * pixels_per_cell,
            p.y * pixels_per_cell
        );
        window.draw(cell);
    }
}

inline void GraphicsView::Impl::draw_preview(const GameModel& /*model*/) {
    const float title_height = 3 * pixels_per_cell;
    sf::RectangleShape title_box({static_cast<float>(window.getSize().x), title_height});
    title_box.setPosition(0.f, 0.f);
    title_box.setFillColor(sf::Color(80, 180, 80));
    title_box.setOutlineThickness(0.f);
    title_box.setOutlineColor(sf::Color::White);
    window.draw(title_box);
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


static sf::Color convert_to_sfml_color(int colorId) {
    switch (colorId) {
        // reset / default
        case ansi_reset:
            return sf::Color::White;

        // black
        case fg_black:
        case bg_black:
            return sf::Color(0, 0, 0);

        // red
        case fg_red:
        case bg_red:
            return sf::Color(170, 0, 0);

        // green
        case fg_green:
        case bg_green:
            return sf::Color(0, 170, 0);

        // yellow
        case fg_yellow:
        case bg_yellow:
            return sf::Color(170, 85, 0);

        // blue
        case fg_blue:
        case bg_blue:
            return sf::Color(0, 0, 170);

        // magenta
        case fg_magenta:
        case bg_magenta:
            return sf::Color(170, 0, 170);

        // cyan
        case fg_cyan:
        case bg_cyan:
            return sf::Color(0, 170, 170);

        // white
        case fg_white:
        case bg_white:
            return sf::Color(170, 170, 170);

        // bright black (gray)
        case fg_bright_black:
        case bg_bright_black:
            return sf::Color(85, 85, 85);

        // bright red
        case fg_bright_red:
        case bg_bright_red:
            return sf::Color(255, 85, 85);

        // bright green
        case fg_bright_green:
        case bg_bright_green:
            return sf::Color(85, 255, 85);

        // bright yellow
        case fg_bright_yellow:
        case bg_bright_yellow:
            return sf::Color(255, 255, 85);

        // bright blue
        case fg_bright_blue:
        case bg_bright_blue:
            return sf::Color(85, 85, 255);

        // bright magenta
        case fg_bright_magenta:
        case bg_bright_magenta:
            return sf::Color(255, 85, 255);

        // bright cyan
        case fg_bright_cyan:
        case bg_bright_cyan:
            return sf::Color(85, 255, 255);

        // bright white
        case fg_bright_white:
        case bg_bright_white:
            return sf::Color(255, 255, 255);

        default:
            return sf::Color::White;
    }
}

} // namespace snakes