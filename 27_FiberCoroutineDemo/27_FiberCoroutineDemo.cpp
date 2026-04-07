#include <Windows.h>
#include <string>

char g_szTime[64] = { "time not set..." }; // 全局时间字符串（共享数据）

LPVOID mainWorkerFiber = NULL;  // 主纤程句柄
LPVOID pWorkerFiber = NULL;     // 子纤程句柄

void WINAPI workerFiberProc(LPVOID lpFiberParameter) {
    while (true){
        // 模拟耗时操作：获取当前时间
        SYSTEMTIME st;
        GetLocalTime(&st);
        // 格式化时间字符串
        wsprintfA(g_szTime, "%04d-%02d-%02d %02d:%02d:%02d",st.wYear, st.wMonth, st.wDay,st.wHour, st.wMinute, st.wSecond);
        printf("%s\n", g_szTime);
        // 这里本来应该切回主纤程,如果被注释掉 → 会导致死循环
        SwitchToFiber(mainWorkerFiber);
    }
}

int main() {
    //将当前线程转换为纤程(主纤程)
    mainWorkerFiber = ConvertThreadToFiber(NULL);
    int index = 0;
    while (index < 100) {
        ++index;
        //创建一个新的纤程
        pWorkerFiber = CreateFiber(0, workerFiberProc, NULL);
        if (pWorkerFiber == NULL) return -1;
        //切换至子纤程(执行workerFiberProc)
        SwitchToFiber(pWorkerFiber);
        //只有workerFiberProc子纤程切换为主纤程，下方代码才会执行
                
        memset(g_szTime, 0, sizeof(g_szTime));// 重置字符串
        strncpy(g_szTime, "time not set...", strlen("time not set..."));
        printf("%s\n", g_szTime);// 打印字符串
        Sleep(1000);
    }
    // 删除纤程
    DeleteFiber(pWorkerFiber);
    // 把纤程转换回普通线程
    ConvertFiberToThread();
    return 0;
}