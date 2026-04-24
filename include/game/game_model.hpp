#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <list>
#include <optional>
#include <random>
#include <sys/ioctl.h>
#include <unistd.h>
#include <unordered_map>
#include <utility>
#include <vector>

#include "entities/health.hpp"
#include "entities/rabbit.hpp"
#include "entities/snake.hpp"
#include "core/bot.hpp"

namespace snakes {

struct GameModel {
    uint32_t width{60};
    uint32_t height{20};

    Point start_point{0, 0};

    std::list<Snake> snakes{};
    std::list<Rabbit> rabbits{};
    std::list<Health> health_items{};

    std::vector<Snake*> human_snakes{};

    GameModel() = default;

    std::size_t max_rabbits{5};
    std::size_t max_health_items{1};
    double health_spawn_chance{0.001};

    void update();
    void rebuild_human_snakes();

private:
    std::vector<Point> generate_rabbits_coords() const;
    std::optional<Point> generate_health_coord() const;
    std::vector<Point> collect_free_cells() const;
    bool is_play_cell(Point point) const noexcept;
    bool is_occupied(Point point) const;
    bool is_respawn_area_free(const Snake& snake, Point spawn_point) const;
    std::optional<Point> find_respawn_point(const Snake& snake) const;

    void update_rabbits();
    void update_health_items();
    void update_snakes();

    std::vector<std::list<Snake>::iterator> check_collisions();
    void remove_crashed_snakes();

    std::pair<bool, std::list<Rabbit>::iterator> has_eaten_rabbit(const Snake& snake);
    void eat_rabbit(Snake& snake, std::list<Rabbit>::iterator rabbit_it);
    void try_eat_rabbit();

    std::pair<bool, std::list<Health>::iterator> has_eaten_health(const Snake& snake);
    void eat_health(Snake& snake, std::list<Health>::iterator health_it);
    void try_eat_health();

