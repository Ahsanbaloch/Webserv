
#include "../includes/LinuxWorker.h"

LinuxWorker::LinuxWorker(const EPoll& Queue)
	: Q(Queue)
{
	memset(&client_addr, 0, sizeof(client_addr));
	addr_size = 0;
}

LinuxWorker::~LinuxWorker()
{
}

//function to run event loop
void	LinuxWorker::runEventLoop()
{
	while (1)
	{
		// check for new events that are registered in our kqueue (could come from a listening or connection socket)
		int new_events = epoll_wait(Q.epoll_fd, event_lst, MAX_EVENTS, -1);
		if (new_events == -1)
			throw CustomException("Failed when checking for new events\n");
		
		// go through all the events we have been notified of
		for (int i = 0; new_events > i; i++)
		{
			// double check what this is exactly checking
			if ((event_lst[i].events & EPOLLERR) || (event_lst[i].events & EPOLLHUP) || (!(event_lst[i].events & EPOLLIN)))
			{
				std::cout << "client disconnected\n"; // or epoll error?
				close(event_lst[i].data.fd);
			}
			// event came from listening socket --> we have to create a connection // maybe there is a different way for checking instead of using a lambda function?
			else if (std::find_if(Q.SocketsBlock.listening_sockets.begin(), Q.SocketsBlock.listening_sockets.end(), [event_lst[i].data.fd](const ListeningSocket& obj) {return obj.getSocketFd() == event_lst[i].data.fd}) != Q.SocketsBlock.listening_sockets.end())
			{
				while (1) // to improve efficiency (reducing calls to kevent), we accept all connection requests related to the event in a loop
				{
					std::cout << "new connection incoming\n";
					int connection_fd = accept(event_lst[i].data.fd, (struct sockaddr *)&client_addr, &addr_size); // accept performs the 3-way TCP handshake
					if (connection_fd == -1) // if we have handled all connection requests related to the specific event notification
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
						{
							Q.attachConnectionSockets(pending_fds);
							pending_fds.clear();
							break;
						}
						else
							throw CustomException("Failed when trying to establish connection\n");
					}
					pending_fds.push_back(connection_fd);
				}
			}
			// event came from connection, so that we want to handle the request
			else
				Handler.handleRequest(event_lst[i].data.fd); // probably make connection_fd the input so that it is independent from kevent/epoll; also keep track of connection_fd in case not everything can be built at once
		}
	}
}

