
#include "KQueue.h"
#include "utils.h"

KQueue::KQueue()
{
	kqueue_fd = kqueue();
	if (kqueue_fd == -1)
		throw std::exception();
}


KQueue::~KQueue()
{
}
