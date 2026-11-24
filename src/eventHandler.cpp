#include <sstream>

#include <eventHandler.hpp>
#include <typeinfo>

void deoEvent::listenAll(const GLFWvidmode* video, double lastCPos[2]) {
    double xPos, yPos;
    glfwGetCursorPos(mainWindow, &xPos, &yPos);
    if (xPos != lastCPos[0] || yPos != lastCPos[1]) {
        deoEvent::MouseMoved* mouse_move = new deoEvent::MouseMoved;
        mouse_move->currentPos[0] = xPos;
        mouse_move->currentPos[1] = yPos;
        mouse_move->lastPos[0] = lastCPos[0];
        mouse_move->lastPos[1] = lastCPos[1];
        MM = mouse_move;

        mainCursor->currentPos[0] = xPos;
        mainCursor->currentPos[1] = yPos;
    }
    lastCPos[0] = xPos;
    lastCPos[1] = yPos;

    GLenum lClickState = glfwGetMouseButton(mainWindow, GLFW_MOUSE_BUTTON_1);
    if (lClickState == GLFW_PRESS || lClickState == GLFW_RELEASE) {
        deoEvent::MouseLClick* mouse_left_click = new deoEvent::MouseLClick;
        mouse_left_click->activity = lClickState;
        MLC = mouse_left_click;
    }

    if (MLC != nullptr) {
        if (MLC->activity == GLFW_PRESS && LMLC.activity == GLFW_RELEASE) {
            mainCursor->firstLClickPos[0] = xPos;
            mainCursor->firstLClickPos[1] = yPos;
        }
    }

    int width, height;
    glfwGetFramebufferSize(mainWindow, &width, &height);
    if (width != lWidth || height != lHeight) {
        deoEvent::WindowResize* window_resize = new deoEvent::WindowResize;
        window_resize->width = width;
        window_resize->height = height;
        window_resize->winWidth = video->width;
        window_resize->winHeight = video->height;
        WRS = window_resize;
    }
    lWidth = width;
    lHeight = height;

    unsigned int backspaceCurState = glfwGetKey(mainWindow, GLFW_KEY_BACKSPACE);
    if (backspaceCurState == GLFW_PRESS || backspaceCurState == GLFW_RELEASE) {
        deoEvent::Backspace* backspace = new deoEvent::Backspace;
        backspace->activity = backspaceCurState;
        BS = backspace;
    }

    unsigned int enterCurState = glfwGetKey(mainWindow, GLFW_KEY_ENTER);
    if (enterCurState == GLFW_RELEASE || enterCurState == GLFW_PRESS) {
        deoEvent::Enter* enter = new deoEvent::Enter;
        enter->activity = enterCurState;
        ENT = enter;
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

unsigned int deoEvent::getKeyState(unsigned int key) {
    return glfwGetKey(mainWindow, key);
}

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
        MM = nullptr;
    }
    if (MLC != nullptr) {
        LMLC.activity = MLC->activity;
        delete MLC;
        MLC = nullptr;
    }
    if (WRS != nullptr) {
        delete WRS;
        WRS = nullptr;
    }
    if (CIN != nullptr) {
        delete CIN;
        CIN = nullptr;
    }
    if (BS != nullptr) {
        LBS.activity = BS->activity;
        delete BS;
        BS = nullptr;
    }
    if (ENT != nullptr) {
        delete ENT;
        ENT = nullptr;
    }
}

void deoEvent::initialize(GLFWwindow* window) {
    mainCursor = new Cursor();
    glfwGetCursorPos(window, &mainCursor->currentPos[0], &mainCursor->currentPos[1]);

    mainWindow = window;

    glfwSetCharCallback(mainWindow, char_callback);
}