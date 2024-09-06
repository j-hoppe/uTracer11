/* message queue - thread safe
https://www.geeksforgeeks.org/implement-thread-safe-queue-in-c/
*/

#ifndef __MESSAGEQUEUE_H__
#define __MESSAGEQUEUE_H__

#include <condition_variable>
#include <mutex>
#include <queue>

#include "messages.h"

// Thread-safe queue
class MessageQueue {
private:
    // Underlying queue
    std::queue<Message*> m_queue;
    // mutex for thread synchronization
    std::mutex m_mutex;
    // Condition variable for signaling
    std::condition_variable m_cond;
public:
    void clear() {
        // Acquire lock by constructor
        std::unique_lock<std::mutex> lock(m_mutex);
        // standard procedure: Swap the contents of myQueue with emptyQueue
        std::queue<Message*> emptyQueue;
        std::swap(m_queue, emptyQueue);
    }

    // Pushes an element to the queue
    void push(Message *msg) {
        // Acquire lock by constructor
        std::unique_lock<std::mutex> lock(m_mutex);
        // Add item
        m_queue.push(msg);
        // Notify one thread that is waiting
        m_cond.notify_one();
        // release lock by scope destructor
    }

    // Pops an element off the queue
    // wait = false: returns nullptr if empty
    // wait = true:
    Message *pop(bool wait) {
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
            m_queue.pop();
        }
        // return item
        return msg;

        // release lock by scope destructor
    }
};

#endif // __MESSAGEQUEUE_H__
