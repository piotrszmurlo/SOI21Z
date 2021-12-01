#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <queue>
#include <unistd.h>
#include <sstream>

#define N 10                //max buffer size
#define PROD_SLEEP_TIME 0      //producer sleep time (miliseconds)
#define CONS_SLEEP_TIME 500      //consumer sleep time (miliseconds)
#define MAX_PROD_COUNT 20  //max iterations per producer (even numbers only)

std::queue<int> b1;
std::queue<int> b2;
std::queue<int> b3;

int b1_count = 0;
int b2_count = 0;
int b3_count = 0;

pthread_mutex_t b1_mutex;
pthread_mutex_t b2_mutex;
pthread_mutex_t b3_mutex;

sem_t b1_full;
sem_t b1_empty;
sem_t b2_full;
sem_t b2_empty;
sem_t b3_full;
sem_t b3_empty;

void *producer1(void * args){
    bool switch_buf = true; 
    int i = 0;
    while(i < MAX_PROD_COUNT) {
        int x = 2*(rand() % 50);
        if (switch_buf) {
            sem_wait(&b1_empty);
            pthread_mutex_lock(&b1_mutex);
            b1.push(x);
            b1_count++;
            pthread_mutex_unlock(&b1_mutex);
            sem_post(&b1_full);
        }
        else {
            sem_wait(&b2_empty);
            pthread_mutex_lock(&b2_mutex);
            b2.push(x);
            b2_count++;
            pthread_mutex_unlock(&b2_mutex);
            sem_post(&b2_full);
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
            sem_wait(&b3_empty);
            pthread_mutex_lock(&b3_mutex);
            b3.push(x);
            b3_count++;
            pthread_mutex_unlock(&b3_mutex);
            sem_post(&b3_full);
        }
        else {
            sem_wait(&b2_empty);
            pthread_mutex_lock(&b2_mutex);
            b2.push(x);
            b2_count++;
            pthread_mutex_unlock(&b2_mutex);
            sem_post(&b2_full);
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
        int y;
        int size;
        std::stringstream msg;
        sem_wait(&b1_full);
        pthread_mutex_lock(&b1_mutex);
        y = b1.front();
        b1.pop();
        b1_count--;
        size = b1.size();
        pthread_mutex_unlock(&b1_mutex);
        sem_post(&b1_empty);
        msg << "C1: received " << y << "; current buffer1 size: "<< size <<std::endl;
        std::cout << msg.str();
        i++;
        if (CONS_SLEEP_TIME) usleep(CONS_SLEEP_TIME*1000);
    }
    return NULL;
}

void *consumer2(void *args) {
    int i  = 0;
    while (i < MAX_PROD_COUNT) {
        int y;
        int size;
        std::stringstream msg;
        sem_wait(&b2_full);
        pthread_mutex_lock(&b2_mutex);
        y = b2.front();
        b2.pop();
        b2_count--;
        size = b2.size();
        pthread_mutex_unlock(&b2_mutex);
        sem_post(&b2_empty);
        msg << "C2: received " << y << "; current buffer2 size: " << size << std::endl;
        std::cout << msg.str();
        i++;
        if (CONS_SLEEP_TIME) usleep(CONS_SLEEP_TIME*1000);      
    }
    return NULL;
}

void *consumer3(void *args) {
    int i = 0;
    while (i < MAX_PROD_COUNT/2) {
        int y;
        int size;
        std::stringstream msg;
        sem_wait(&b3_full);
        pthread_mutex_lock(&b3_mutex);
        y = b3.front();
        b3.pop();
        b3_count--;
        size = b3.size();
        pthread_mutex_unlock(&b3_mutex);
        sem_post(&b3_empty);
        msg << "C3: received " << y << "; current buffer3 size: "<< size << std::endl;
        std::cout << msg.str();
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

    pthread_mutex_init(&b1_mutex, NULL);
    pthread_mutex_init(&b2_mutex, NULL);
    pthread_mutex_init(&b3_mutex, NULL);

    sem_init(&b1_empty, 0, N);
    sem_init(&b1_full, 0, 0);
    sem_init(&b2_empty, 0, N);
    sem_init(&b2_full, 0, 0);
    sem_init(&b3_empty, 0, N);
    sem_init(&b3_full, 0, 0);

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

    pthread_mutex_destroy(&b1_mutex);
    pthread_mutex_destroy(&b2_mutex);
    pthread_mutex_destroy(&b3_mutex);

    sem_destroy(&b1_empty);
    sem_destroy(&b1_full);
    sem_destroy(&b2_empty);
    sem_destroy(&b2_full);
    sem_destroy(&b3_empty);
    sem_destroy(&b3_full);

    return 0;
}