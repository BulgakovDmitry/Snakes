#pragma once

namespace snakes {

enum class KeyEvents {
    up_1,
    down_1,
    right_1,
    left_1,

    up_2,
    down_2,
    right_2,
    left_2,

    restart,
    pause,
    exit,
};

struct Event {
    KeyEvents key;
};

} // namespace snakes