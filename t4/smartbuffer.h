#include <sys/types.h> 
#include <sys/stat.h> 
#include <string.h> 
#include <errno.h> 
#include <fcntl.h> 
#include <pthread.h> 
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h> 
#include <stdlib.h> 
#include "monitor.h"
#include <queue>
#include <sstream>
#include <iostream>

class Smartbuffer : Monitor
{
    std::queue<int> buffer;
    int bufferSize;
    int bufferID;
    Condition full;
    Condition empty;

    public:
    Smartbuffer(int bufferSize, int bufferID) {
        this->bufferSize = bufferSize;
        this->bufferID = bufferID;
    }

    void produce(int product, int producerID) {
        enter();
        if (buffer.size() == bufferSize) {
            wait(full);
        }
        std::stringstream msg;
        buffer.push(product);
        msg << "P" << producerID << ": produced " << product << " into buffer" << bufferID << "; size: " << buffer.size() << std::endl;
        std::cout << msg.str();
        signal(empty);
        leave();
    }

    void consume(int consumerID) {
        enter();
        if (buffer.size() == 0) {
            wait(empty);
        }
        std::stringstream msg;
        int product = buffer.front();
        buffer.pop();
        msg << "C" << consumerID <<": received " << product << "; size: " << buffer.size() << std::endl;
        std::cout << msg.str();  
        signal(full);
        leave();
    }
};