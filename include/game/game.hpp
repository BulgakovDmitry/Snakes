#pragma once

#include <chrono>
#include <optional>
#include <stdexcept>
#include <thread>

#include "game/game_model.hpp"
#include "view/view.hpp"

namespace snakes {

struct RoundResult {
    bool exit_requested{false};
    std::optional<int32_t> winner_color{};
};

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
    RoundResult run_round();

private:
    void process_event();
    bool is_round_over() const;
}; 

inline Game::Game(GameModel& model, IView& view)
    : model_(model), view_(view) {
    for (auto& snake : model_.snakes) {
        if (snake.is_human_controlled()) {
            model_.human_snakes.push_back(std::addressof(snake));
        }
    }
}

inline void Game::run() {
    running_ = true;
    paused_ = false;

    while (running_) {
        process_event();
        view_.render(model_);

        if (!paused_) {
            model_.update();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(90));
    }
}

inline RoundResult Game::run_round() {
    running_ = true;
    paused_ = false;

    RoundResult result{};

    while (running_) {
        process_event();

        if (!running_) {
            result.exit_requested = true;
            break;
        }

        view_.render(model_);

        if (!paused_) {
            model_.update();

            if (is_round_over()) {
                view_.render(model_);
                if (model_.snakes.size() == 1) {
                    result.winner_color = model_.snakes.front().color();
                }
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(90));
    }

    return result;
}

inline bool Game::is_round_over() const {
    return model_.snakes.size() <= 1;
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