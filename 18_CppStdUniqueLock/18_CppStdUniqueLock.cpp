#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

// 全局变量
std::mutex mtx;             // 创建互斥锁，用于保护共享数据的访问
std::condition_variable cv; // 创建条件变量，用于线程间同步
std::queue<int> q;          // 共享队列，生产者向其中放入数据，消费者从中取出数据

// 生产者函数
void producer() {
    for (int i = 0; i < 5; i++) {
        std::unique_lock<std::mutex> lock(mtx);   // 获取锁，保护对队列的操作
        q.push(i);                                // 将数据 i 放入队列
        std::cout << "Produce:" << i << std::endl;
        lock.unlock();                            // 提前释放锁，允许其他线程操作队列
        cv.notify_one();                          // 通知一个正在等待的消费者线程（唤醒消费者）
    }
}

// 消费者函数
void consumer() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);  // 获取锁

        // 重点解释：cv.wait(lock, predicate)
        // 1. wait() 会先检查条件（第二个参数，即lambda函数）是否满足：
        //    - 如果条件满足（lambda返回true，即队列不为空），则wait()直接返回，继续执行后面的代码。
        //    - 如果条件不满足（队列为空），则wait()会释放锁（lock）并将当前线程（消费者）挂起（进入等待状态），
        //      直到其他线程（生产者）调用 cv.notify_one() 或 cv.notify_all() 将其唤醒。
        // 2. 当线程被唤醒后，wait()会重新获取锁（lock），然后再次检查条件是否满足。
        // 3. 这样设计的目的是为了避免“虚假唤醒”（spurious wakeup），即线程可能在没有其他线程通知的情况下被操作系统唤醒，
        //    所以需要再次检查条件，确保队列确实不为空。
        // 4. 这种模式是条件变量的标准用法：在等待条件满足时，必须使用循环或带条件的wait来避免虚假唤醒。
        cv.wait(lock, [] { return !q.empty(); });

        int val = q.front();  // 从队列中取出数据
        q.pop();
        std::cout << "consume:" << val << std::endl;
        if (val == 4) break;  // 如果取到4，则退出循环，结束消费者线程
    }
}


int main(){
    std::thread t1(producer);  // 创建生产者线程
    std::thread t2(consumer);  // 创建消费者线程

    t1.join();  // 等待生产者线程结束
    t2.join();  // 等待消费者线程结束
    return 0;

}
