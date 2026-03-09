#pragma once

#include "core/types.hpp"
#include "entities/snake.hpp"
#include <deque>

namespace snakes {

class Rabbit {
private:
    Point position_{};
public:
    Rabbit() = default;
    explicit Rabbit(Point position);

    void set_position(Point position) noexcept;
    Point position() const noexcept;
    void spawn(int width, int height, const Snake& snake);
};

// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
inline Rabbit::Rabbit(Point position) : position_(position) {}

inline void Rabbit::set_position(Point position) noexcept { position_ = position; }

inline Point Rabbit::position() const noexcept { return position_; }

inline void Rabbit::spawn(int width, int height, const Snake& snake) {} //TODO: implement

} // namespace snakes