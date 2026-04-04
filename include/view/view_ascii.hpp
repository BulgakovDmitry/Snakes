#pragma once

#include <iosfwd>
#include <memory>
#include <optional>
#include <ostream>
#include <iostream>
#include <termios.h>
#include <fstream>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include "game/game_model.hpp"
#include "view.hpp"
#include "colors.hpp"

namespace snakes {

class AsciiView final : public IView {
public:
    AsciiView();
    ~AsciiView();

    void render(GameModel& model) override;
    std::optional<Event> poll_event() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace snakes