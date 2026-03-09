#include "game/game.hpp"
#include "view/view_ascii.hpp"


int main() {
    snakes::GameModel model(80, 20);
    snakes::AsciiView view{};

    std::cout << "Welcome to Snakes!" << std::endl;

    snakes::Game game(model, view);

    std::cout << "Press any key to start..." << std::endl;

    game.run();
}