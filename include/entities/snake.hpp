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

// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
inline Snake::Snake(Point start, Direction direction) {
    reset(start, direction);
}

inline void Snake::reset(Point start, Direction direction) {
    body_.clear();
    body_.push_back(start);
    direction_ = direction;
}

inline void Snake::set_direction(Direction direction) noexcept {
    if (!is_opposite(direction_, direction)) {
        direction_ = direction;
    }
}

inline Direction Snake::direction() const noexcept { return direction_; }

inline Point Snake::head() const noexcept { return body_.front(); }

inline const std::deque<Point>& Snake::body() const noexcept { return body_; }

inline bool Snake::located_on(Point point) const noexcept {
    for (const auto& body_segment : body_) {
        if (body_segment == point) {
            return true;
        }
    }
    return false;
}

inline bool Snake::hits_itself() const noexcept {
    if (body_.size() < 2) return false;

    const Point head_point = head();
    for (size_t i = 1; i < body_.size(); ++i) {
        if (body_[i] == head_point) {
            return true;
        }
    }
    return false;
}

inline void Snake::move(bool grow) {
    body_.push_front(next_point(head(), direction_));
    if (!grow) {
        body_.pop_back();
    }
}

} // namespace snakes
