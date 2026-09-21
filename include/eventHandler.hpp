#pragma once

#include <vector>
#include <GLFW/glfw3.h>

namespace deoEvent{
    struct Event {
        //Empty struct for compatibility
    };

    struct MouseMoved : Event{
        double currentPos[2] = {0, 0};
    };

    inline MouseMoved* MM;

    struct MouseLClick : Event {
        GLenum activity;
    };

    inline MouseLClick* MLC;
    inline MouseLClick LMLC;

    void listenAll(GLFWwindow* window, double lastCPos[2]);

    const deoEvent::MouseMoved* requestMM();

    void clearEvents();
};