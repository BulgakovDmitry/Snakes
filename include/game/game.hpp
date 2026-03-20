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
    bool paused_{false};

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
inline Game::Game(GameModel& model, IView& view)
    : model_(model), view_(view) {}


inline void Game::run() {
    while (running_) {
        // std::cout << "Running game loop..." << std::endl;
        process_event();

        if(!paused_)
            model_.update();

        view_.render(model_);
        std::this_thread::sleep_for(std::chrono::milliseconds(90)); 
    }
}

inline void Game::process_event() {
    while (auto event = view_.poll_event()) {
        switch (event->key) {
            case KeyEvents::exit: {
                running_ = false;
                break;
            }
            case KeyEvents::restart: {
                // std::cout << "Restarting game..." << std::endl;
                // TODO: implement restart logic
                break;
            }
            case KeyEvents::pause: {
                // std::cout << "Pausing game..." << std::endl;
                paused_ = !paused_;
                break;
            }
            case KeyEvents::up: {
                // std::cout << "Moving up..." << std::endl;
                model_.snakes.front().set_direction(Direction::up);
                break;
            }
            case KeyEvents::down: {
                // std::cout << "Moving down..." << std::endl;
                model_.snakes.front().set_direction(Direction::down);
                break;
            }
            case KeyEvents::left: {
                // std::cout << "Moving left..." << std::endl; 
                model_.snakes.front().set_direction(Direction::left);
                break;
            }
            case KeyEvents::right: {
                // std::cout << "Moving right..." << std::endl;
                model_.snakes.front().set_direction(Direction::right);
                break;
            }
            default: {
                throw std::runtime_error("Unsupported event");
            }
        }
    }

}

} // namespace snakes
