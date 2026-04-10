#include "TaskPool.h"
#include <functional>

TaskPool::TaskPool() :m_bRuning(false) {

}

TaskPool::~TaskPool() {
    removeAllTasks();
}

void TaskPool::threadFunc() {
    std::shared_ptr<Task> spTask;
    while (true) {
        std::unique_lock<std::mutex> guard(m_mutexList);
        while (m_taskList.empty()) {
            if (!m_bRuning) break;
            m_cv.wait(guard);
            //队列为空条件不满足,m_cv.wait()先释放锁,且挂起当前线程
            //当发生变化后，条件满足，m_cv.wait() 将唤醒挂起的线程，且获得锁
        }
        if (!m_bRuning) break;
        //取出任务
        spTask = m_taskList.front();
        m_taskList.pop_front();
        //执行任务
        if (spTask == NULL) continue;
        spTask->doIt();
        //释放智能指针管理的对象引用计数,但是此处并不是必须的,因为下一次循环spTask会被新的任务覆盖,旧任务会自动释放,更多是编码规范
        spTask.reset();
    }
    std::cout << "exit thread,threadID:" << std::this_thread::get_id() << std::endl;
}

void TaskPool::init(int threadNum) {
    if (threadNum <= 0)
        threadNum = 5;
    m_bRuning = true;
    //添加工作线程(创建线程并让智能指针管理)
    for (int i = 0; i < threadNum; i++) {
        //老式写法
        //std::shared_ptr<std::thread> spThread;
        //spThread.reset(new std::thread(std::bind(&TaskPool::threadFunc, this)));
        //m_threads.push_back(spThread);

        //新写法
        m_threads.push_back(std::make_shared<std::thread>(&TaskPool::threadFunc, this));
        //std::thread 的构造函数可以直接接受 成员函数指针 + 对象指针，不需要 std::bind
        //    &TaskPool::threadFunc 是成员函数指针
        //    this 是对象指针
        //    std::thread 会自动调用 this->threadFunc()
        //std::make_shared<std::thread>(...) 会创建一个 std::shared_ptr<std::thread> 并直接初始化，不需要先声明再 reset。
        //
    }
}

void TaskPool::stop() {
    m_bRuning = false;//标志位置为false
    m_cv.notify_all();//唤醒所有线程让其退出
    for (auto& iter : m_threads) {
        if (iter->joinable()) iter->join();
    }
}

void TaskPool::addTask(Task* task) {
    //老式写法
    //std::shared_ptr<Task> spTask;
    //spTask.reset(task);//任务队列指针管理task指针

    //现代写法
    auto spTask = std::shared_ptr<Task>(task);
    {
        std::lock_guard<std::mutex> guard(m_mutexList);
        m_taskList.push_back(spTask);
        std::cout << "add a Task" << std::endl;
    }
    m_cv.notify_one();//条件变量对象唤醒一个线程
}

void TaskPool::removeAllTasks() {
    std::lock_guard<std::mutex> guard(m_mutexList);
    for (auto& iter : m_taskList) {
        iter.reset();  // 释放任务
    }
    m_taskList.clear();
}


