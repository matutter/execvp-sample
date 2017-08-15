#include <stdio.h>

// errno
#include <errno.h>

// wait
#include <sys/wait.h>

// pid_t
#include <sys/types.h>

// strcmp
#include <string.h>

// true,false,bool
#include <stdbool.h>

// stat
#include <sys/stat.h>

// pipe
#include <unistd.h>

// malloc
#include <stdlib.h>

#include "debug.h"
#include "cp.h"

static int run(ProcessInfo*);
static inline int beforeRun(ProcessInfo*);
static inline int allocateBuffer(ProcessInfo*);

const struct CHILD_PROC_MOD ChildProcess = {
  .run = run
};

static inline int allocateBuffer(ProcessInfo* info) {
  int status = -1;

  size_t size = info->buffer.size;
  if(info->buffer.double_buffer) size *= 2;

  unsigned char* ptr = realloc(info->buffer.ptr, size);

  if(ptr) {
    info->buffer.ptr = ptr;
    status = 0;
  }

  return status;
}


/**
* Validate and setup `info` object.
*/
static inline int beforeRun(ProcessInfo* info) {
  debug_where();
  int status = 0;

  char* executable = info->executable;
  char** argv = info->argv;
  int argc = info->argc;

  if(executable == NULL) {
    debug_warning("Executable not defined.");
    errno = EINVAL;
    status = -1;
  }

  if(status == 0) {
    status = allocateBuffer(info);
  }

  if(status == 0) {
    info->command = realloc(info->command, sizeof(char*) * (argc + 1));
    info->command[0] = executable;
    info->command[argc] = NULL;

    for(int i = 0; i < argc; i++) {
      info->command[i+1] = argv[i];
    }

    #if 0
      for(int i = 0; i < command_argc; i++) {
        debug_info("arg %d: %s", i, info->command[i]);
      }
    #endif

    status = pipe(info->pipes.fds);
    if(status == 0) {

      fflush(stdout);
      info->pid = fork();
      if(info->pid == -1) {
        debug_errno("fork failed");
        status = -1;
      }
    } else {
      debug_errno("pipe failed");
    }
  }

  return status;
}


/**
* Run the processes with the following info.
*/
int run(ProcessInfo* info) {
  debug_where();
  int status = 0;

  status = beforeRun(info);
  if(status == 0) {

    if(info->pid == 0) {
      // child
      close(info->pipes.io.read);
      status = dup2(info->pipes.io.write, STDOUT_FILENO);
      if(status != -1) {
        status = 0;
        execvp(info->executable, info->command);
        fflush(stdout);
      }

      exit(status);
    } else {
      // parent
      debug_success("(parent)");
      close(info->pipes.io.write);

      do {
        waitpid(info->pid, &info->status, 0);
      } while (!WIFEXITED(info->status));

      size_t size = 0;
      size_t limit = info->buffer.size;
      unsigned char* ptr = info->buffer.ptr;
      bool double_buffer = info->buffer.double_buffer;
      do {
        size = read(info->pipes.io.read, ptr, limit);
        if(size > 0) {
          ptr[size-1] = '\0';
          debug("read %lu: %s", size, ptr);
        }
      } while(size > 0);

    }

  }

  return status;
}


