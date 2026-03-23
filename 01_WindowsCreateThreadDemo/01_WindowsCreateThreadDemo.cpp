#include <stdio.h>
#include <windows.h>
DWORD WINAPI ThreadProc(LPVOID lpParameters) {
    while (true) {
        Sleep(1);//睡眠1秒
        printf("I am New Thread!\n");
    }
}

int main() {
    DWORD dwThreadID;
    //HANDLE CreateThread(
    //    LPSECURITY_ATTRIBUTES   lpThreadAttributes,  // 线程安全属性，一般设置为NULL
    //    SIZE_T                  dwStackSize,         // 线程栈空间大小，一般指定为0，表示默认大小
    //    LPTHREAD_START_ROUTINE  lpStartAddress,      // 线程函数，注意此处需要显示指定_stdcall调用类型
    //    LPVOID                  lpParameter,         // 传给线程函数的参数
    //    DWORD                   dwCreationFlags,     // 默认为0表示创建好线程之后立即执行，不希望立即执行，设置为4
    //    LPDWORD                 lpThreadId           // 线程创建成功返回的线程ID
    //);
    HANDLE hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, &dwThreadID);
    if (hThread == NULL) {
        printf("Failed to CreateThread!");
    }
    //权宜之计，让主线程不要提前退出
    while (true) {
        Sleep(1000);
    }
    return 0;
}

