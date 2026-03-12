#pragma once

#include <iosfwd>
#include <memory>
#include <optional>
#include <ostream>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

#include "game/game_model.hpp"
#include "view.hpp"
#include "colors.hpp"

namespace snakes {

class AsciiView final : public IView {
public:
    AsciiView();
    ~AsciiView();

    void render(const GameModel& model) override;
    
    std::optional<Event> poll_event() override;

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

struct AsciiView::Impl {
    std::ostream& out{std::cout};
    std::string buffer = "\033[2J\033[H";

    termios old_term{};
    bool terminal_configured{false};

    Impl();
    ~Impl();

    void clear_screen();
    void draw(const GameModel& model);
    void gotoxy(uint32_t x, uint32_t y);
    void gotoxy(const Point& p);
    void set_color(const int bg_color);
    void reset_color();

    std::optional<Event> read_key();

private:
    void setup_terminal();
    void restore_terminal();

    void draw_frame(uint32_t width, uint32_t height, 
                    Point curr_pos);

    void draw_preview(uint32_t width, uint32_t height, 
                      Point curr_pos);
};


// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
AsciiView::AsciiView() : impl_(std::make_unique<Impl>()) {}
AsciiView::~AsciiView() = default;

void AsciiView::render(const GameModel& model) {
    impl_->buffer.clear();
    impl_->clear_screen();
    impl_->draw(model);
    impl_->out << impl_->buffer;
    impl_->out.flush();
}

std::optional<Event> AsciiView::poll_event() {
    return impl_->read_key();
}

AsciiView::Impl::Impl() {
    setup_terminal();
}

AsciiView::Impl::~Impl() {
    restore_terminal();
}

void AsciiView::Impl::clear_screen() {
    buffer+="\033[H";
}

void AsciiView::Impl::set_color(const int bg_color) {
    buffer += "\033[" + std::to_string(bg_color) + "m";
}

void AsciiView::Impl::reset_color() {
    set_color(ansi_reset);
}

void AsciiView::Impl::gotoxy(uint32_t x, uint32_t y) {
    buffer += "\033[" + std::to_string(y + 1) + ";" + std::to_string(x + 1) + "H";
}

void AsciiView::Impl::gotoxy(const Point& p) {
    buffer += "\033[" + std::to_string(p.y + 1) + ";" + std::to_string(p.x + 1) + "H";
}

void AsciiView::Impl::draw(const GameModel& model) {
    uint32_t width = model.width;
    uint32_t height = model.height;

    Point start_p{20, 8};
    draw_preview(width, height, start_p);
    start_p.y += 3;
    draw_frame(width, height, start_p);

    //gotoxy(start_p + Point{1, 1});
    // for (auto it = model.snakes.begin(); it != model.snakes.end(); ++it) {
    //     draw_snake(*it);
    // }
}

void AsciiView::Impl::draw_frame(uint32_t width, uint32_t height, 
                                 Point curr_pos) {
    set_color(fg_black);
    set_color(bg_bright_black);
    gotoxy(curr_pos);
    ++curr_pos.y;
    buffer += "┏";
    for (uint32_t x = 0; x < width; ++x) {
        buffer += "━";
    }
    buffer += "┓\n";

    for (uint32_t y = 0; y < height; ++y) {
        gotoxy(curr_pos);
        ++curr_pos.y;
        buffer +=  "┃";
        for (uint32_t x = 0; x < width; ++x) {
            set_color(bg_green);
            buffer += " ";
        }
        set_color(bg_bright_black);
        buffer += "┃\n";
    }

    gotoxy(curr_pos);
    buffer += "┗";
    for (uint32_t x = 0; x < width; ++x) {
        buffer += "━";
    }
    buffer += "┛\n";
    reset_color();
}

void AsciiView::Impl::draw_preview(uint32_t width, uint32_t height, 
                      Point curr_pos) {
    
    uint32_t start_x = curr_pos.x + width/2 - 6;

    gotoxy(curr_pos);
    set_color(bg_green);
    set_color(fg_black);
    buffer += "╔";
    for (uint32_t x = 0; x < width; ++x) {
        buffer += "═";
    }
    buffer += "╗\n";
    ++curr_pos.y;

    gotoxy(curr_pos);
    buffer +=  "║";
    for (uint32_t x = 0; x < width; ++x) {
        buffer += " ";
    }
    buffer += "║\n";
    ++curr_pos.y;

    gotoxy(curr_pos);
    buffer += "╚";
    for (uint32_t x = 0; x < width; ++x) {
        buffer += "═";
    }
    buffer += "╝\n";

    gotoxy(start_x, curr_pos.y - 1);
    buffer += " S N A K E S ";
    reset_color();

}

void AsciiView::Impl::setup_terminal() {
    if (terminal_configured) {
        return;
    }

    tcgetattr(STDIN_FILENO, &old_term);
    termios new_term = old_term;

    new_term.c_lflag &= ~(ICANON | ECHO); 

    tcsetattr(STDIN_FILENO, TCSANOW, &new_term);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    terminal_configured = true;
}

void AsciiView::Impl::restore_terminal() {
    if (!terminal_configured) {
        return;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);

    terminal_configured = false;
}

std::optional<Event> AsciiView::Impl::read_key() {
    char ch;
    const ssize_t n = ::read(STDIN_FILENO, &ch, 1);

    if (n <= 0) {
        return std::nullopt;
    }

    switch (ch) {
        case 'w':
        case 'W': {
            return Event{KeyEvents::up};
            break;
        }

        case 's':
        case 'S': {
            return Event{KeyEvents::down};
            break;
        }
        case 'a':
        case 'A': {
            return Event{KeyEvents::left};
            break;
        }

        case 'd':
        case 'D': {
            return Event{KeyEvents::right};
            break;
        }

        case 'p':
        case 'P': {
            return Event{KeyEvents::pause};
            break;
        }

        case 'r':
        case 'R': {
            return Event{KeyEvents::restart};
            break;
        }

        case 'q':
        case 'Q':
        case 'e':
        case 'E': {
            return Event{KeyEvents::exit};
            break;
        }

        default:
            return std::nullopt;
    }

    return std::nullopt;
}

} // namespace snakes