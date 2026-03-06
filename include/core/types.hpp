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

} // namespace snakes
