#include "game/game.hpp"
#include "view/view_ascii.hpp"
#include "entities/snake.hpp"


int main() {
    snakes::GameModel model{};
    snakes::AsciiView view{};

    snakes::Snake snake(snakes::Point{22, 10}, snakes::Direction::right);
    model.snakes.push_back(snake);

    std::cout << "Welcome to Snakes!" << std::endl;

    snakes::Game game(model, view);


    game.run();
}