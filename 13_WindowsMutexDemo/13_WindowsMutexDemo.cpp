#include <Windows.h>
#include <string>
#include <iostream>

HANDLE g_hMutex = NULL;
int g_iResource = 0;

DWORD _stdcall WorkerThreadProc(LPVOID lpThreadParameter) {
    DWORD dwThreadID = GetCurrentThreadId();
    while (true) {
        if (WaitForSingleObject(g_hMutex, 1000) == WAIT_OBJECT_0) {
            g_iResource++;
            std::cout << "Thread:" << dwThreadID << " becomes mutex owner,ResourceNo:" << g_iResource << std::endl;
            ReleaseMutex(g_hMutex);
        }
        Sleep(1000);
    }
    return 0;
}

int main(){
    //创建一个匿名的Mutex对象并设置默认情况下主线程不拥有该Mutex
    g_hMutex = CreateMutex(NULL, FALSE, NULL);
    HANDLE hWorkerThread[5];
    for (int i = 0; i < 5; ++i) {
        //HANDLE CreateThread(
        //    LPSECURITY_ATTRIBUTES   lpThreadAttributes,  // 线程安全属性，一般设置为NULL
        //    SIZE_T                  dwStackSize,         // 线程栈空间大小，一般指定为0，表示默认大小
        //    LPTHREAD_START_ROUTINE  lpStartAddress,      // 线程函数，注意此处需要显示指定_stdcall调用类型
        //    LPVOID                  lpParameter,         // 传给线程函数的参数
        //    DWORD                   dwCreationFlags,     // 默认为0表示创建好线程之后立即执行，不希望立即执行，设置为4
        //    LPDWORD                 lpThreadId           // 线程创建成功返回的线程ID
        //);
        hWorkerThread[i] = CreateThread(NULL, 0, WorkerThreadProc, NULL, 0, NULL);
    }

    for (int i = 0; i < 5; ++i) {
        WaitForSingleObject(hWorkerThread[i], INFINITE);
        CloseHandle(hWorkerThread[i]);
    }
    CloseHandle(g_hMutex);
    return 0;
}

