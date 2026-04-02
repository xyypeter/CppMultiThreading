#include <Windows.h>
#include <string>
#include <iostream>
#include <list>
#include <time.h>

HANDLE g_hMsgSemaphore = NULL;       //信号量 生产者添加消息时增加信号量计数 消费者等待信号量，有消息时才尝试读取
std::list<std::string> g_listChatMsg;//存储聊天消息的队列
CRITICAL_SECTION g_csMsg;            //临界区对象-保护g_listChatMsg临时区对象

DWORD __stdcall NetThreadProc(LPVOID lpThreadParameter) {
    int nMsgIndex = 0;
    while (true) {
        EnterCriticalSection(&g_csMsg);
        //随机产生1~4条消息
        int count = rand() % 4 + 1;
        for (int i = 0; i < count; ++i) {
            nMsgIndex++;
            SYSTEMTIME st;
            GetLocalTime(&st);
            char szChatMsg[64] = { 0 };
            sprintf_s(szChatMsg, 64, "[%04d-%02d-%02d %02d:%02d:%02d:%03d] A new msg, NO.%d.",
                st.wYear,
                st.wMonth,
                st.wDay,
                st.wHour,
                st.wMinute,
                st.wSecond,
                st.wMilliseconds,
                nMsgIndex);
            g_listChatMsg.emplace_back(szChatMsg);
        }
        LeaveCriticalSection(&g_csMsg);
        ReleaseSemaphore(g_hMsgSemaphore, count, NULL); 
    }
    return 0;
}

DWORD __stdcall ParseThreadProc(LPVOID lpThreadParemeter) {
    DWORD dwThreadID = GetCurrentThreadId();
    std::string current;
    while (true) {
        if (WaitForSingleObject(g_hMsgSemaphore, INFINITE) == WAIT_OBJECT_0) {
            EnterCriticalSection(&g_csMsg);
            if (!g_listChatMsg.empty()) {
                current = g_listChatMsg.front();
                g_listChatMsg.pop_front();
                std::cout << "Thread:" << dwThreadID << "parse msg:" << current << std::endl;
            }
            LeaveCriticalSection(&g_csMsg);
        }
    }
    return 0;
}

int main(){
    srand(time(NULL));                  //初始化随机种子
    InitializeCriticalSection(&g_csMsg);//初始化临界区对象
    g_hMsgSemaphore = CreateSemaphore(NULL, 0, INT_MAX, NULL);                    //初始化信号量
    HANDLE hNetThread = CreateThread(NULL, 0, NetThreadProc, NULL, 0, NULL);      //网络信息线程
    HANDLE hWorkerThreads[4];
    for (int i = 0; i < 4; i++) {
        hWorkerThreads[i] = CreateThread(NULL, 0, ParseThreadProc, NULL, 0, NULL);//工作线程
    }
    for (int i = 0; i < 4; i++) {                                                 //等待线程并推出
        WaitForSingleObject(hWorkerThreads[i], INFINITE);
        CloseHandle(hWorkerThreads[i]);
    }
    WaitForSingleObject(hNetThread, INFINITE);
    CloseHandle(hNetThread);
    CloseHandle(g_hMsgSemaphore);
    DeleteCriticalSection(&g_csMsg);
    return 0;
}


