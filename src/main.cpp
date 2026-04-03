#include "game/game.hpp"
#include "view/view_ascii.hpp"
#include "entities/snake.hpp"


int main() {
    snakes::GameModel model{};
    snakes::AsciiView view{};

    snakes::Snake snake_1 = snakes::Snake::Builder()
        .set_color(fg_red)
        .set_direction(snakes::Direction::right)
        .set_spawn_point({4, 6})
        .set_human_controlled(true)
        .build();
    model.snakes.push_back(snake_1);

    snakes::Snake snake_2 = snakes::Snake::Builder()
        .set_color(fg_blue)
        .set_direction(snakes::Direction::right)
        .set_spawn_point({4, 8})
        .set_human_controlled(true)
        .build();
    model.snakes.push_back(snake_2);

    snakes::Snake snake_bot1 = snakes::Snake::Builder()
        .set_color(fg_magenta)
        .set_direction(snakes::Direction::right)
        .set_spawn_point({10, 8})
        .set_human_controlled(false)
        .build();
    model.snakes.push_back(snake_bot1);
    

    //std::cout << "Welcome to Snakes!" << std::endl;

    snakes::Game game(model, view);

    game.run();
}