#pragma once

#include <vector>

namespace deoTime {
    class WaitTask {
    public:
        float time;//time in seconds that must be waited
        float accum = 0.0f;//accumulation of time since task has been pending
        bool finished = 0;//just if waiting is done
    };
    inline std::vector<WaitTask*> allWaitTasks;

    inline float lastTime = 0.0f;
    void updWaitTasks();

    bool waitFor(const float time);
};