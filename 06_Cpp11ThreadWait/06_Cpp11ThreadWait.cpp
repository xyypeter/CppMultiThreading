#include <iostream>
#include <stdio.h>
#include <string>
#include <time.h>
#include <windows.h>

#define TIME_FILENAME "time.txt"

DWORD WINAPI FileThreadFunc(LPVOID lpParameters) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char timeStr[32] = { 0 };
    sprintf_s(timeStr, 32, "%04d/%02d/%02d/ %02d:%02d:%02d",
        t->tm_year + 1900,
        t->tm_mon + 1,
        t->tm_mday,
        t->tm_hour,
        t->tm_min,
        t->tm_sec);
    FILE* fp = fopen(TIME_FILENAME, "w");
    if (fp == NULL) {
        printf("Faild to create time.txt\n");
        return 1;
    }
    size_t sizeToWrite = strlen(timeStr) + 1;
    size_t ret = fwrite(timeStr, 1, sizeToWrite, fp);
    if (ret != sizeToWrite) {
        printf("write file error");
    }
    fclose(fp);
    return 2;
}

int main() {
    DWORD dwfileThreadID;
    HANDLE hFileThread = CreateThread(NULL, 0, FileThreadFunc, NULL, 0, &dwfileThreadID);
    if (hFileThread == NULL) {
        printf("Failed to create fileThread\n");
        return -1;
    }
    //无线等待知道文件线程退出，否则程序将一直挂起
    WaitForSingleObject(hFileThread, INFINITE);
    //打开文件读取
    FILE* fp = fopen(TIME_FILENAME, "r");//r选项要求文件必须存在
    if (fp == NULL) {
        printf("open file error\n");
        return -2;
    }
    char buf[32] = { 0 };
    int sizeRead = fread(buf, 1, 32, fp);
    if (sizeRead == 0) {
        printf("read file error \n");
        fclose(fp);
        return -3;
    }
    printf("current Tile is:%s.\n", buf);
    fclose(fp);
    return 0;
}

