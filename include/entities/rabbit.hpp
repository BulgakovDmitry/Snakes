#pragma once

#include "types.hpp"
#include <deque>

namespace snakes {

class Rabbit {
private:
    Point position_;
public:
    void spawn(int width, int height, const std::deque<Point>& snake_body);
    Point get_position() const noexcept;
};

} // namespace snakes