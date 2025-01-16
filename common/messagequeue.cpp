/* message queue - thread safe
https://www.geeksforgeeks.org/implement-thread-safe-queue-in-c/
*/

#include <cstdio>
#include <pthread.h>
#include "messagequeue.h"


void MessageQueue::clear() {
    // Acquire lock by constructor
    std::unique_lock<std::mutex> lock(m_mutex);
    // standard procedure: Swap the contents of myQueue with emptyQueue
    std::queue<Message*> emptyQueue;
    std::swap(m_queue, emptyQueue);
}

// Pushes an element to the queue
void MessageQueue::push(Message *msg) {
    // Acquire lock by constructor
    std::unique_lock<std::mutex> lock(m_mutex);
    // Add item
//fprintf(stderr, "[%lx] %s.push() mgs = %s\n", (long)pthread_self(), name, msg->render()) ;
    m_queue.push(msg);
    // Notify one thread that is waiting
    m_cond.notify_one();
    // release lock by scope destructor
}

// Pops an element off the queue
// wait = false: returns nullptr if empty
// wait = true: block until something in queue
Message *MessageQueue::pop(bool wait) {
    Message *msg = nullptr ;
    // acquire lock
    std::unique_lock<std::mutex> lock(m_mutex);
    if (wait) {
        // wait calling thread until queue is not empty
        m_cond.wait(lock, [this]() {
            return !m_queue.empty();
        });
    }
    if (!m_queue.empty()) {
        // retrieve item, return nullptr if empty
        msg = m_queue.front();
//fprintf(stderr, "[%lx] %s.pop() mgs = %s\n", (long)pthread_self(), name, msg->render()) ;
        m_queue.pop();
    }
    // return item
    return msg;

    // release lock by scope destructor
}

