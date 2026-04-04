#include <time.h>
#include <stdio.h>

void test1(){
    time_t tNow = time(NULL);
    time_t tEnd = tNow + 1800;
    //注意下面两行的区别
    struct tm* ptm = localtime(&tNow);
    struct tm* ptmEnd = localtime(&tEnd);

    char szTmp[50] = { 0 };
    strftime(szTmp, 50, "%H:%M:%S", ptm);

    //struct tm* ptmEnd = localtime(&tEnd);
    char szEnd[50] = { 0 };
    strftime(szEnd, 50, "%H:%M:%S", ptmEnd);
    printf("%s \n", szTmp);
    printf("%s \n", szEnd);
}

void test2() {
    time_t tNow = time(NULL);
    time_t tEnd = tNow + 1800;
    //注意下面两行的区别
    struct tm* ptm = localtime(&tNow);

    char szTmp[50] = { 0 };
    strftime(szTmp, 50, "%H:%M:%S", ptm);

    struct tm* ptmEnd = localtime(&tEnd);
    char szEnd[50] = { 0 };
    strftime(szEnd, 50, "%H:%M:%S", ptmEnd);
    printf("%s \n", szTmp);
    printf("%s \n", szEnd);
}

int main(){
    test1();
    test2();

    return 0;
}
