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
	const char *name = nullptr ;
	void clear() ;
	void push(Message *msg) ;
    Message *pop(bool wait) ;
};

#endif // __MESSAGEQUEUE_H__
