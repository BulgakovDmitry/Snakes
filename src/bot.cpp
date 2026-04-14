#include "core/bot.hpp"
#include "game/game_model.hpp"
#include "entities/snake.hpp"
#include "entities/rabbit.hpp"

#include <array>
#include <limits>
#include <cmath>

namespace snakes {

static constexpr std::array<Direction, 4> dirs{
    Direction::up, Direction::down, Direction::left, Direction::right
};

static bool is_inside(const GameModel& model, Point p);
static bool is_occupied(const GameModel& model, Point p, const Snake& self);
static int distance(Point a, Point b);
static const Rabbit* nearest_rabbit(const GameModel& model, const Snake& snake);
static int count_free_neighbors(const GameModel& model, Point p, const Snake& self);


Direction choose_bot_direction(const GameModel& model, const Snake& snake) {
    Direction best_dir = snake.direction();
    int best_score = std::numeric_limits<int>::max();
    bool found = false;

    for (Direction dir : dirs) {
        if (is_opposite(snake.direction(), dir)) {
            continue;
        }

        Point next = next_point(snake.head(), dir);

        if (!is_inside(model, next)) {
            continue;
        }

        if (is_occupied(model, next, snake)) {
            continue;
        }

        const Rabbit* rabbit = nearest_rabbit(model, snake);

        int rabbit_score = 0;
        if (rabbit) {
            rabbit_score = distance(next, rabbit->position());
        }

        int space_score = count_free_neighbors(model, next, snake);

        int score = rabbit_score * 10 - space_score;

        if (!found || score < best_score) {
            found = true;
            best_score = score;
            best_dir = dir;
        }
    }

    return best_dir;
}

static bool is_inside(const GameModel& model, Point p) {
    Point start = model.start_point + Point{0, 3};

    return p.x >= start.x + 1 &&
           p.x <= start.x + static_cast<int32_t>(model.width) &&
           p.y >= start.y + 1 &&
           p.y <= start.y + static_cast<int32_t>(model.height);
}

static bool is_occupied(const GameModel& model, Point p, const Snake& self) {
    for (const Snake& snake : model.snakes) {
        const auto& body = snake.body();

        for (std::size_t i = 0; i < body.size(); ++i) {
            if (&snake == &self && i == 0) {
                continue; 
            }

            if (body[i] == p) {
                return true;
            }
        }
    }

    return false;
}

static int distance(Point a, Point b) {
    return std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2);
}

static const Rabbit* nearest_rabbit(const GameModel& model, const Snake& snake) {
    const Rabbit* best = nullptr;
    int best_dist = std::numeric_limits<int>::max();

    for (const Rabbit& rabbit : model.rabbits) {
        int dist = distance(snake.head(), rabbit.position());
        if (dist < best_dist) {
            best_dist = dist;
            best = &rabbit;
        }
    }
    return best;
}

static int count_free_neighbors(const GameModel& model, Point p, const Snake& self) {
    int count = 0;
    for (Direction dir : dirs) {
        Point np = next_point(p, dir);
        if (is_inside(model, np) && !is_occupied(model, np, self)) {
            ++count;
        }
    }
    return count;
}

} // namespace snakes