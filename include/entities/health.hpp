#pragma once

#include "core/types.hpp"

namespace snakes {

class Health {
private:
    Point position_{};

public:
    Health() = default;
    explicit Health(Point position);

    void set_position(Point position) noexcept;
    Point position() const noexcept;
};

// ----------------------------------------------------------------------------
// @section Implementations
// Implementations
// ----------------------------------------------------------------------------
inline Health::Health(Point position) : position_(position) {}

inline void Health::set_position(Point position) noexcept { position_ = position; }

inline Point Health::position() const noexcept { return position_; }

} // namespace snakes