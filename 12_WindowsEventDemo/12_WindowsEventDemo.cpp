#include <Windows.h>
#include <string>
#include <iostream>

bool        g_bTaskCompleted = false;//任务完成标识
std::string g_TaskResult;            //任务结果字符串
HANDLE      g_hTaskEvent = NULL;     //事件对象句柄

DWORD __stdcall WorkerThreadProc(LPVOID lpThreadParameter) {
    //使用Sleep函数模拟一个很耗时的操作
    //睡眠3秒
    Sleep(3000);
    g_TaskResult = "task completed";
    g_bTaskCompleted = true;
    SetEvent(g_hTaskEvent);//设置事件信号为受信状态
    return 0;
}

int main() {
    g_hTaskEvent = CreateEvent(NULL, TRUE, FALSE, NULL);         //创建一个匿名的手动重置初始无信号的事件对象
    HANDLE hWorkerThread = CreateThread(NULL, 0, WorkerThreadProc, NULL, 0, NULL);
    DWORD dwResult = WaitForSingleObject(g_hTaskEvent, INFINITE);//阻塞等待g_hTaskEvent信号(此处线程会挂起，不占CPU)
    if (dwResult == WAIT_OBJECT_0){
        std::cout << g_TaskResult << std::endl;
    }

    //while (true){
    //    if (g_bTaskCompleted){
    //        std::cout << g_TaskResult << std::endl;
    //        break;
    //    }
    //    else
    //        std::cout << "Task is in progress..." << std::endl;// 任务未完成，显示进行中状态
    //}
    CloseHandle(hWorkerThread);
    CloseHandle(g_hTaskEvent);
    return 0;
}



