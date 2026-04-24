#include "view/view_graphics.hpp"

#include <SFML/Graphics.hpp>

#include <algorithm>
#include <array>
#include <chrono>
#include <cstddef>
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
    bool font_loaded{false};

    uint32_t field_width{0};
    uint32_t field_height{0};
    float pixels_per_cell{20.f};

    Impl(uint32_t field_w, uint32_t field_h);

    void draw_snakes(const GameModel& model);
    void draw_rabbits(const GameModel& model);
    void draw_health_items(const GameModel& model);
    void draw_preview(const GameModel& model);

    void draw_heart(sf::Vector2f center, float size, sf::Color color);
    void draw_lives_preview(const GameModel& model);

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

    font_loaded = font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf");
}

void GraphicsView::render(GameModel& model) {
    if (!impl_->window.isOpen()) {
        return;
    }

    model.width = impl_->field_width;
    model.height = impl_->field_height;

    impl_->window.clear(sf::Color(18, 24, 32));

    impl_->draw_preview(model);
    impl_->draw_health_items(model);
    impl_->draw_rabbits(model);
    impl_->draw_snakes(model);

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

        ear.setPosition(base_x + pixels_per_cell * 0.18f,
                        base_y + pixels_per_cell * 0.02f);
        window.draw(ear);

        ear.setPosition(base_x + pixels_per_cell * 0.52f,
                        base_y + pixels_per_cell * 0.02f);
        window.draw(ear);

        rabbit_shape.setPosition(base_x + pixels_per_cell * 0.18f,
                                 base_y + pixels_per_cell * 0.22f);
        window.draw(rabbit_shape);
    }
}

void GraphicsView::Impl::draw_health_items(const GameModel& model) {
    for (const Health& health : model.health_items) {
        const Point p = health.position();

        const float center_x = p.x * pixels_per_cell + pixels_per_cell * 0.5f;
        const float center_y = p.y * pixels_per_cell + pixels_per_cell * 0.45f;

        draw_heart(
            {center_x, center_y},
            pixels_per_cell * 0.85f,
            sf::Color(230, 45, 75)
        );
    }
}

void GraphicsView::Impl::draw_heart(sf::Vector2f center, float size, sf::Color color) {
    const float radius = size * 0.28f;

    sf::CircleShape left_part(radius);
    left_part.setFillColor(color);
    left_part.setOrigin(radius, radius);
    left_part.setPosition(center.x - radius * 0.75f,
                          center.y - radius * 0.25f);
    window.draw(left_part);

    sf::CircleShape right_part(radius);
    right_part.setFillColor(color);
    right_part.setOrigin(radius, radius);
    right_part.setPosition(center.x + radius * 0.75f,
                           center.y - radius * 0.25f);
    window.draw(right_part);

    sf::ConvexShape bottom_part;
    bottom_part.setPointCount(3);
    bottom_part.setPoint(0, {center.x - size * 0.55f,
                             center.y - radius * 0.15f});
    bottom_part.setPoint(1, {center.x + size * 0.55f,
                             center.y - radius * 0.15f});
    bottom_part.setPoint(2, {center.x,
                             center.y + size * 0.60f});
    bottom_part.setFillColor(color);
    window.draw(bottom_part);
}

void GraphicsView::Impl::draw_lives_preview(const GameModel& model) {
    const float preview_height = 3.f * pixels_per_cell;

    float x = 14.f;
    float y = 8.f;

    const float marker_size = 10.f;
    const float heart_size = 14.f;
    const float heart_gap = 4.f;
    const float snake_gap = 26.f;
    const float row_height = 24.f;

    const float right_limit = static_cast<float>(window.getSize().x) - 14.f;

    for (const Snake& snake : model.snakes) {
        const float block_width =
            marker_size + 10.f +
            static_cast<float>(snake.lives()) * (heart_size + heart_gap) +
            snake_gap;

        if (x + block_width > right_limit) {
            x = 14.f;
            y += row_height;
        }

        if (y + heart_size > preview_height - 4.f) {
            break;
        }

        sf::RectangleShape snake_marker({marker_size, marker_size});
        snake_marker.setPosition(x, y + 4.f);
        snake_marker.setFillColor(convert_to_sfml_color(snake.color()));
        snake_marker.setOutlineThickness(1.f);
        snake_marker.setOutlineColor(sf::Color(220, 220, 220));
        window.draw(snake_marker);

        x += marker_size + 10.f;

        for (std::size_t i = 0; i < snake.lives(); ++i) {
            draw_heart(
                {x + heart_size * 0.5f, y + heart_size * 0.55f},
                heart_size,
                sf::Color(230, 45, 75)
            );

            x += heart_size + heart_gap;
        }

        x += snake_gap;
    }
}

