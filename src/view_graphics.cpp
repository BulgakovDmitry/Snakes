// #include "view/view_graphics.hpp"
// #include <SFML/Graphics.hpp>

// namespace snakes {

// struct GraphicsView::Impl {
//     sf::RenderWindow window; //(sf::VideoMode(windowWidth, windowHeight), "SFML movement");
//     sf::CircleShape shape;

//     Impl();
//     ~Impl();

// };

// // ----------------------------------------------------------------------------
// // @section Implementations
// // Implementations
// // ----------------------------------------------------------------------------
// GraphicsView::GraphicsView() : impl_(std::make_unique<Impl>()) {}
// GraphicsView::~GraphicsView() = default;

// GraphicsView::Impl::Impl() {
//     window.create(sf::VideoMode(600, 600), "Snakes");
//     window.setFramerateLimit(60);
//     shape.setRadius( 100.f );
//     shape.setFillColor( sf::Color::Green );
// }

// void GraphicsView::render(GameModel& model) {
//     if (!impl_->window.isOpen()) return;

//     impl_->window.clear();

//     // тута что-то нужно делать

//     impl_->window.display();
// }





// std::optional<Event> GraphicsView::poll_event() {
//     return std::nullopt; //impl_->read_key();
// }

// } // namespace snakes