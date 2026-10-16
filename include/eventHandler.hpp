#pragma once

#include <vector>
#include <GLFW/glfw3.h>

namespace deoEvent{
    struct Event {
        //Empty struct for compatibility
    };

    struct MouseMoved : Event{
        double lastPos[2] = {0, 0};
        double currentPos[2] = {0, 0};
    };
    inline MouseMoved* MM;

    struct MouseLClick : Event {
        GLenum activity;
    };
    inline MouseLClick* MLC;
    inline MouseLClick LMLC;

    struct WindowResize : Event {
        int width, height;
        int winWidth, winHeight;
    };
    inline int lWidth = 0, lHeight = 0;
    inline WindowResize* WRS;

    void listenAll(GLFWwindow* window, const GLFWvidmode* video, double lastCPos[2]);

    const deoEvent::MouseMoved* requestMM();

    void clearEvents();
};