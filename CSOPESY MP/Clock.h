#ifndef CLOCK_H
#define CLOCK_H
#include <condition_variable>
#include <mutex>
#include <atomic>

class Clock {
public:
    std::mutex mtx;
    std::condition_variable cv;
    std::atomic<int> cycle{0};
    bool running = true;

    void waitForNextCycle(int lastCycle) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&] { return cycle > lastCycle || !running; });
    }

    void advance() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            ++cycle;
        }
        cv.notify_all();
    }

    void stop() {
        running = false;
        cv.notify_all();
    }
};
#endif
