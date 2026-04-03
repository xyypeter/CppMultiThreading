#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <iostream>

//任务类
class Task {
public:
    Task(int taskID) {
        this->taskID = taskID;
    }
    void doTask() {
        std::cout << "Handle a task,taskID:" << taskID << ",threadID:" << std::this_thread::get_id() << std::endl;
    }
private:
    int taskID;
};

std::mutex mymutex;
std::list<Task*> tasks;      //任务列表
std::condition_variable mycv;//条件变量

void* consumer_thread() {
    Task* pTask = nullptr;
    while (true) {
        std::unique_lock<std::mutex> guard(mymutex);
        while (tasks.empty()) {
            //如果获得了互斥锁，但是条件不合适的话，pthread_cond_wait会释放锁，不往下执行。
            //当发生变化后，条件合适，pthread_cond_wait将直接获得锁。
            mycv.wait(guard);
        }
        pTask = tasks.front();
        tasks.pop_front();
        if (pTask == nullptr)
            continue;
        pTask->doTask();
        delete pTask;
        pTask = nullptr;
    }
    return NULL;
}

void* producer_thread(){
    int taskID = 0;
    Task* pTask = NULL;
    while (true){
        pTask = new Task(taskID);
        //使用括号减小guard锁的作用范围(如果不想这样写,手动提前释放锁也可以)
        {
            std::lock_guard<std::mutex> guard(mymutex);
            tasks.push_back(pTask);
            std::cout << "produce a task, taskID: " << taskID << ", threadID: " << std::this_thread::get_id() << std::endl;
        }
        //释放信号量，通知消费者线程
        mycv.notify_one();
        taskID++;
        //休眠1秒
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return NULL;
}

int main(){
    // 创建5个消费者线程
    std::thread consumer1(consumer_thread);
    std::thread consumer2(consumer_thread);
    std::thread consumer3(consumer_thread);
    std::thread consumer4(consumer_thread);
    std::thread consumer5(consumer_thread);

    //创建一个生产者线程
    std::thread producer(producer_thread);

    producer.join();
    consumer1.join();
    consumer2.join();
    consumer3.join();
    consumer4.join();
    consumer5.join();

    return 0;
}

