#include <thread>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <vector>
#include <memory>

std::mutex mymutex;
std::condition_variable mycv;
bool success = false;

void thread_func(int no) {
    {
        std::unique_lock<std::mutex> lock(mymutex);
        success = true;
        mycv.notify_all();//通知所有正在等待此条件变量的线程(使用one更好一些)
    }
    std::cout << "worker thread started, threadNO: " << no << std::endl;
    //实际的线程执行的工作代码放在下面
    //这里为了模拟方便，简单地写个死循环
    while (true){}
}

int main(){
    //主线程逐个创建子线程，并通过条件变量确认“子线程已经启动成功”，再继续创建下一个线程
    std::vector<std::shared_ptr<std::thread>> threads;
    for (int i = 0; i < 5; i++) {
        success = false;
        std::shared_ptr<std::thread> spthread;
        //声明智能指针:类型：shared_ptr 管理对象 管理对象：std::thread 当前状态spthread = nullptr
        spthread.reset(new std::thread(thread_func, i));
        //1. 创建线程对象
        //2. spthread.reset(ptr);等价于 delete 旧对象（如果有） spthread = ptr
        //3. shared_ptr建立引用计数


        //使用花括号减小锁的粒度
        {
            std::unique_lock<std::mutex> lock(mymutex);
            //与子线程
            //success = true;
            //mycv.notify_all();
            //优秀的启动确认机制
            while (!success){
                mycv.wait(lock);
            }
        }
        std::cout << "start thread successfully， index: " << i << std::endl;
        threads.push_back(spthread);
    }
    for (auto& iter : threads){
        iter->join();
    }
    return 0;
}

