#include "view/view_graphics.hpp"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <optional>
#include <string>
#include <thread>

#include "view/colors.hpp"

namespace snakes {

namespace {

sf::Color convert_to_sfml_color(int color_id) {
    switch (color_id) {
        case ansi_reset:
            return sf::Color::White;
        case fg_black:
        case bg_black:
            return sf::Color(0, 0, 0);
        case fg_red:
        case bg_red:
            return sf::Color(170, 0, 0);
        case fg_green:
        case bg_green:
            return sf::Color(0, 170, 0);
        case fg_yellow:
        case bg_yellow:
            return sf::Color(170, 85, 0);
        case fg_blue:
        case bg_blue:
            return sf::Color(0, 0, 170);
        case fg_magenta:
        case bg_magenta:
            return sf::Color(170, 0, 170);
        case fg_cyan:
        case bg_cyan:
            return sf::Color(0, 170, 170);
        case fg_white:
        case bg_white:
            return sf::Color(170, 170, 170);
        case fg_bright_black:
        case bg_bright_black:
            return sf::Color(85, 85, 85);
        case fg_bright_red:
        case bg_bright_red:
            return sf::Color(255, 85, 85);
        case fg_bright_green:
        case bg_bright_green:
            return sf::Color(85, 255, 85);
        case fg_bright_yellow:
        case bg_bright_yellow:
            return sf::Color(255, 255, 85);
        case fg_bright_blue:
        case bg_bright_blue:
            return sf::Color(85, 85, 255);
        case fg_bright_magenta:
        case bg_bright_magenta:
            return sf::Color(255, 85, 255);
        case fg_bright_cyan:
        case bg_bright_cyan:
            return sf::Color(85, 255, 255);
        case fg_bright_white:
        case bg_bright_white:
            return sf::Color(255, 255, 255);
        default:
            return sf::Color::White;
    }
}

} // namespace

struct GraphicsView::Impl {
    sf::RenderWindow window;
    sf::RectangleShape shape;
    sf::Font font;

    uint32_t field_width{0};
    uint32_t field_height{0};
    float pixels_per_cell{20.f};

    Impl(uint32_t field_w, uint32_t field_h);

    void draw_snakes(const GameModel& model);
    void draw_rabbits(const GameModel& model);
    void draw_preview(const GameModel& model);

    bool process_window_events(bool return_on_exit = false);
    void draw_scoreboard_screen(const std::vector<ScoreboardEntry>& entries,
                                std::size_t current_round,
                                std::size_t total_rounds,
                                bool final_screen);
};

GraphicsView::GraphicsView(uint32_t width, uint32_t height)
    : impl_(std::make_unique<Impl>(width, height)) {}

GraphicsView::~GraphicsView() = default;

GraphicsView::Impl::Impl(uint32_t field_w, uint32_t field_h)
    : field_width(field_w),
      field_height(field_h) {
    window.create(
        sf::VideoMode(static_cast<unsigned int>(field_width * pixels_per_cell),
                      static_cast<unsigned int>((field_height + 4) * pixels_per_cell)),
        "Snakes"
    );
    window.setFramerateLimit(60);
    shape.setFillColor(sf::Color::Green);
}

void GraphicsView::render(GameModel& model) {
    if (!impl_->window.isOpen()) {
        return;
    }

    model.width = impl_->field_width;
    model.height = impl_->field_height;

    impl_->window.clear(sf::Color(18, 24, 32));
    impl_->draw_preview(model);
    impl_->draw_snakes(model);
    impl_->draw_rabbits(model);
    impl_->window.display();
}

bool GraphicsView::show_scoreboard(const std::vector<ScoreboardEntry>& entries,
                                   std::size_t current_round,
                                   std::size_t total_rounds,
                                   bool final_screen,
                                   std::chrono::milliseconds duration) {
    if (!impl_->window.isOpen()) {
        return false;
    }

    const auto finish_at = std::chrono::steady_clock::now() + duration;
    while (std::chrono::steady_clock::now() < finish_at) {
        if (!impl_->process_window_events(true)) {
            return false;
        }

        impl_->draw_scoreboard_screen(entries, current_round, total_rounds, final_screen);
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    return impl_->window.isOpen();
}

void GraphicsView::Impl::draw_snakes(const GameModel& model) {
    sf::RectangleShape cell({pixels_per_cell, pixels_per_cell});

    for (const Snake& snake : model.snakes) {
        cell.setFillColor(convert_to_sfml_color(snake.color()));
        for (const Point& p : snake.body()) {
            cell.setPosition(p.x * pixels_per_cell, p.y * pixels_per_cell);
            window.draw(cell);
        }
    }
}

void GraphicsView::Impl::draw_rabbits(const GameModel& model) {
    sf::CircleShape rabbit_shape(pixels_per_cell * 0.3f);
    rabbit_shape.setFillColor(sf::Color::White);
    rabbit_shape.setOutlineThickness(2.f);
    rabbit_shape.setOutlineColor(sf::Color(230, 230, 230));

    sf::CircleShape ear(pixels_per_cell * 0.12f);
    ear.setFillColor(sf::Color(255, 210, 220));

    for (const Rabbit& rabbit : model.rabbits) {
        const Point p = rabbit.position();
        const float base_x = p.x * pixels_per_cell;
        const float base_y = p.y * pixels_per_cell;

        ear.setPosition(base_x + pixels_per_cell * 0.18f, base_y + pixels_per_cell * 0.02f);
        window.draw(ear);
        ear.setPosition(base_x + pixels_per_cell * 0.52f, base_y + pixels_per_cell * 0.02f);
        window.draw(ear);

        rabbit_shape.setPosition(base_x + pixels_per_cell * 0.18f, base_y + pixels_per_cell * 0.22f);
        window.draw(rabbit_shape);
    }
}

void GraphicsView::Impl::draw_preview(const GameModel& model) {
    const float title_height = 3.f * pixels_per_cell;

    sf::RectangleShape title_box({static_cast<float>(window.getSize().x), title_height});
    title_box.setPosition(0.f, 0.f);
    title_box.setFillColor(sf::Color(32, 44, 60));
    window.draw(title_box);

    sf::RectangleShape field_frame({field_width * pixels_per_cell, field_height * pixels_per_cell});
    field_frame.setPosition(0.f, title_height);
    field_frame.setFillColor(sf::Color(14, 18, 24));
    field_frame.setOutlineThickness(2.f);
    field_frame.setOutlineColor(sf::Color(110, 125, 150));
    window.draw(field_frame);
}

bool GraphicsView::Impl::process_window_events(bool return_on_exit) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            return false;
        }

