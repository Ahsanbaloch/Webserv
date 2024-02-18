
#include "KQueue.h"
#include "utils.h"

KQueue::KQueue()
{
	kqueue_fd = kqueue();
	if (kqueue_fd == -1)
		throw std::exception();
	listening_sock_ident = 0;
	connection_sock_ident = 1;
}


KQueue::~KQueue()
{
}
