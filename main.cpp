#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/event.h>

#define BUFFER_SIZE 1024
#define MAX_EVENTS 1 // how to determine what to set here?


// function to set fd as nonblocking // could also be done with open() call
void	setNonblocking(int fd) // --> do when doing socket creation and socketopt
{

	// the correct way to make the fd non-blocking would be to first get the current flags with F_GETFL and then add the non-blocking one. However, F_GETFL is not allowed
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
		perror("fcntl failure"); // is perror allowed?
	
}

int	main(void)
{
	struct sockaddr_in my_addr;
	struct sockaddr_storage client_addr;
	char buffer[BUFFER_SIZE];
	socklen_t addr_size;


	

	// creating server socket
	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
		return (perror("Failure creating server socket"), 1);
	

	// enabling SO_REUSEADDR for the socket indicates that the socket can be reused even if it is in a TIME_WAIT state. This can be helpful in scenarios where you want to restart a server quickly after it has been shut down, without waiting for the operating system to release the socket.
	// A TCP local socket address that has been bound is unavailable for some time after closing, unless the SO_REUSEADDR flag has been
	// set.  Care should be taken when using this flag as it makes TCP less reliable.
	// how is it related to timeout functionality (RFC 9112 (HTTP 1.1) section 9.5) and do we implement it?
	int enable = 1;
	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1)
		return (perror("Failure setting socket options"), 1);


	// binding
	// see man bind(2) and man ip7
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(4242); // port in network byte order --> here using 8080
	my_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // ip host address --> here using 127.0.0.1 (man ip7)

	if (bind(socket_fd, (struct sockaddr*)&my_addr, sizeof(my_addr)) < 0)
		return (perror("Failure when binding socket"), 1);
	


	// listening to incoming requests
	//setNonblocking(socket_fd);
	if (listen(socket_fd, SOMAXCONN) < 0)
		return (perror("Failure when listening for requests"), 1);

	// create kqueue object by calling kqueue()
	int kq_fd = kqueue();
	if (kq_fd == -1)
		return (perror("Failure when creating kqueue object"), 1);
	
	// attach socket to kqueue (how to attach several sockets?)
	// define what events we are interested in
	// by calling kevent()
	// in a loop(?) --> no; because this is the listening socket we are attaching
	// and we don't create any new listening sockets while the server is running (only conncetion sockets)
	struct kevent listening_event[1];
	EV_SET(listening_event, socket_fd, EVFILT_READ, EV_ADD, 0, 0, 0); // may add EV_CLEAR and/or EV_ENABLE flags in addition to EV_ADD
	if (kevent(kq_fd, listening_event, 1, NULL, 0, NULL) == -1)
		return (perror("Failure in registering event"), 1);

	// create event loop
	struct kevent eventSet[1];

	while (1)
	{
		// check for new events that are registered in our kqueue (could come from a listening or conncetion socket), but do not register them with the kqueue yet. 5th arg specifies the number of new events to handle per each iteration
		struct kevent event_lst[MAX_EVENTS]; 
		int new_events = kevent(kq_fd, NULL, 0, event_lst, MAX_EVENTS, NULL);
		if (new_events == -1)
			return (perror("Checking for new events"), 1);
		
		// go through all the events we have been notified of
		for (int i = 0; new_events > i; i++)
		{
			// when client discounnected an EOF is sent. Close fd to rm event from kqueue
			// event_lst[i].ident is basically the file descriptor of the socket that triggered
			if (event_lst[i].flags & EV_EOF)
			{
				printf("client disconnected\n");
				close(event_lst[i].ident); // is this enough to remove from queue?
			}
			// event came from listening socket --> we have to create the connection
			else if (event_lst[i].ident == socket_fd)
			{
				printf("new connection incoming");
				// accept basically performs the 3-way TCP handshake
				// probably need to be store an array or so
				int connection_fd = accept(event_lst[i].ident, (struct sockaddr *)&client_addr, &addr_size);
				if (connection_fd == -1)
					perror("Failure when trying to establish connection");
					// close connection_fd?
				else
				{
					// attach conncetion to kqueue to be able to receive updates
					// we later also want to add write
					EV_SET(eventSet, connection_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
					if (kevent(kq_fd, eventSet, 1, NULL, 0, NULL) < 0)
						perror("Failure when registering event");
				}
			}

			// event came from conncetion socket and had been added with read filter
			// signalling that it is ready for reading --> so, we want to read as much as possible
			else if (event_lst[i].filter == EVFILT_READ)
			{
				char buf[BUFFER_SIZE];
				size_t bytes_read = recv(event_lst[i].ident, buf, sizeof(buf), 0);
				printf("read %zu bytes\n", bytes_read);
			}
		}
	}
	close(socket_fd);


	// while (1)
	// {
	// 	int connect_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size);
	// 	printf("Successful connection\n");
     
	// 	// Read the HTTP request
	// 	ssize_t bytes_read = recv(connect_fd, buffer, BUFFER_SIZE, 0);

	// 	// Print the HTTP request
	// 	printf("Received HTTP request:\n%s\n", buffer);

	// 	const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello, World!";
	// 	send(connect_fd, response, strlen(response), 0);
	// 	close(connect_fd);
	// }
	// close(socket_fd);

	// close kqueue object


}


// epoll/kqueue etc. provide an interface to kernel
// 1) creating a KQ object (one per provess/thread)
// 2) attach socket_fd to KQ
// --> since we have several socket_fds, we would need to attach several?
// --> this is usually done once per socket
// 3) wait for incoming events from KQ
// 4) destroy KQ object

// how to test?

