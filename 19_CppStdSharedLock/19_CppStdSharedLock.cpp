#include <iostream>
#include <thread>
#include <shared_mutex>

std::shared_mutex rwMutex;
int sharedData = 0;

// 写线程（独占）
void writer() {
    for (int i = 0; i < 5; ++i) {
        std::unique_lock<std::shared_mutex> lock(rwMutex);
        sharedData++;
        std::cout << "write: " << sharedData << std::endl;
    }
}

// 读线程（共享）
void reader(int id) {
    for (int i = 0; i < 5; ++i) {
        std::shared_lock<std::shared_mutex> lock(rwMutex);
        std::cout << "reader " << id << " read: " << sharedData << std::endl;
    }
}

int main() {
    std::thread w(writer);
    std::thread r1(reader, 1);
    std::thread r2(reader, 2);

    w.join();
    r1.join();
    r2.join();
}
