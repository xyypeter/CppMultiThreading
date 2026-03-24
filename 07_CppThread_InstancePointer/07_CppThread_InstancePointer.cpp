#include <thread>
#include <memory>
#include <stdio.h>

class Thread {
public:
    Thread() : m_stopped(true) { }
    ~Thread() {
        Stop();  // 确保线程结束
    }
    void Start() {
        m_stopped = false;
        m_spThread.reset(new std::thread(&Thread::threadFunc, this, 8888, 9999));
        //.reset含义为 重置智能指针，让它管理一个新创建的 std::thread 对象
        //threadFunc是类的非静态方法，所以作为线程函数，第一个参数必须传递类实例地址，即this指针
    }
    void Stop(){
        printf("Stopping thread...\n");
        m_stopped = true;
        if (m_spThread)
        {
            if (m_spThread->joinable())
                m_spThread->join();
        }
    }
private:
    //线程函数
    void threadFunc(int arg1, int arg2) {
        while (!m_stopped) {
            printf("Thread function use instance method!");
        }
    }
private:
    std::shared_ptr<std::thread> m_spThread;//线程智能指针
    bool                         m_stopped; //线程停止标志
};


int main(){
    Thread mythread;
    mythread.Start();
    //权宜之计，让主线程不要提前退出
    while (true){

    }
    return 0;
}