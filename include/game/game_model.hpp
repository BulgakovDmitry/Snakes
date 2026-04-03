#pragma once

#include <random>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <sys/ioctl.h>
#include <unistd.h>
#include <utility>
#include <list>
#include <vector>
#include <functional>
#include <unordered_map>
#include "entities/snake.hpp"
#include "entities/rabbit.hpp"

namespace snakes {

struct GameModel {

    uint32_t width;
    uint32_t height;

    Point start_point{0, 0};
    
    std::list<Snake>  snakes{};
    std::list<Rabbit> rabbits{};
    std::vector<std::reference_wrapper<Snake>> human_snakes{};


    // TODO: builder with insertion human_snakes
    int score{0};

    //GameModel(uint32_t w = 120, uint32_t h = 30) : width(w), height(h) {}
    GameModel() = default;

    static constexpr std::size_t max_rabbits = 10;

    void update(); // split to steps

private: 
    std::vector<Point> generate_rabbits_coords() const;
    void update_rabbits();
    void update_snakes();

    std::vector<std::list<Snake>::const_iterator> check_collisions() const;
    void remove_crashed_snakes();

    std::pair<bool, std::list<Rabbit>::const_iterator> has_eaten_rabbit(const Snake& snake) const;
    void eat_rabbit(Snake& snake, std::list<Rabbit>::const_iterator rabbit_it);
    void try_eat_rabbit();

    void update_terminal_size();
};

// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
inline void GameModel::update() {
    update_terminal_size();
    update_snakes();
    update_rabbits();
    remove_crashed_snakes();
    try_eat_rabbit();
}

inline void GameModel::update_rabbits() {
    const std::vector<Point> new_coords = generate_rabbits_coords();

    for (const Point& pos : new_coords) {
        rabbits.emplace_back(pos);
    }
}

inline void GameModel::update_snakes() {
    for (Snake& snake : snakes) {
        snake.move();
    }
}

inline std::vector<Point> GameModel::generate_rabbits_coords() const {
    if (rabbits.size() >= max_rabbits) {
        return {};
    }

    std::vector<Point> free_cells;

    const Point frame_start{
        start_point.x,
        start_point.y + 3
    };

    const int32_t play_left   = frame_start.x + 1;
    const int32_t play_top    = frame_start.y + 1;
    const int32_t play_right  = play_left + static_cast<int32_t>(width)  - 1;
    const int32_t play_bottom = play_top  + static_cast<int32_t>(height) - 1;

    for (int32_t y = play_top; y <= play_bottom; ++y) {
        for (int32_t x = play_left; x <= play_right; ++x) {
            Point candidate{x, y};

            bool occupied = false;

            for (const Snake& snake : snakes) {
                if (snake.located_on(candidate)) {
                    occupied = true;
                    break;
                }
            }

            if (occupied) {
                continue;
            }

            for (const Rabbit& rabbit : rabbits) {
                if (rabbit.position() == candidate) {
                    occupied = true;
                    break;
                }
            }

            if (!occupied) {
                free_cells.push_back(candidate);
            }
        }
    }

    if (free_cells.empty()) {
        return {};
    }

    static std::mt19937 gen(std::random_device{}());
    std::shuffle(free_cells.begin(), free_cells.end(), gen);

    const std::size_t need =
        std::min(max_rabbits - rabbits.size(), free_cells.size());

    return std::vector<Point>(free_cells.begin(), free_cells.begin() + need);
}

inline std::vector<std::list<Snake>::const_iterator> GameModel::check_collisions() const {
    std::vector<std::list<Snake>::const_iterator> crashed_snakes;

    for (auto it1 = snakes.begin(); it1 != snakes.end(); ++it1) {
        const Point head = it1->head();

        // Check collision with walls
        Point start = start_point + Point{0, 3};
        if (head.x < start.x + 1 || head.x > start.x + width ||
            head.y < start.y + 1 || head.y > start.y + height) {
            crashed_snakes.push_back(it1);
            continue;
        }

        // // Check collision with itself
        // for (std::size_t i = 1; i < it1->body().size(); ++i) {
        //     if (it1->body()[i] == head) {
        //         crashed_snakes.push_back(it1);
        //         break;
        //     }
        // }

        // Check collision with other snakes
        for (auto it2 = snakes.begin(); it2 != snakes.end(); ++it2) {
            if (it1 == it2) {
                continue;
            }
            for (const Point& segment : it2->body()) {
                if (segment == head) {
                    crashed_snakes.push_back(it1);
                    break;
                }
            }
        }
    }
    return crashed_snakes;
}

inline void GameModel::remove_crashed_snakes() {
    const auto crashed_snakes = check_collisions();

    for (const auto& it : crashed_snakes) {
        snakes.erase(it);
    }
}

inline std::pair<bool, std::list<Rabbit>::const_iterator> GameModel::has_eaten_rabbit(const Snake& snake) const {
    for (auto rabbit_it = rabbits.begin(); rabbit_it != rabbits.end(); ++rabbit_it) {
        if (rabbit_it->position() == snake.head()) {
            return {true, rabbit_it};
        }
    }
    return {false, rabbits.end()};
}

inline void GameModel::eat_rabbit(Snake& snake, std::list<Rabbit>::const_iterator rabbit_it) {
    if (rabbit_it == rabbits.cend()) {
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

inline void GameModel::update_terminal_size() {
    winsize ws{};

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) {
        width = 60;
        height = 20;
        return;
    }

    const int32_t usable_w =
        static_cast<int32_t>(ws.ws_col) - static_cast<int32_t>(start_point.x) - 2;

    const int32_t usable_h =
        static_cast<int32_t>(ws.ws_row) - static_cast<int32_t>(start_point.y) - 5;

    width = usable_w > 20 ? static_cast<uint32_t>(usable_w) : 20;
    height = usable_h > 10 ? static_cast<uint32_t>(usable_h) : 10;
}

} // namespace snakes