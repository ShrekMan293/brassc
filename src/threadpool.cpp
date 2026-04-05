#include "threadpool.hpp"

void Brass::ThreadPool::worker()
{
    for (;;) {
        std::function<void()> cur_task; 
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]() {
                return stop || !queue.empty();
            });

            if (queue.empty() && stop)
                break;
            if (queue.empty())
                continue;

            cur_task = queue.front();
            queue.pop();
        }
        cur_task();
    }
}

Brass::ThreadPool::ThreadPool(size_t numThreads)
{
    stop = false;
    for (size_t i = 0; i < numThreads; i++) {
        workers.emplace_back(&ThreadPool::worker, this);
    }
}

Brass::ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mutex);
        stop = true;
    }

    cv.notify_all();
    for (auto& worker : workers) {
        worker.join();
    }
}