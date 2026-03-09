#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include "entities/snake.hpp"
#include "entities/rabbit.hpp"

namespace snakes {

struct GameModel {
    uint32_t width{20};
    uint32_t height{10};

    std::list<Snake> snakes{};
    std::list<Rabbit> rabbits{};

    bool game_over{false};
    int score{0};

    GameModel(uint32_t w = 20, uint32_t h = 10) : width(w), height(h) {}
};

} // namespace snakes