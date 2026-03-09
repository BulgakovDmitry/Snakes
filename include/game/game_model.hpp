#pragma once

#include <cstddef>
#include <cstdint>
#include <list>
#include "entities/snake.hpp"
#include "entities/rabbit.hpp"

namespace snakes {

struct GameModel {
    int32_t width{20};
    int32_t height{10};

    std::list<Snake> snakes{};
    std::list<Rabbit> rabbits{};

    bool game_over{false};
    int score{0};
};

} // namespace snakes