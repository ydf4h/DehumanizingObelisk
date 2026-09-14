#include <eventHandler.hpp>

const bool* deoEvent::MouseMoved::operator () (double* x, double* y) const {
    x = newPos[0];
    y = newPos[1];

    return &active;
}