#pragma once

#include <iosfwd>
#include <memory>
#include <optional>
#include <ostream>
#include <iostream>
#include <termios.h>
#include <fstream>
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
    std::string buffer{};
    termios old_term{};
    bool terminal_configured{false};

    Impl();
    ~Impl();

    void show();
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

    void hide_cursor();
    void show_cursor();

    void draw_frame(uint32_t width, uint32_t height, 
                    Point curr_pos);

    void draw_preview(uint32_t width, uint32_t height, 
                      Point curr_pos);

    void draw_rabbits(const GameModel& model);

    void draw_snakes(const GameModel& model);  
};


// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
inline AsciiView::AsciiView() : impl_(std::make_unique<Impl>()) {}
inline AsciiView::~AsciiView()  = default; /* {
    impl_->buffer += "\033[2J\033[3J\033[H";
    impl_->show();
}*/

inline void AsciiView::render(const GameModel& model) {
    impl_->buffer.clear();
    impl_->clear_screen();
    impl_->draw(model);
    impl_->show();
}

inline std::optional<Event> AsciiView::poll_event() {
    return impl_->read_key();
}

inline AsciiView::Impl::Impl() {
    setup_terminal();
    
    buffer += "\033[?1049h"; 
    buffer += "\033[2J";      
    buffer += "\033[H";      
    //hide_cursor();
    show();
    buffer.clear();
}

inline AsciiView::Impl::~Impl() {
    buffer.clear();
    reset_color();
    // show_cursor();
    buffer += "\033[H";
    buffer += "\033[2J";
    buffer += "\033[?1049l";  
    show();

    restore_terminal();
}

inline void AsciiView::Impl::show() {
    out << buffer << std::flush;
}

inline void AsciiView::Impl::clear_screen() {
    buffer += "\033[2J\033[H";
}

inline void AsciiView::Impl::set_color(const int bg_color) {
    buffer += "\033[" + std::to_string(bg_color) + "m";
}

inline void AsciiView::Impl::reset_color() {
    set_color(ansi_reset);
}

inline void AsciiView::Impl::hide_cursor() {
    buffer+="\033[?25l";
}

inline void AsciiView::Impl::show_cursor() {
    buffer+="\033[?25h";
}

inline void AsciiView::Impl::gotoxy(uint32_t x, uint32_t y) {
    buffer += "\033[" + std::to_string(y + 1) + ";" + std::to_string(x + 1) + "H";
}

inline void AsciiView::Impl::gotoxy(const Point& p) {
    buffer += "\033[" + std::to_string(p.y + 1) + ";" + std::to_string(p.x + 1) + "H";
}

inline void AsciiView::Impl::draw(const GameModel& model) {
    uint32_t width = model.width;
    uint32_t height = model.height;

    Point start_p = model.start_point;
    draw_preview(width, height, start_p);

    start_p.y += 3;
    draw_frame(width, height, start_p);

    draw_rabbits(model);
    draw_snakes(model);
    gotoxy(0, 0);
}

inline void AsciiView::Impl::draw_rabbits(const GameModel& model) {  
    for (const auto& rabbit : model.rabbits) {
        gotoxy(rabbit.position());
        set_color(bg_green);
        buffer += "¤";
        reset_color();
    }
}

inline void AsciiView::Impl::draw_snakes(const GameModel& model) {  
    for (const Snake& snake : model.snakes) {
        const auto& body = snake.body();

        if (body.empty()) {
            continue;
        }

        bool is_head = true;
        for (const Point& segment : body) {
            gotoxy(segment);

            if (is_head) {
                set_color(snake.color());  
                set_color(bg_green);
                buffer += "@";
                reset_color();
                is_head = false;
            } else {
                set_color(snake.color());  
                set_color(bg_green); 
                buffer += "o";
                reset_color();
            }
        }
    }
}

inline void AsciiView::Impl::draw_frame(uint32_t width, uint32_t height, Point curr_pos) {
    
    set_color(fg_black);
    set_color(bg_bright_black);

    gotoxy(curr_pos);
    buffer += "┏";
    for (uint32_t x = 0; x < width; ++x) {
        buffer += "━";
    }
    buffer += "┓";

    ++curr_pos.y;

    for (uint32_t y = 0; y < height; ++y) {
        gotoxy(curr_pos);
        buffer += "┃";

        set_color(bg_green);
        for (uint32_t x = 0; x < width; ++x) {
            buffer += " ";
        }

        set_color(bg_bright_black);
        buffer += "┃";

        ++curr_pos.y;
    }

    gotoxy(curr_pos);
    buffer += "┗";
    for (uint32_t x = 0; x < width; ++x) {
        buffer += "━";
    }
    buffer += "┛";

    reset_color();
}

inline void AsciiView::Impl::draw_preview(uint32_t width, uint32_t /*height*/, Point curr_pos) {
    uint32_t title_len = 10; // " S N A K E S "
    uint32_t start_x = curr_pos.x + width / 2 - title_len / 2;

    set_color(bg_green);
    set_color(fg_black);

    gotoxy(curr_pos);
    buffer += "╔";
    for (uint32_t x = 0; x < width; ++x) {
        buffer += "═";
    }
    buffer += "╗";

    ++curr_pos.y;
    gotoxy(curr_pos);
    buffer += "║";
    for (uint32_t x = 0; x < width; ++x) {
        buffer += " ";
    }
    buffer += "║";

    ++curr_pos.y;
    gotoxy(curr_pos);
    buffer += "╚";
    for (uint32_t x = 0; x < width; ++x) {
        buffer += "═";
    }
    buffer += "╝";

    gotoxy(start_x, curr_pos.y - 1);
    buffer += " S N A K E S ";

    reset_color();
}

inline void AsciiView::Impl::setup_terminal() {
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

inline void AsciiView::Impl::restore_terminal() {
    if (!terminal_configured) {
        return;
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_term);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flags);

    terminal_configured = false;
}

inline std::optional<Event> AsciiView::Impl::read_key() {
    char ch;
    const ssize_t n = ::read(STDIN_FILENO, &ch, 1);

    if (n <= 0) {
        return std::nullopt;
    }

    switch (ch) {
        case 'w':
        case 'W': {
            return Event{KeyEvents::up_1};
            break;
        }

        case 's':
        case 'S': {
            return Event{KeyEvents::down_1};
            break;
        }
        case 'a':
        case 'A': {
            return Event{KeyEvents::left_1};
            break;
        }

        case 'd':
        case 'D': {
            return Event{KeyEvents::right_1};
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

        case '\x1b': { 
            char seq[2];
            const ssize_t n1 = ::read(STDIN_FILENO, &seq[0], 1);
            const ssize_t n2 = ::read(STDIN_FILENO, &seq[1], 1);

            if (n1 <= 0 || n2 <= 0) {
                return std::nullopt;
            }

            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': return Event{KeyEvents::up_2};    
                    case 'B': return Event{KeyEvents::down_2};  
                    case 'C': return Event{KeyEvents::right_2}; 
                    case 'D': return Event{KeyEvents::left_2};  
                    default:  return std::nullopt;
                }
            }

            return std::nullopt;
        }

        default:
            return std::nullopt;
    }

    return std::nullopt;
}

} // namespace snakes