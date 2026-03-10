#include "game/game.hpp"
#include "view/view_ascii.hpp"


int main() {
    snakes::GameModel model{};
    snakes::AsciiView view{};

    std::cout << "Welcome to Snakes!" << std::endl;

    snakes::Game game(model, view);


    game.run();
}