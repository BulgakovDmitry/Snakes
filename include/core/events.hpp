#pragma once

namespace snakes {

enum class KeyEvents {
    up,
    down,
    right,
    left,

    restart,
    pause,
    exit,
};

struct Event {
    KeyEvents key;
};

} // namespace snakes