void GraphicsView::Impl::draw_preview(const GameModel& model) {
    const float title_height = 3.f * pixels_per_cell;

    sf::RectangleShape title_box({static_cast<float>(window.getSize().x), title_height});
    title_box.setPosition(0.f, 0.f);
    title_box.setFillColor(sf::Color(32, 44, 60));
    window.draw(title_box);

    draw_lives_preview(model);

    sf::RectangleShape field_frame({field_width * pixels_per_cell,
                                    field_height * pixels_per_cell});
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

        if (return_on_exit &&
            event.type == sf::Event::KeyPressed &&
            event.key.code == sf::Keyboard::Q) {
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
    const sf::Vector2f panel_size{window_size.x * 0.72f,
                                  window_size.y * 0.76f};
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

    sf::RectangleShape header({panel_size.x, 86.f});
    header.setPosition(panel_pos);
    header.setFillColor(sf::Color(46, 64, 92));
    window.draw(header);

    if (font_loaded) {
        sf::Text title;
        title.setFont(font);
        title.setCharacterSize(30);
        title.setFillColor(sf::Color::White);
        title.setString(final_screen ? "Tournament finished" : "Tournament scoreboard");
        title.setPosition(panel_pos.x + 28.f, panel_pos.y + 14.f);
        window.draw(title);

        sf::Text subtitle;
        subtitle.setFont(font);
        subtitle.setCharacterSize(18);
        subtitle.setFillColor(sf::Color(210, 220, 235));

        if (final_screen) {
            subtitle.setString("Final result. Target score: " + std::to_string(total_rounds));
        } else {
            subtitle.setString("Round " + std::to_string(current_round) +
                               ". First to " + std::to_string(total_rounds) + " wins.");
        }

        subtitle.setPosition(panel_pos.x + 30.f, panel_pos.y + 54.f);
        window.draw(subtitle);
    }

    float y = panel_pos.y + 120.f;
    std::size_t place = 1;

    for (const auto& entry : entries) {
        sf::RectangleShape row({panel_size.x - 50.f, 62.f});
        row.setPosition(panel_pos.x + 25.f, y);
        row.setFillColor(place % 2 == 1
            ? sf::Color(41, 52, 70)
            : sf::Color(37, 47, 63));
        row.setOutlineThickness(1.5f);
        row.setOutlineColor(sf::Color(74, 89, 116));
        window.draw(row);

        sf::CircleShape bullet(11.f);
        bullet.setFillColor(convert_to_sfml_color(entry.color));
        bullet.setPosition(panel_pos.x + 42.f, y + 20.f);
        window.draw(bullet);

        if (font_loaded) {
            sf::Text place_text;
            place_text.setFont(font);
            place_text.setCharacterSize(20);
            place_text.setFillColor(sf::Color(210, 220, 235));
            place_text.setString(std::to_string(place) + ".");
            place_text.setPosition(panel_pos.x + 72.f, y + 18.f);
            window.draw(place_text);

            sf::Text name_text;
            name_text.setFont(font);
            name_text.setCharacterSize(20);
            name_text.setFillColor(sf::Color::White);
            name_text.setString(entry.label);
            name_text.setPosition(panel_pos.x + 115.f, y + 18.f);
            window.draw(name_text);

            sf::Text score_text;
            score_text.setFont(font);
            score_text.setCharacterSize(22);
            score_text.setFillColor(sf::Color(255, 235, 130));
            score_text.setString("Score: " + std::to_string(entry.score));
            score_text.setPosition(panel_pos.x + panel_size.x - 180.f, y + 17.f);
            window.draw(score_text);
        } 

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
                case sf::Keyboard::W:
                    return Event{KeyEvents::up_1};
                case sf::Keyboard::S:
                    return Event{KeyEvents::down_1};
                case sf::Keyboard::A:
                    return Event{KeyEvents::left_1};
                case sf::Keyboard::D:
                    return Event{KeyEvents::right_1};

                case sf::Keyboard::Up:
                    return Event{KeyEvents::up_2};
                case sf::Keyboard::Down:
                    return Event{KeyEvents::down_2};
                case sf::Keyboard::Left:
                    return Event{KeyEvents::left_2};
                case sf::Keyboard::Right:
                    return Event{KeyEvents::right_2};

                case sf::Keyboard::Q:
                    return Event{KeyEvents::exit};
                case sf::Keyboard::P:
                    return Event{KeyEvents::pause};
                case sf::Keyboard::R:
                    return Event{KeyEvents::restart};

                default:
                    break;
            }
        }
    }

    return std::nullopt;
}

} // namespace snakes