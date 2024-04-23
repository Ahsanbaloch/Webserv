#ifndef DEFINES_H
# define DEFINES_H

#define BUFFER_SIZE 8192 // this basically presents the max header size (incl. the request line)
#define LF 10
#define CR 13
#define SP 32

extern int g_num_temp_files;
extern int g_num_temp_unchunked_files;
extern int g_num_temp_raw_body;

#endif
