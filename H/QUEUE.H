#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <stdlib.h>

#include "types.h"

class Thread;

struct Node {
    Node* next;
    Node* prev;
    Thread* data;

     Node(Thread* thread) {
         this->next = NULL;
         this->prev = NULL;
         this->data = thread;
     }
};

class Queue {
    public:
        Queue();
        void enqueue(Thread* thread);
        void Enqueue(Thread* thread) { this->enqueue(thread); }
        Thread* dequeue();
        Thread* Dequeue() { return this->dequeue(); }
        int empty();
        int Empty() { return this->empty(); }
        ~Queue();
    protected:
        Node* head;
        Node* tail;

};

class ThreadQueue : public Queue {
    public:
        Thread* GetThreadById(ID id);
};

class SleepQueue: public Queue {
    public:
        void Tick();
};

#endif