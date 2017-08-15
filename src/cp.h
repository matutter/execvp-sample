#ifndef CHILD_PROC_MOD_H
#define CHILD_PROC_MOD_H

#include <sys/types.h>

union Pipe {
  int fds[2];
  struct {
    int read;
    int write;
  } io;
};

typedef struct PROC_INFO ProcessInfo;
struct PROC_INFO {
  char* executable;
  int argc;
  char** argv;
  char** command;
  int status;
  union Pipe pipes;
  pid_t pid;
  struct {
    bool double_buffer;
    size_t size;
    unsigned char * ptr;
  } buffer;
};

struct CHILD_PROC_MOD {
  int (* const run)(struct PROC_INFO*);
};

const struct CHILD_PROC_MOD ChildProcess;

#endif