        if (return_on_exit && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Q) {
            window.close();
            return false;
        }
    }
    return true;
}

void GraphicsView::Impl::draw_scoreboard_screen(const std::vector<ScoreboardEntry>& entries,
                                                std::size_t current_round,
                                                std::size_t total_rounds,
                                                bool final_screen) {
    window.clear(sf::Color(12, 18, 28));

    const auto window_size = window.getSize();
    const sf::Vector2f panel_size{window_size.x * 0.68f, window_size.y * 0.70f};
    const sf::Vector2f panel_pos{(window_size.x - panel_size.x) * 0.5f,
                                 (window_size.y - panel_size.y) * 0.5f};

    sf::RectangleShape shadow(panel_size + sf::Vector2f(12.f, 12.f));
    shadow.setPosition(panel_pos.x + 10.f, panel_pos.y + 12.f);
    shadow.setFillColor(sf::Color(0, 0, 0, 120));
    window.draw(shadow);

    sf::RectangleShape panel(panel_size);
    panel.setPosition(panel_pos);
    panel.setFillColor(sf::Color(34, 43, 58));
    panel.setOutlineThickness(3.f);
    panel.setOutlineColor(sf::Color(110, 130, 165));
    window.draw(panel);

    sf::RectangleShape header({panel_size.x, 80.f});
    header.setPosition(panel_pos);
    header.setFillColor(sf::Color(46, 64, 92));
    window.draw(header);

    float y = panel_pos.y + 140.f;
    std::size_t place = 1;
    for (const auto& entry : entries) {
        sf::RectangleShape row({panel_size.x - 50.f, 62.f});
        row.setPosition(panel_pos.x + 25.f, y);
        row.setFillColor(place % 2 == 1 ? sf::Color(41, 52, 70) : sf::Color(37, 47, 63));
        row.setOutlineThickness(1.5f);
        row.setOutlineColor(sf::Color(74, 89, 116));
        window.draw(row);

        sf::CircleShape bullet(11.f);
        bullet.setFillColor(convert_to_sfml_color(entry.color));
        bullet.setPosition(panel_pos.x + 42.f, y + 19.f);
        window.draw(bullet);

        y += 74.f;
        ++place;
    }

    window.display();
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
                case sf::Keyboard::Up: return Event{KeyEvents::up_2};
                case sf::Keyboard::Down: return Event{KeyEvents::down_2};
                case sf::Keyboard::Left: return Event{KeyEvents::left_2};
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