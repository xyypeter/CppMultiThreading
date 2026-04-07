#include "TaskPool.h"

TaskPool::TaskPool():m_bRunning(false){

}

TaskPool::~TaskPool(){
    removeAllTasks();
}

//初始化线程池(关键点如下)
//1.创建多个线程
//2.每个线程执行同一个函数 threadFunc
//3.线程池本质：多个 worker + 一个共享队列
void TaskPool::init(int threadNum){
    if (threadNum <= 0)
        threadNum = 5;
    m_bRunning = true;
    //添加工作线程
    for (int i = 0; i < threadNum; ++i) {
        std::shared_ptr<std::thread> spThread;
        //C++11的std::thread类对函数签名没有限制,即使类成员函数是类的实例方法也可以,但是必须显式地将线程函数所属的类对象实例指针
        spThread.reset(new std::thread(std::bind(&TaskPool::threadFunc, this)));
        m_threads.push_back(spThread);
    }
}

void TaskPool::threadFunc() {
    std::shared_ptr<Task> spTask;
    while (true) {
        std::unique_lock<std::mutex> guard(m_mutexList);//保护任务队列防止并发冲突
        while (m_taskList.empty()) {//使用while防止虚假唤醒
            if (!m_bRunning) 
                break;
            //如果获得了互斥锁，但是条件不满足的话(队列为空)，m_cv.wait()调用会释放锁，且挂起当前线程，因此不往下执行
            //当发生变化后，条件满足，m_cv.wait() 将唤醒挂起的线程，且获得锁
            m_cv.wait(guard);
            //本质为
            //1.释放 mutex
            //2.挂起线程
            //3.被唤醒后重新加锁
        }
        //如果线程池已经停止，直接跳出
        if (!m_bRunning) 
            break;
        //取任务
        spTask = m_taskList.front();
        m_taskList.pop_front();
        if (spTask == NULL) 
            continue;
        //此处设计略有缺陷:
        //1.此时 锁还没释放！
        //2.所有线程执行任务时都持锁
        //3.任务执行慢 → 阻塞其他线程取任务
        //guard.unlock();   // 释放锁(建议添加)
        spTask->doIt();
        spTask.reset();
    }
    std::cout << "exit thread,threadID:" << std::this_thread::get_id() << std::endl;
}

void TaskPool::stop() {
    m_bRunning = false;
    m_cv.notify_all(); //唤醒所有线程让其退出
    for (auto& iter : m_threads) {
        if (iter->joinable())
            iter->join();
    }
}

//添加任务(生产者)
//主线程 → addTask → 放任务 → 唤醒 worker
void TaskPool::addTask(Task* task) {
    std::shared_ptr<Task> spTask;
    spTask.reset(task);//任务队列指针管理task指针
    {
        std::lock_guard<std::mutex> guard(m_mutexList);
        m_taskList.push_back(spTask);
        std::cout << "add a Task." << std::endl;
    }
    // 唤醒一个线程
    m_cv.notify_one();
}

void TaskPool::removeAllTasks(){
    std::lock_guard<std::mutex> guard(m_mutexList);
    for (auto& iter : m_taskList){
        iter.reset();  // 释放任务
    }
    m_taskList.clear();
}


