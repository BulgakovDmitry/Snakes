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
    bool restart_{false};

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
    : model_(model), view_(view) {
    for (auto& snake : model_.snakes) {
        if (snake.is_human_controlled()) {
            model_.human_snakes.push_back(std::addressof(snake));
        } else {
            model_.bot_snakes.push_back(std::addressof(snake));
        }
    }
}


inline void Game::run() {
    running_ = true;
    while (running_) {
        // std::cout << "Running game loop..." << std::endl;
        process_event();
        view_.render(model_);

        if(!paused_)
            model_.update();

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
                restart_ = true;
                //run();
                break;
            }
            case KeyEvents::pause: {
                paused_ = !paused_;
                break;
            }
            case KeyEvents::up_1: {
                if (model_.human_snakes.size() > 0 && model_.human_snakes[0]) {
                    model_.human_snakes[0]->set_direction(Direction::up);
                }
                break;
            }
            case KeyEvents::down_1: {
                if (model_.human_snakes.size() > 0 && model_.human_snakes[0]) {
                    model_.human_snakes[0]->set_direction(Direction::down);
                }
                break;
            }
            case KeyEvents::left_1: {
                if (model_.human_snakes.size() > 0 && model_.human_snakes[0]) {
                    model_.human_snakes[0]->set_direction(Direction::left);
                }
                break;
            }
            case KeyEvents::right_1: {
                if (model_.human_snakes.size() > 0 && model_.human_snakes[0]) {
                    model_.human_snakes[0]->set_direction(Direction::right);
                }
                break;
            }
            case KeyEvents::up_2: {
                if (model_.human_snakes.size() > 1 && model_.human_snakes[1]) {
                    model_.human_snakes[1]->set_direction(Direction::up);
                }
                break;
            }
            case KeyEvents::down_2: {
                if (model_.human_snakes.size() > 1 && model_.human_snakes[1]) {
                    model_.human_snakes[1]->set_direction(Direction::down);
                }
                break;
            }
            case KeyEvents::left_2: {
                if (model_.human_snakes.size() > 1 && model_.human_snakes[1]) {
                    model_.human_snakes[1]->set_direction(Direction::left);
                }
                break;
            }
            case KeyEvents::right_2: {  
                if (model_.human_snakes.size() > 1 && model_.human_snakes[1]) {
                    model_.human_snakes[1]->set_direction(Direction::right);
                }
                break;
            }
            
            default: {
                throw std::runtime_error("Unsupported event");
            }
        }
    }

}

} // namespace snakes
