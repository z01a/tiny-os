#include <iostream.h>

#include "queue.h"
#include "thread.h"
#include "section.h"

Queue::Queue() {
    this->head = NULL;
    this->tail = NULL;
}

Thread* ThreadQueue::GetThreadById(ID id) {
    LOCK();
    if(head == NULL) {
        return NULL;
    } else {
        Node* current = head;
        while(current != NULL) {
            Thread* thread = current->data;
            if(thread->getId() == id) {
                return thread;
            }

            current = current->next;
        }
    }

    UNLOCK();

    return NULL;
}

void SleepQueue::Tick() {
    LOCK();
    if(head == NULL) {
        return;
    } else {
        Node* current = head;
        while(current != NULL) {
            Thread* thread = current->data;
            thread->wakeUp();
            current = current->next;
        }
    }
    UNLOCK();
}

void Queue::enqueue(Thread* thread) {
    LOCK();
    if(thread == NULL) {
        return;
    }

    if(this->head == NULL) {
        this->head = new Node(thread);
        this->tail = this->head;
    } else {
        this->tail->next = new Node(thread);
        this->tail->next->prev = this->tail;
        this->tail = this->tail->next;
    }
    UNLOCK();
}

Thread* Queue::dequeue() {
    LOCK();
    if(this->head == NULL) {
        return NULL;
    }

    Node* current = this->head;
    this->head = this->head->next;

    if(this->head == NULL) {
        this->tail = NULL;
    }

    Thread* thread = current->data;
    
    delete current;

    UNLOCK();
    return thread;
}

int Queue::empty() {
    return (this->head == NULL);
}

Queue::~Queue() {
    // Empty
}