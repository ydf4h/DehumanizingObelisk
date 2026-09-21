#include <eventHandler.hpp>
#include <typeinfo>

void deoEvent::listenAll(GLFWwindow* window, double lastCPos[2]) {
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    if (xPos != lastCPos[0] || yPos != lastCPos[1]) {
        deoEvent::MouseMoved* mouse_move = new deoEvent::MouseMoved;
        mouse_move->currentPos[0] = xPos;
        mouse_move->currentPos[1] = yPos;
        MM = mouse_move;
    }else {
        MM = nullptr;
    }
    lastCPos[0] = xPos;
    lastCPos[1] = yPos;

    GLenum lClickState = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_1);
    if (lClickState == GLFW_PRESS || lClickState == GLFW_RELEASE) {
        deoEvent::MouseLClick* mouse_left_click = new deoEvent::MouseLClick;
        mouse_left_click->activity = lClickState;
        MLC = mouse_left_click;
    }else {
        MLC = nullptr;
    }
}

/*const deoEvent::Event* deoEvent::requestEvent(Event* type) {
    Event* OUTPUT;

    if (ALLEVENTS.size() != 0) {
        for (unsigned int i = 0; i < ALLEVENTS.size(); i++) {
            if (typeid(*ALLEVENTS[i]) ==  typeid(*type)) {
                OUTPUT = ALLEVENTS[i];
            }else if (i + 1 == ALLEVENTS.size()) {
                OUTPUT = nullptr;
            }
        }
    }else {
        OUTPUT = nullptr;
    }

    return OUTPUT;
}*/

/*const deoEvent::MouseMoved* deoEvent::requestMM() {
    if (MM != nullptr) {
        for (unsigned int i = 0; i < ALLMM.size(); i++) {
            if (typeid(*ALLMM[i]) ==  typeid(*type)) {
                type = ALLMM[i];
            }else if (i + 1 == ALLMM.size()) {
                type = nullptr;
            }
        }
    }else {
        type = nullptr;
    }
}*/

void deoEvent::clearEvents() {
    if (MM != nullptr) {
        delete MM;
    }
    if (MLC != nullptr) {
        LMLC.activity = MLC->activity;
        delete MLC;
    }
}