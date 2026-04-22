#include "game/game.hpp"
#include "view/view_ascii.hpp"
#include "view/view_graphics.hpp"
#include "entities/snake.hpp"

#include <CLI/CLI.hpp>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <vector>

namespace {

std::vector<snakes::Point> make_spawn_points(std::uint32_t field_width,
                                             std::uint32_t field_height,
                                             std::size_t count) {
    std::vector<snakes::Point> points;
    points.reserve(count);

    const int32_t min_x = 4;
    const int32_t min_y = 6;

    const int32_t max_x = std::max<int32_t>(min_x, static_cast<int32_t>(field_width) - 6);
    const int32_t max_y = std::max<int32_t>(min_y, static_cast<int32_t>(field_height) - 2);

    const int32_t step_x = std::max<int32_t>(8, static_cast<int32_t>(field_width) / 4);
    const int32_t step_y = std::max<int32_t>(6, static_cast<int32_t>(field_height) / 3);

    for (int32_t y = min_y; y <= max_y && points.size() < count; y += step_y) {
        for (int32_t x = min_x; x <= max_x && points.size() < count; x += step_x) {
            points.push_back({x, y});
        }
    }

    for (int32_t y = min_y; points.size() < count; ++y) {
        for (int32_t x = min_x; points.size() < count; x += 4) {
            points.push_back({x, y});
        }
    }

    return points;
}

int pick_color(std::size_t index, bool human) {
    static const std::vector<int> human_colors{
        fg_red,
        fg_blue,
        fg_cyan,
        fg_green
    };

    static const std::vector<int> bot_colors{
        fg_magenta,
        fg_bright_yellow,
        fg_bright_green,
        fg_bright_cyan,
        fg_bright_red
    };

    const auto& palette = human ? human_colors : bot_colors;
    return palette[index % palette.size()];
}

} // namespace

int main(int argc, char** argv) {
    CLI::App arg_parser{"Snake game"};

    bool graphics = true;
    std::uint32_t width = 60;
    std::uint32_t height = 30;
    std::size_t num_humans = 2;
    std::size_t num_bots = 3;
    std::size_t num_rabbits = 5;

    arg_parser.add_option("--graphics", graphics,
                          "Use graphics mode (true/false)")
        ->default_val(true);

    arg_parser.add_option("--width", width,
                          "Field width in cells")
        ->default_val(60);

    arg_parser.add_option("--height", height,
                          "Field height in cells")
        ->default_val(30);

    arg_parser.add_option("--num_humans", num_humans,
                          "Number of human-controlled snakes")
        ->default_val(2);

    arg_parser.add_option("--num_bots", num_bots,
                          "Number of bot snakes")
        ->default_val(3);

    arg_parser.add_option("--num_rabbits", num_rabbits,
                          "Max number of rabbits on the field")
        ->default_val(5);

    CLI11_PARSE(arg_parser, argc, argv);

    width = std::max<std::uint32_t>(20, width);
    height = std::max<std::uint32_t>(10, height);

    snakes::GameModel model{};
    model.width = width;
    model.height = height;
    model.max_rabbits = num_rabbits;

    const std::size_t total_snakes = num_humans + num_bots;
    const auto spawn_points = make_spawn_points(width, height, total_snakes);

    for (std::size_t i = 0; i < num_humans; ++i) {
        model.snakes.push_back(
            snakes::Snake::Builder()
                .set_color(pick_color(i, true))
                .set_direction(snakes::Direction::right)
                .set_spawn_point(spawn_points[i])
                .set_human_controlled(true)
                .build()
        );
    }

    for (std::size_t i = 0; i < num_bots; ++i) {
        model.snakes.push_back(
            snakes::Snake::Builder()
                .set_color(pick_color(i, false))
                .set_direction(snakes::Direction::right)
                .set_spawn_point(spawn_points[num_humans + i])
                .set_human_controlled(false)
                .build()
        );
    }

    std::unique_ptr<snakes::IView> view;
    
    if (graphics) {
        view = std::make_unique<snakes::GraphicsView>(width, height);
    } else {
        view = std::make_unique<snakes::AsciiView>();
    }

    snakes::Game game(model, *view);
    game.run();

    return 0;
}