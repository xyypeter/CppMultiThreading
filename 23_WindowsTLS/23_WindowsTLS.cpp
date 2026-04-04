#include <iostream>
#include <Windows.h>

// 定义一个线程局部存储变量
// 每个线程都会有自己独立的一份 g_mydata，初始值都是 1
__declspec(thread) int g_mydata = 1;

// 线程1：不断递增 g_mydata
DWORD __stdcall WorkerThreadProc1(LPVOID lpThreadParameter) {
    while (true) {
        ++g_mydata;        // 修改的是“当前线程自己的 g_mydata”
        Sleep(1000);       // 每隔 1 秒执行一次
    }
    return 0;
}

// 线程2：不断打印 g_mydata
DWORD __stdcall WorkerThreadProc2(LPVOID lpThreadParameter) {
    while (true) {
        std::cout
            << "g_mydata = " << g_mydata   // 打印的是“当前线程自己的 g_mydata”
            << ", ThreadID = " << GetCurrentThreadId() // 打印线程ID
            << std::endl;

        Sleep(1000);       // 每隔 1 秒执行一次
    }
    return 0;
}

int main() {

    // 创建两个线程
    HANDLE hWorkerThreads[2];

//HANDLE CreateThread(
//    LPSECURITY_ATTRIBUTES   lpThreadAttributes,  // 线程安全属性，一般设置为NULL
//    SIZE_T                  dwStackSize,         // 线程栈空间大小，一般指定为0，表示默认大小
//    LPTHREAD_START_ROUTINE  lpStartAddress,      // 线程函数，注意此处需要显示指定_stdcall调用类型
//    LPVOID                  lpParameter,         // 传给线程函数的参数
//    DWORD                   dwCreationFlags,     // 默认为0表示创建好线程之后立即执行，不希望立即执行，设置为4
//    LPDWORD                 lpThreadId           // 线程创建成功返回的线程ID
//);

    // 线程1：负责修改 g_mydata
    hWorkerThreads[0] = CreateThread(
        NULL, 0,
        WorkerThreadProc1,
        NULL,
        0,
        NULL
    );

    // 线程2：负责打印 g_mydata
    hWorkerThreads[1] = CreateThread(
        NULL, 0,
        WorkerThreadProc2,
        NULL,
        0,
        NULL
    );

    // 关闭线程句柄（只是关闭句柄，不影响线程运行）
    CloseHandle(hWorkerThreads[0]);
    CloseHandle(hWorkerThreads[1]);

    // 主线程什么都不做，只是保持进程不退出
    while (true)
    {
        Sleep(1000);
    }

    return 0;
}

