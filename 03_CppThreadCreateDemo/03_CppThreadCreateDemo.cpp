#include <iostream>
#include <stdio.h>
#include <thread>

void threadproc1() {
    while (true) {
        printf("I am New thread1");
    }
}

void threadproc2(int a, int b) {
    while (true) {
        printf("I am New thread2");
    }
}

int main() {
    std::thread t1(&threadproc1);
    std::thread t2(&threadproc2, 1, 2);
    while (true) {

    }
    return 0;
}

