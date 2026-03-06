#pragma once

#include <cstddef>
#include <cstdint>

namespace snakes {

struct Point {
    int32_t x, y;

    bool operator==(const Point& other) const noexcept{
            return x == other.x && y == other.y;
    }
};

enum class Direction {
    up, 
    down, 
    left, 
    right
};

Point next_point(Point point, Direction direction) noexcept;
bool is_opposite(Direction lhs, Direction rhs) noexcept;


// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
inline Point next_point(Point point, Direction direction) noexcept {
    switch (direction) {
        case Direction::up: {
            --point.y;
            break;
        }
        case Direction::down: {
            ++point.y;
            break;
        }   
        case Direction::left: {
            --point.x;
            break;
        }
        case Direction::right: {
            ++point.x;
            break;
        }
    }
    return point;
}

inline bool is_opposite(Direction lhs, Direction rhs) noexcept {
    return (lhs == Direction::up    && rhs == Direction::down)
        || (lhs == Direction::down  && rhs == Direction::up)
        || (lhs == Direction::left  && rhs == Direction::right)
        || (lhs == Direction::right && rhs == Direction::left);
}

} // namespace snakes
