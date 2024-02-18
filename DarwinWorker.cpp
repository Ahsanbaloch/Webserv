
#include "DarwinWorker.h"
#include "utils.h"

DarwinWorker::DarwinWorker()
{
	kqueue_fd = kqueue();
	if (kqueue_fd == -1)
		throw std::exception();
	listening_sock_ident = 0;
	connection_sock_ident = 1;
}

DarwinWorker::~DarwinWorker()
{
}

