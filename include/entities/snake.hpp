#pragma once

#include <deque>
#include "core/types.hpp"
#include "view/colors.hpp"

namespace snakes {

class Snake {
private:
    std::deque<Point> body_;
    Direction direction_;
    int32_t color_;
    Point spawn_point_;
    bool human_controlled_;
    
public:

    Snake(int32_t color, Direction direction, Point spawn_point_, bool human_controlled);
    struct Builder;

    void set_direction(Direction direction) noexcept;
    Direction direction() const noexcept;

    bool located_on(Point point) const noexcept;

    Point head() const noexcept;
    const std::deque<Point>& body() const noexcept;
    std::deque<Point>& body() noexcept;
    int32_t color() const noexcept;
    bool is_human_controlled() const noexcept;

    void move();
};

// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
inline Snake::Snake(int32_t color, Direction direction, Point spawn_point, bool human_controlled)
  : direction_(direction), color_(color), spawn_point_(spawn_point), human_controlled_(human_controlled)
    {
        body_.push_back(spawn_point);
        body_.push_front(spawn_point + Point{1, 0});
        body_.push_front(spawn_point + Point{2, 0});
    }

inline void Snake::set_direction(Direction direction) noexcept {
    if (!is_opposite(direction_, direction)) {
        direction_ = direction;
    }
}

inline bool Snake::located_on(Point point) const noexcept {
    for (const auto& body_segment : body_) {
        if (body_segment == point) {
            return true;
        }
    }
    return false;
}

inline Direction Snake::direction() const noexcept { return direction_; }

inline Point Snake::head() const noexcept { return body_.front(); }

inline const std::deque<Point>& Snake::body() const noexcept { return body_; }
inline std::deque<Point>& Snake::body() noexcept { return body_; }

inline int32_t Snake::color() const noexcept { return color_; }

inline bool Snake::is_human_controlled() const noexcept { return human_controlled_; }

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

struct Snake::Builder {
    Direction direction{Direction::right};
    int32_t color{fg_red};
    Point spawn_point_{4, 4};
    bool human_controlled_{false};

    Builder& set_direction(const Direction dir) {
        direction = dir;
        return *this;
    }
    Builder& set_color(const int32_t c) {
        color = c;
        return *this;
    }
    Builder& set_spawn_point(const Point p) {
        spawn_point_ = p;
        return *this;
    }
    Builder& set_human_controlled(const bool hc) {
        human_controlled_ = hc;
        return *this;
    }

    Snake build() const {
        return Snake(color, direction, spawn_point_, human_controlled_);
    }
};

} // namespace snakes
