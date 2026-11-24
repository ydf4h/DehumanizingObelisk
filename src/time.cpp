#include <iostream>

#include <GLFW/glfw3.h>
#include <time.hpp>

void deoTime::updWaitTasks() {
    float currentTime = glfwGetTime();
    float deltaTime = currentTime - lastTime;
    std::vector<WaitTask*>::const_iterator v = allWaitTasks.begin();
    for (unsigned int i = 0; i < allWaitTasks.size(); i++, v++) {
        allWaitTasks[i]->accum += deltaTime;
        if (allWaitTasks[i]->finished) {
            delete allWaitTasks[i];
            allWaitTasks.erase(v);
        }
        if (allWaitTasks[i]->accum >= allWaitTasks[i]->time) {
            allWaitTasks[i]->finished = 1;
        }
    }

    lastTime = currentTime;
}

bool deoTime::waitFor(const float time) {
    bool alreadyExists = 0;
    for (unsigned int i = 0; i < allWaitTasks.size(); i++) {
        if (allWaitTasks[i]->time == time) {
            alreadyExists = 1;
            return allWaitTasks[i]->finished;
        }
    }
    if (!alreadyExists) {
        WaitTask* result = new WaitTask;
        result->time = time;
        allWaitTasks.push_back(result);
        return result->finished;
    }
}