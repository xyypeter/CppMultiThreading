#pragma once
#include <thread>
#include <mutex>
#include <condition_variable>
#include <list>
#include <vector>
#include <memory>
#include <iostream>
#include <functional>

//任务抽象
class Task {
public:
    // 任务执行函数(多态)
    virtual void doIt() {
        std::cout << "handle a task..." << std::endl;
    }
    // 虚析构:保证子类正确释放
    virtual ~Task() {
        std::cout << "a task destructed..." << std::endl;
    }
};

//线程池类(final表示该类不能被继承，即不能有派生类)
class TaskPool final {
public:
    TaskPool();
    ~TaskPool();
    TaskPool(const TaskPool& rhs) = delete;
    TaskPool& operator=(const TaskPool& rhs) = delete;
public:
    void init(int threadNum = 5);  //初始化线程池
    void stop();                   //停止线程池           
    void addTask(Task* task);      //添加任务
    void removeAllTasks();         //移除任务
private:
    void threadFunc();             //线程函数
private:
    std::list<std::shared_ptr<Task>> m_taskList;         //任务队列(双向链表)
    std::mutex m_mutexList;                              //保护任务队列(防止并发冲突)
    std::condition_variable m_cv;                        //线程阻塞/唤醒机制(避免繁忙等)
    bool m_bRuning;                                      //控制线程池是否运行
    std::vector<std::shared_ptr<std::thread>> m_threads; //存放工作线程
};