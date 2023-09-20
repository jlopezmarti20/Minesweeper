#pragma once
#include <iostream>
#include <chrono>

class Timer {
private:
    bool running = false;
    bool paused = true;
    std::chrono::steady_clock::time_point  start_time;
    std::chrono::steady_clock::time_point pause_time;
    std::chrono::seconds elapsed_time{0};
    std::chrono::seconds paused_elapse_time{0};
public:
    Timer() : running(false), elapsed_time(0), start_time(std::chrono::steady_clock::now()) {}
    void Start() {
        if (!running) {
            start_time = std::chrono::steady_clock::now();
            running = true;
            paused = false;
        }
    }

    void Pause() {
        if (running) {
//            pause_time = std::chrono::steady_clock::now();
//            running = false;
//            elapsed_time += std::chrono::duration_cast<std::chrono::seconds>(pause_time - start_time);
//            paused_elapse_time = elapsed_time;
            pause_time = std::chrono::steady_clock::now();
            paused_elapse_time = std::chrono::duration_cast<std::chrono::seconds>(pause_time - start_time - elapsed_time);
            running = false;
        }
    }

    void Resume() {
        if (!running) {
            auto resume_time = std::chrono::steady_clock::now();
            start_time = resume_time - elapsed_time - paused_elapse_time;
            running = true;
        }
    }

    void Reset() {
        elapsed_time = std::chrono::seconds(0);
        paused_elapse_time = std::chrono::seconds(0);
        running = false;
    }

    int get_elapsed_time() const {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = elapsed_time;
        if (running) {
            elapsed += std::chrono::duration_cast<std::chrono::seconds>(now - start_time);
        }
        else {
            elapsed += paused_elapse_time;
        }
        return static_cast<int>(elapsed.count());
    }
};