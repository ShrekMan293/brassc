#pragma once
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <future>
#include "result.hpp"

namespace Brass {
    class ThreadPool {
    private:
        vector<std::thread> workers;
        std::mutex mutex;
        std::condition_variable cv;
        std::queue<std::function<void()>> queue;
        bool stop;
        void worker();

    public:
        ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
        ~ThreadPool();

        template<typename F, typename... Args>
        auto enqueue(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
            using return_type = std::invoke_result_t<F, Args...>;
            
            auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
            );

            std::future<return_type> res = task->get_future();
            {
                std::unique_lock<std::mutex> lock(mutex);
                queue.emplace([task](){ (*task)(); });
            }
            cv.notify_one();
            return res;
        }
    };
}