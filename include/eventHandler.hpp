#pragma once

namespace deoEvent{
    struct MouseMoved{
        bool active = 0;

        double* newPos[2] = {0, 0};

        const bool* operator () (double* x, double* y) const;
    };
};