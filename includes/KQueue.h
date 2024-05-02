#ifndef KQUEUE_H
# define KQUEUE_H

# include <sys/event.h>
# include <unistd.h>
# include <vector>
# include "CustomException.h"
# include "ListeningSocketsBlock.h"

class KQueue
{
private:
	//vars
	int		kqueue_fd;
	int		listening_sock_ident;
	int		connection_sock_ident;

public:
	// constructors & destructors
	KQueue(const KQueue&);
	KQueue& operator=(const KQueue&);
	KQueue();
	~KQueue();

	// getters
	int		getKQueueFD() const;
	int		getListeningSocketIdent() const;
	int		getConnectionSocketIdent() const;

	// methods
	void	attachListeningSockets(ListeningSocketsBlock&);
	void	attachConnectionSockets(std::vector<int>);
	void	closeQueue();
};

#endif