    void update_bot_snakes();
};

// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
inline void GameModel::update() {
    update_bot_snakes();
    update_snakes();
    try_eat_rabbit();
    try_eat_health();
    remove_crashed_snakes();
    update_rabbits();
    update_health_items();
}

inline void GameModel::rebuild_human_snakes() {
    human_snakes.clear();
    for (Snake& snake : snakes) {
        if (snake.is_human_controlled()) {
            human_snakes.push_back(std::addressof(snake));
        }
    }
}

inline void GameModel::update_rabbits() {
    const std::vector<Point> new_coords = generate_rabbits_coords();

    for (const Point& pos : new_coords) {
        rabbits.emplace_back(pos);
    }
}

inline void GameModel::update_health_items() {
    if (health_items.size() >= max_health_items) {
        return;
    }

    static std::mt19937 gen(std::random_device{}());
    std::bernoulli_distribution should_spawn(health_spawn_chance);

    if (!should_spawn(gen)) {
        return;
    }

    if (const auto pos = generate_health_coord(); pos.has_value()) {
        health_items.emplace_back(*pos);
    }
}

inline void GameModel::update_snakes() {
    for (Snake& snake : snakes) {
        snake.move();
    }
}

inline bool GameModel::is_play_cell(Point point) const noexcept {
    const Point frame_start{start_point.x, start_point.y + 3};

    const int32_t play_left   = frame_start.x + 1;
    const int32_t play_top    = frame_start.y + 1;
    const int32_t play_right  = play_left + static_cast<int32_t>(width)  - 1;
    const int32_t play_bottom = play_top  + static_cast<int32_t>(height) - 1;

    return point.x >= play_left && point.x <= play_right &&
           point.y >= play_top  && point.y <= play_bottom;
}

inline bool GameModel::is_occupied(Point point) const {
    for (const Snake& snake : snakes) {
        if (snake.located_on(point)) {
            return true;
        }
    }

    for (const Rabbit& rabbit : rabbits) {
        if (rabbit.position() == point) {
            return true;
        }
    }

    for (const Health& health : health_items) {
        if (health.position() == point) {
            return true;
        }
    }

    return false;
}

inline std::vector<Point> GameModel::collect_free_cells() const {
    std::vector<Point> free_cells;

    const Point frame_start{start_point.x, start_point.y + 3};
    const int32_t play_left   = frame_start.x + 1;
    const int32_t play_top    = frame_start.y + 1;
    const int32_t play_right  = play_left + static_cast<int32_t>(width)  - 1;
    const int32_t play_bottom = play_top  + static_cast<int32_t>(height) - 1;

    for (int32_t y = play_top; y <= play_bottom; ++y) {
        for (int32_t x = play_left; x <= play_right; ++x) {
            Point candidate{x, y};
            if (!is_occupied(candidate)) {
                free_cells.push_back(candidate);
            }
        }
    }

    return free_cells;
}

inline std::vector<Point> GameModel::generate_rabbits_coords() const {
    if (rabbits.size() >= max_rabbits) {
        return {};
    }

    std::vector<Point> free_cells = collect_free_cells();
    if (free_cells.empty()) {
        return {};
    }

    static std::mt19937 gen(std::random_device{}());
    std::shuffle(free_cells.begin(), free_cells.end(), gen);

    const std::size_t need =
        std::min(max_rabbits - rabbits.size(), free_cells.size());

    return std::vector<Point>(free_cells.begin(), free_cells.begin() + need);
}

inline std::optional<Point> GameModel::generate_health_coord() const {
    std::vector<Point> free_cells = collect_free_cells();
    if (free_cells.empty()) {
        return std::nullopt;
    }

    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<std::size_t> dist(0, free_cells.size() - 1);
    return free_cells[dist(gen)];
}

inline bool GameModel::is_respawn_area_free(const Snake& snake, Point spawn_point) const {
    const Point body_points[] = {
        spawn_point,
        spawn_point + Point{1, 0},
        spawn_point + Point{2, 0}
    };

    for (const Point point : body_points) {
        if (!is_play_cell(point)) {
            return false;
        }

        for (const Snake& other : snakes) {
            if (&other == &snake) {
                continue;
            }
            if (other.located_on(point)) {
                return false;
            }
        }

        for (const Rabbit& rabbit : rabbits) {
            if (rabbit.position() == point) {
                return false;
            }
        }

        for (const Health& health : health_items) {
            if (health.position() == point) {
                return false;
            }
        }
    }

    return true;
}

inline std::optional<Point> GameModel::find_respawn_point(const Snake& snake) const {
    if (is_respawn_area_free(snake, snake.spawn_point())) {
        return snake.spawn_point();
    }

    std::vector<Point> free_cells = collect_free_cells();
    static std::mt19937 gen(std::random_device{}());
    std::shuffle(free_cells.begin(), free_cells.end(), gen);

    for (const Point candidate : free_cells) {
        if (is_respawn_area_free(snake, candidate)) {
            return candidate;
        }
    }

    return std::nullopt;
}

inline std::vector<std::list<Snake>::iterator> GameModel::check_collisions() {
    std::vector<std::list<Snake>::iterator> crashed_snakes;

    for (auto it1 = snakes.begin(); it1 != snakes.end(); ++it1) {
        const Point head = it1->head();
        bool crashed = false;

        if (!is_play_cell(head)) {
            crashed = true;
        }

        if (!crashed) {
            for (std::size_t i = 1; i < it1->body().size(); ++i) {
                if (it1->body()[i] == head) {
                    crashed = true;
                    break;
                }
            }
        }

        if (!crashed) {
            for (auto it2 = snakes.begin(); it2 != snakes.end(); ++it2) {
                if (it1 == it2) {
                    continue;
                }

                for (const Point& segment : it2->body()) {
                    if (segment == head) {
                        crashed = true;
                        break;
                    }
                }

                if (crashed) {
                    break;
                }
            }
        }

        if (crashed) {
            crashed_snakes.push_back(it1);
        }
    }

    return crashed_snakes;
}

inline void GameModel::remove_crashed_snakes() {
    const auto crashed_snakes = check_collisions();

    for (const auto& it : crashed_snakes) {
        if (it->lose_life()) {
            if (const auto respawn_point = find_respawn_point(*it); respawn_point.has_value()) {
                it->respawn(*respawn_point);
            } else {
                snakes.erase(it);
            }
        } else {
            snakes.erase(it);
        }
    }

    if (!crashed_snakes.empty()) {
        rebuild_human_snakes();
    }
}

inline std::pair<bool, std::list<Rabbit>::iterator> GameModel::has_eaten_rabbit(const Snake& snake) {
    for (auto rabbit_it = rabbits.begin(); rabbit_it != rabbits.end(); ++rabbit_it) {
        if (rabbit_it->position() == snake.head()) {
            return {true, rabbit_it};
        }
    }
    return {false, rabbits.end()};
}

inline void GameModel::eat_rabbit(Snake& snake, std::list<Rabbit>::iterator rabbit_it) {
    if (rabbit_it == rabbits.end()) {
        return;
    }

    snake.body().push_back(snake.body().back());
    rabbits.erase(rabbit_it);
}

inline void GameModel::try_eat_rabbit() {
    for (Snake& snake : snakes) {
        auto [eaten, rabbit_it] = has_eaten_rabbit(snake);
        if (eaten) {
            eat_rabbit(snake, rabbit_it);
        }
    }
}

inline std::pair<bool, std::list<Health>::iterator> GameModel::has_eaten_health(const Snake& snake) {
    for (auto health_it = health_items.begin(); health_it != health_items.end(); ++health_it) {
        if (health_it->position() == snake.head()) {
            return {true, health_it};
        }
    }
    return {false, health_items.end()};
}

inline void GameModel::eat_health(Snake& snake, std::list<Health>::iterator health_it) {
    if (health_it == health_items.end()) {
        return;
    }

    snake.add_life();
    health_items.erase(health_it);
}

inline void GameModel::try_eat_health() {
    for (Snake& snake : snakes) {
        auto [eaten, health_it] = has_eaten_health(snake);
        if (eaten) {
            eat_health(snake, health_it);
        }
    }
}

inline void GameModel::update_bot_snakes() {
    for (Snake& snake : snakes) {
        if (!snake.is_human_controlled()) {
            snake.set_direction(choose_bot_direction(*this, snake));
        }
    }
}

} // namespace snakes