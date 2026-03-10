#pragma once

#include <thread>
#include <chrono>
#include <stdexcept>
#include <iostream>
#include "game/game_model.hpp"
#include "view/view.hpp"

namespace snakes {

class Game {
private:
    GameModel& model_;
    IView& view_;

    bool running_{true};

public:
    Game(GameModel& model, IView& view);

    void run();

private:
    void process_event();
}; 


// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
Game::Game(GameModel& model, IView& view)
    : model_(model), view_(view) {}


void Game::run() {
    while (running_) {
        // std::cout << "Running game loop..." << std::endl;
        process_event();
        view_.render(model_);
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
    }
}

void Game::process_event() {
    const auto& event = view_.poll_event();

    while (auto event = view_.poll_event()) {
        switch (event->key) {
            case KeyEvents::exit: {
                running_ = false;
                break;
            }
            case KeyEvents::restart: {
                std::cout << "Restarting game..." << std::endl;
                // TODO: implement restart logic
                break;
            }
            case KeyEvents::pause: {
                std::cout << "Pausing game..." << std::endl;
                // TODO: implement pause logic
                break;
            }
            case KeyEvents::up: {
                std::cout << "Moving up..." << std::endl;
                // TODO: implement move up logic
                break;
            }
            case KeyEvents::down: {
                std::cout << "Moving down..." << std::endl;
                // TODO: implement move down logic
                break;
            }
            case KeyEvents::left: {
                std::cout << "Moving left..." << std::endl; 
                // TODO: implement move left logic
                break;
            }
            case KeyEvents::right: {
                std::cout << "Moving right..." << std::endl;
                // TODO: implement move right logic
                break;
            }
            default: {
                throw std::runtime_error("Unsupported event");
            }
        }
    }

}

} // namespace snakes
