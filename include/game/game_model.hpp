#pragma once

#include <cstddef>
#include <cstdint>

#include "snake.hpp"
#include "rabbit.hpp"

namespace snakes {

struct GameModel {
    int32_t width{20};
    int32_t height{10};

    Snake snake{};
    Rabbit rabbit{};

    bool game_over{false};
    int score{0};
};

} // namespace snakes