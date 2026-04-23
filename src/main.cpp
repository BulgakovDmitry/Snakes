#include "game/game.hpp"
#include "view/view_ascii.hpp"
#include "view/view_graphics.hpp"
#include "entities/snake.hpp"

#include <CLI/CLI.hpp>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

struct SnakeSetup {
    int color;
    bool human_controlled;
    snakes::Point spawn_point;
};

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

std::string color_name(int color) {
    switch (color) {
        case fg_red: return "Red snake";
        case fg_green: return "Green snake";
        case fg_blue: return "Blue snake";
        case fg_cyan: return "Cyan snake";
        case fg_magenta: return "Magenta snake";
        case fg_yellow: return "Yellow snake";
        case fg_bright_red: return "Bright red snake";
        case fg_bright_green: return "Bright green snake";
        case fg_bright_yellow: return "Bright yellow snake";
        case fg_bright_blue: return "Bright blue snake";
        case fg_bright_magenta: return "Bright magenta snake";
        case fg_bright_cyan: return "Bright cyan snake";
        default: return "Snake";
    }
}

std::vector<SnakeSetup> build_snake_setups(std::uint32_t width,
                                           std::uint32_t height,
                                           std::size_t num_humans,
                                           std::size_t num_bots) {
    const std::size_t total_snakes = num_humans + num_bots;
    const auto spawn_points = make_spawn_points(width, height, total_snakes);

    std::vector<SnakeSetup> setups;
    setups.reserve(total_snakes);

    for (std::size_t i = 0; i < num_humans; ++i) {
        setups.push_back(SnakeSetup{pick_color(i, true), true, spawn_points[i]});
    }

    for (std::size_t i = 0; i < num_bots; ++i) {
        setups.push_back(SnakeSetup{pick_color(i, false), false, spawn_points[num_humans + i]});
    }

    return setups;
}

snakes::GameModel create_model(std::uint32_t width,
                               std::uint32_t height,
                               std::size_t num_rabbits,
                               const std::vector<SnakeSetup>& snake_setups) {
    snakes::GameModel model{};
    model.width = width;
    model.height = height;
    model.max_rabbits = num_rabbits;

    for (const auto& setup : snake_setups) {
        model.snakes.push_back(
            snakes::Snake::Builder()
                .set_color(setup.color)
                .set_direction(snakes::Direction::right)
                .set_spawn_point(setup.spawn_point)
                .set_human_controlled(setup.human_controlled)
                .build()
        );
    }

    return model;
}

std::vector<snakes::ScoreboardEntry> build_scoreboard_entries(
    const std::vector<SnakeSetup>& snake_setups,
    const std::unordered_map<int, std::size_t>& wins) {

    std::vector<snakes::ScoreboardEntry> entries;
    entries.reserve(snake_setups.size());

    for (const auto& snake : snake_setups) {
        const auto it = wins.find(snake.color);

        entries.push_back(snakes::ScoreboardEntry{
            color_name(snake.color),
            snake.color,
            (it != wins.end()) ? it->second : 0
        });
    }

    std::sort(entries.begin(), entries.end(), [](const auto& lhs, const auto& rhs) {
        if (lhs.score != rhs.score) {
            return lhs.score > rhs.score;
        }
        return lhs.label < rhs.label;
    });

    return entries;
}

bool has_tournament_winner(const std::unordered_map<int, std::size_t>& wins,
                           std::size_t target_wins) {
    for (const auto& [color, score] : wins) {
        (void)color;
        if (score >= target_wins) {
            return true;
        }
    }
    return false;
}

int run_tournament(std::uint32_t width,
                   std::uint32_t height,
                   std::size_t num_rabbits,
                   const std::vector<SnakeSetup>& snake_setups,
                   snakes::IView& view) {
    constexpr std::size_t target_wins = 3;
    std::unordered_map<int, std::size_t> wins;

    for (const auto& snake : snake_setups) {
        wins[snake.color] = 0;
    }

    std::size_t round = 1;
    while (!has_tournament_winner(wins, target_wins)) {
        const auto entries_before_round = build_scoreboard_entries(snake_setups, wins);
        if (!view.show_scoreboard(entries_before_round,
                                  round,
                                  target_wins,
                                  false,
                                  std::chrono::seconds(3))) {
            return 0;
        }

        auto model = create_model(width, height, num_rabbits, snake_setups);
        snakes::Game game(model, view);
        const snakes::RoundResult result = game.run_round();

        if (result.exit_requested) {
            return 0;
        }

        if (result.winner_color.has_value()) {
            ++wins[*result.winner_color];
        }

        ++round;
    }

    const auto final_entries = build_scoreboard_entries(snake_setups, wins);
    view.show_scoreboard(final_entries,
                         round - 1,
                         target_wins,
                         true,
                         std::chrono::seconds(3));
    return 0;
}

} // namespace

int main(int argc, char** argv) {
    CLI::App arg_parser{"Snake game"};

    bool graphics = true;
    bool tournament = false;
    std::uint32_t width = 60;
    std::uint32_t height = 30;
    std::size_t num_humans = 2;
    std::size_t num_bots = 3;
    std::size_t num_rabbits = 5;

    arg_parser.add_option("--graphics", graphics,
                          "Use graphics mode (true/false)")
        ->default_val(true);

    arg_parser.add_option("--tournament", tournament,
                          "Run tournament mode (true/false)")
        ->default_val(false);

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

    const auto snake_setups = build_snake_setups(width, height, num_humans, num_bots);

    std::unique_ptr<snakes::IView> view;
    if (graphics) {
        view = std::make_unique<snakes::GraphicsView>(width, height);
    } else {
        view = std::make_unique<snakes::AsciiView>();
    }

    if (tournament) {
        return run_tournament(width, height, num_rabbits, snake_setups, *view);
    }

    auto model = create_model(width, height, num_rabbits, snake_setups);
    snakes::Game game(model, *view);
    game.run();

    return 0;
}