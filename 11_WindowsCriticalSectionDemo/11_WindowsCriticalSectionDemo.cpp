#include <Windows.h>
#include <list>
#include <iostream>
#include <string>

CRITICAL_SECTION g_cs;       //创建临界区对象
int             g_number = 0;

//线程函数:注意windows的线程函数需要显示指定_stdcall调用类型
DWORD _stdcall WorkerThreadProc(LPVOID lpThreadParameter) {
    DWORD dwThreadID = GetCurrentThreadId();
    while (true) {
        EnterCriticalSection(&g_cs);//进入临界区加锁
        std::cout << "EnterCriticalSection,ThreadID:" << dwThreadID << std::endl;
        g_number++;
        //获取当前系统时间
        SYSTEMTIME st;
        GetLocalTime(&st);
        char szMsg[64] = { 0 };
        sprintf(szMsg, "[%04d-%02d-%02d %02d:%02d:%02d:%03d]NO.%d,ThreadID:%d.",
            st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, g_number, dwThreadID);
        std::cout << szMsg << std::endl;
        std::cout << "LeaceCriticalSection,ThreadID:" << dwThreadID << std::endl;
        LeaveCriticalSection(&g_cs);
        Sleep(1);//睡眠一秒
    }
    return 0;
}

int main(){
    InitializeCriticalSection(&g_cs);
    //HANDLE CreateThread(
    //    LPSECURITY_ATTRIBUTES   lpThreadAttributes,  // 线程安全属性，一般设置为NULL
    //    SIZE_T                  dwStackSize,         // 线程栈空间大小，一般指定为0，表示默认大小
    //    LPTHREAD_START_ROUTINE  lpStartAddress,      // 线程函数，注意此处需要显示指定_stdcall调用类型
    //    LPVOID                  lpParameter,         // 传给线程函数的参数
    //    DWORD                   dwCreationFlags,     // 默认为0表示创建好线程之后立即执行，不希望立即执行，设置为4
    //    LPDWORD                 lpThreadId           // 线程创建成功返回的线程ID
    //);
    HANDLE hWorkerThread1 = CreateThread(NULL, 0, WorkerThreadProc, NULL, 0, NULL);
    HANDLE hWorkerThread2 = CreateThread(NULL, 0, WorkerThreadProc, NULL, 0, NULL);
    WaitForSingleObject(hWorkerThread1, INFINITE); //INFINITE表示无限等待
    WaitForSingleObject(hWorkerThread2, INFINITE);
    //关闭线程句柄
    CloseHandle(hWorkerThread1);
    CloseHandle(hWorkerThread2);
    //释放临界区对象
    DeleteCriticalSection(&g_cs);
   
    return 0;
}

