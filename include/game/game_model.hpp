#pragma once

#include <random>
#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <list>
#include <vector>
#include <unordered_map>
#include "entities/snake.hpp"
#include "entities/rabbit.hpp"

namespace snakes {

struct GameModel {

    uint32_t width;
    uint32_t height;

    Point start_point{2, 2};
    
    std::list<Snake>  snakes{};
    std::list<Rabbit> rabbits{}; 
    std::vector<Snake> human_snakes{};

    // TODO: builder with insertion human_snakes
    bool game_over{false};
    int score{0};

    GameModel(uint32_t w = 80, uint32_t h = 20) : width(w), height(h) {}

    static constexpr std::size_t max_rabbits = 3;

    void update(); // split to steps

private: 
    std::vector<Point> generate_rabbits_coords() const;
    void update_rabbits();
    void update_snakes();

    std::vector<Snake> check_collisions() const;
    void remove_crashed_snakes();

};

// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
inline void GameModel::update() {
    if (game_over) return;

    update_snakes();
    update_rabbits();
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

} // namespace snakes