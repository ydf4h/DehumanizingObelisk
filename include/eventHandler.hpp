#pragma once

#include <vector>
#include<sstream>
#include <iostream>

#include <GLFW/glfw3.h>

namespace deoEvent{
    inline GLFWwindow* mainWindow;

    struct Cursor {
        double firstLClickPos[2];
        double currentPos[2];
    };
    inline Cursor* mainCursor;

    struct MouseMoved{
        double lastPos[2] = {0, 0};
        double currentPos[2] = {0, 0};
    };
    inline MouseMoved* MM;

    struct MouseLClick{
        GLenum activity = GLFW_RELEASE;
    };
    inline MouseLClick* MLC;
    inline MouseLClick LMLC;

    struct WindowResize{
        int width, height;
        int winWidth, winHeight;
    };
    inline int lWidth = 0, lHeight = 0;
    inline WindowResize* WRS;

    struct CharInput {
        unsigned char c;
    };
    inline CharInput* CIN;

    struct Backspace {
        GLenum activity = GLFW_RELEASE;
    };
    inline Backspace* BS;
    inline Backspace LBS;

    struct Enter {
        GLenum activity = GLFW_RELEASE;
    };
    inline Enter* ENT;

    void listenAll(const GLFWvidmode* video, double lastCPos[2]);

    unsigned int getKeyState(unsigned int key);

    const deoEvent::MouseMoved* requestMM();

    void clearEvents();

    inline void char_callback(GLFWwindow* window, unsigned int codepoint) {
        unsigned char realChar = static_cast<unsigned char>(codepoint);

        CIN = new CharInput;
        CIN->c = realChar;
    }

    void initialize(GLFWwindow* window);
};