#include "SharedMutex.h"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <mutex>

// 共享数据
int sharedData = 0;
SharedMutex dataMutex;
std::mutex coutMutex;  // 添加一个全局的互斥锁保护输出
std::atomic<int> readerCount(0);
std::atomic<int> writerCount(0);

// 读线程函数（修改后）
void readerThread(int id) {
    for (int i = 0; i < 5; ++i) {
        {
            SharedLockGuard lock(dataMutex);
            readerCount.fetch_add(1, std::memory_order_relaxed);

            {
                std::lock_guard<std::mutex> coutLock(coutMutex);  // 保护输出
                std::cout << "Reader " << id << " read: " << sharedData
                    << " (iteration: " << i
                    << ", readers: " << readerCount.load()
                    << ", writers: " << writerCount.load() << ")" << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            readerCount.fetch_sub(1, std::memory_order_relaxed);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// 写线程函数（同样修改）
void writerThread(int id) {
    for (int i = 0; i < 3; ++i) {
        {
            UniqueLockGuard lock(dataMutex);
            writerCount.fetch_add(1, std::memory_order_relaxed);

            ++sharedData;
            {
                std::lock_guard<std::mutex> coutLock(coutMutex);  // 保护输出
                std::cout << "Writer " << id << " wrote: " << sharedData
                    << " (iteration: " << i
                    << ", readers: " << readerCount.load()
                    << ", writers: " << writerCount.load() << ")" << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            writerCount.fetch_sub(1, std::memory_order_relaxed);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}

// 测试读写锁基本功能
void basicTest() {
    std::cout << "=== Basic SharedMutex Test ===" << std::endl;

    // 创建线程
    std::vector<std::thread> threads;

    // 创建2个写线程
    for (int i = 0; i < 2; ++i) {
        threads.emplace_back(writerThread, i);
    }

    // 创建5个读线程
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back(readerThread, i);
    }

    // 等待所有线程完成
    for (auto& t : threads) {
        t.join();
    }

    std::cout << "Final shared data value: " << sharedData << std::endl;
    std::cout << "Basic test completed successfully!" << std::endl;
}

int main() {
    std::cout << "Starting SharedMutex test..." << std::endl;
    basicTest();
    return 0;
}