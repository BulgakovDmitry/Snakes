#pragma once

#include <deque>
#include "types.hpp"

namespace snakes {

class Snake {
private:
    std::deque<Point> body_{};
    Direction direction_{Direction::right};

public:
    Snake() = default;
    Snake(Point start, Direction direction);

    void reset(Point start, Direction direction);

    void set_direction(Direction direction) noexcept;
    Direction direction() const noexcept;

    Point head() const noexcept;
    const std::deque<Point>& body() const noexcept;

    bool located_on(Point point) const noexcept;
    bool hits_itself() const noexcept;

    void move(bool grow = false);
};



} // namespace snakes
