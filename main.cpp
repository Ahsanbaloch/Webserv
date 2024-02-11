#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024


// function to set fd as nonblocking // could also be done with open() call
void	setNonblocking(int fd)
{
	// the correct way to make the fd non-blocking would be to first get the current flags with F_GETFL and then add the non-blocking one. However, F_GETFL is not allowed
	if (fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("fcntl failure"); // is perror allowed?
	}
	
}


int	main(void)
{
	struct sockaddr_in my_addr;
	struct sockaddr_storage client_addr;
	char buffer[BUFFER_SIZE];
	socklen_t addr_size;

	int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

	// setNonblocking(socket_fd);

	//see man bind(2) and man ip7
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(4242); // port in network byte order --> here using 8080
	my_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // ip host address --> --> here using 127.0.0.1 (man ip7)

	bind(socket_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
	listen(socket_fd, 20); // second arg is number of connection requests allowed in the queue

	while (1)
	{
		int connect_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &addr_size);
		printf("Successful connection\n");
     
		// Read the HTTP request
		ssize_t bytes_read = recv(connect_fd, buffer, BUFFER_SIZE, 0);

		// Print the HTTP request
		printf("Received HTTP request:\n%s\n", buffer);

		const char* response = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\n\r\nHello, World!";
		send(connect_fd, response, strlen(response), 0);
		close(connect_fd);
	}
	close(socket_fd);


}

