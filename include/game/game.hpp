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
        //process_event();
        view_.render(model_);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Game::process_event() {
    const auto& event = view_.poll_event();

    while (event.has_value()) {
        switch (event->key) {
            case KeyEvents::exit: {
                running_ = false;
                break;
            }
            
            default: {
                throw std::runtime_error("Unsupported event");
            }
        }
    }

}

} // namespace snakes
