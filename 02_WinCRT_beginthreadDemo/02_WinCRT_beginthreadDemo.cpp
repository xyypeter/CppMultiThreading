#include <process.h>
#include <stdio.h>

unsigned int __stdcall threadfun(void* arg) {
    while (true) {
        printf("I am New Thread\n");
    }
}

int main() {
    unsigned int threadid;
    _beginthreadex(0, 0, threadfun, 0, 0, &threadid);
    while (true) {

    }
    return 0;
}

