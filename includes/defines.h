#ifndef DEFINES_H
# define DEFINES_H

#define BUFFER_SIZE 8192 // this basically presents the max header size (incl. the request line)
#define MAX_EVENTS 128 // how to determine what to set here? --> maybe partly related to SOMAXCONN, but apparently not entirely
#define LF 10
#define CR 13
#define SP 32

#endif
