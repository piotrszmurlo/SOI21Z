#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <unistd.h>
#include <sstream>
#include "smartbuffer.h"

#define N 10                     //max buffer size
#define PROD_SLEEP_TIME 125        //producer sleep time (miliseconds)
#define CONS_SLEEP_TIME 500      //consumer sleep time (miliseconds)
#define MAX_PROD_COUNT 20        //max iterations per producer (even numbers only)

Smartbuffer b1(N, 1);
Smartbuffer b2(N, 2);
Smartbuffer b3(N, 3);

void *producer1(void * args){
    bool switch_buf = true; 
    int i = 0;
    while(i < MAX_PROD_COUNT) {
        int x = 2*(rand() % 50);
        if (switch_buf) {
            b1.produce(x, 1);
          
        }
        else {
            b2.produce(x, 1);
        }
        i++;
        switch_buf = !switch_buf;
        if (PROD_SLEEP_TIME) usleep(PROD_SLEEP_TIME*1000);

    }
    return NULL;
}

void *producer2(void * args) {
    bool switch_buf = true; 
    int i = 0;
    while(i < MAX_PROD_COUNT) {
        int x = 2*(rand() % 50) + 1;
        if (switch_buf) {
            b3.produce(x, 2);
        }
        else {
            b2.produce(x, 2);
        }
        i++;
        switch_buf = !switch_buf;
        if (PROD_SLEEP_TIME) usleep(PROD_SLEEP_TIME*1000);
    }
    return NULL;
}

void *consumer1(void *args) {
    int i = 0;
    while (i < MAX_PROD_COUNT/2) {
        b1.consume(1);
        i++;
        if (CONS_SLEEP_TIME) usleep(CONS_SLEEP_TIME*1000);
    }
    return NULL;
}

void *consumer2(void *args) {
    int i  = 0;
    while (i < MAX_PROD_COUNT) {
        b2.consume(2);
        i++;
        if (CONS_SLEEP_TIME) usleep(CONS_SLEEP_TIME*1000);      
    }
    return NULL;
}

void *consumer3(void *args) {
    int i = 0;
    while (i < MAX_PROD_COUNT/2) {
        b3.consume(3);
        i++;
        if (CONS_SLEEP_TIME) usleep(CONS_SLEEP_TIME*1000);
    }
    return NULL;
}


int main(void) {
    if (MAX_PROD_COUNT%2) {
        std::cout << "set MAX_PROD_COUNT to even number" << std::endl;
        return -1;
    }
    srand(time(NULL));
    pthread_t prod1;
    pthread_t prod2;

    pthread_t cons1;
    pthread_t cons2;
    pthread_t cons3;

    if (pthread_create(&cons1, NULL, &consumer1, NULL) != 0) {
        perror("Thread creation error");
    }
    if (pthread_create(&cons2, NULL, &consumer2, NULL) != 0) {
        perror("Thread creation error");
    }
    if (pthread_create(&cons3, NULL, &consumer3, NULL) != 0) {
        perror("Thread creation error");
    }
    if (pthread_create(&prod1, NULL, &producer1, NULL) != 0) {
        perror("Thread creation error");
    }
    if (pthread_create(&prod2, NULL, &producer2, NULL) != 0) {
        perror("Thread creation error");
    }
    if (pthread_join(prod1, NULL) != 0) {
        perror("Thread join error");
    }
    if (pthread_join(prod2, NULL) != 0) {
        perror("Thread join error");
    }
    if (pthread_join(cons1, NULL) != 0) {
        perror("Thread join error");
    }
    if (pthread_join(cons2, NULL) != 0) {
        perror("Thread join error");
    }
    if (pthread_join(cons3, NULL) != 0) {
        perror("Thread join error");
    }

    return 0;
}