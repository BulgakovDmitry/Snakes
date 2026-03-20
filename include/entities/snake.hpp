#pragma once

#include <deque>
#include "core/types.hpp"

namespace snakes {

class Snake {
private:
    std::deque<Point> body_{};
    Direction direction_;
    int32_t color;

public:
    Snake() = default;
    Snake(int32_t color, Direction direction = Direction::right) 
        : color(color), direction_(direction) {};

    void set_direction(Direction direction) noexcept;
    Direction direction() const noexcept;

    Point head() const noexcept;
    const std::deque<Point>& body() const noexcept;

    void move();
};

// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
inline void Snake::set_direction(Direction direction) noexcept {
    if (!is_opposite(direction_, direction)) {
        direction_ = direction;
    }
}

inline Direction Snake::direction() const noexcept { return direction_; }

inline Point Snake::head() const noexcept { return body_.front(); }

inline const std::deque<Point>& Snake::body() const noexcept { return body_; }

inline void Snake::move() {
    switch (direction_) {
        case Direction::up: {
            body_.push_front(head() + Point{0, -1});
            break;
        }
        case Direction::down: {
            body_.push_front(head() + Point{0, 1});
            break;
        }
        case Direction::left: {
            body_.push_front(head() + Point{-1, 0});
            break;
        }
        case Direction::right: {
            body_.push_front(head() + Point{1, 0});
            break;  
            break;
        }
        default:
            throw std::runtime_error("Invalid direction");

    }
    body_.pop_back();
}

} // namespace snakes
