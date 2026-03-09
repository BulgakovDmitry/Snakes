#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include "entities/snake.hpp"
#include "entities/rabbit.hpp"

namespace snakes {

struct GameModel {
    uint32_t width;
    uint32_t height;

    std::list<Snake> snakes{};
    std::list<Rabbit> rabbits{};

    bool game_over{false};
    int score{0};

    GameModel(uint32_t w = 80, uint32_t h = 20) : width(w), height(h) {}
};

} // namespace snakes