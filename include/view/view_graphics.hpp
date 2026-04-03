#pragma once

#include <cstddef>
#include <cstdint>
#include "view.hpp"

namespace snakes {
    
class GraphicsView final : public IView {
public:
    GraphicsView(uint32_t screen_width = 1280, uint32_t screen_height = 720);
    GraphicsView() = default;
    ~GraphicsView();

    virtual void render(GameModel& model) override;
};

} // namespace snakes