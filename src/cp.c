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

// SIGKILL
#include <signal.h>

// malloc
#include <stdlib.h>

// O_NONBLOCK
#include <fcntl.h>

// select
#include <sys/select.h>

#include "debug.h"
#include "cp.h"

static int run(ProcessInfo*);
static ProcessInfo* newProcess(char**);
static inline int beforeRun(ProcessInfo*);
static inline int allocateBuffer(ProcessInfo*);

const struct CHILD_PROC_MOD ChildProcess = {
  .run = run,
  .new = newProcess
};

static inline int allocateBuffer(ProcessInfo* info) {
  debug_where(
    "Allocating %lu, double: %d"
    , info->buffer.size
    , info->buffer.double_buffer
  );
  int status = -1;

  size_t size = info->buffer.size;
  if(info->buffer.double_buffer) size *= 2;

  unsigned char* ptr = malloc(size);

  if(ptr) {
    info->buffer.ptr = ptr;
    status = 0;
  }

  return status;
}

static ProcessInfo* newProcess(char** argv) {
  debug_where();

  int status = 0;

  ProcessInfo* new_info = calloc(1, sizeof(ProcessInfo));
  if(new_info) {

    ProcessInfo info = {
      .pipes.fds = { 0, 0 },
      .status = 0,
      .argv = argv,
      .buffer = {
        .size = BUFSIZ,
        .double_buffer = true,
        .ptr = NULL
      }
    };

    status = allocateBuffer(&info);
    if(status == 0) {
      memcpy(new_info, &info, sizeof(ProcessInfo));
    } else {
      debug_warning("Failed to allocate data buffer");
    }
  } else {
    debug_warning("Failed to allocate ProcessInfo");
  }

  return new_info;
}

/**
* Validate and setup `info` object.
*/
static inline int beforeRun(ProcessInfo* info) {
  debug_where();
  int status = 0;

  if(info->argv == NULL || info->argv[0] == NULL) {
    debug_warning("Executable not defined.");
    errno = EINVAL;
    status = -1;
  }

  if(status == 0) {

    #if 0
      for(int i = 0; i < argv; i++) {
        debug_info("arg %d: %s", i, info->argv[i]);
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

ssize_t process_io(ProcessInfo* info) {
  int status = 0;
  ssize_t this_read = 0;
  ssize_t last_read = 0;
  ssize_t limit = info->buffer.size;
  unsigned char* ptr = info->buffer.ptr;
  bool double_buffer = info->buffer.double_buffer;

  fd_set set;
  struct timeval timeout;

  FD_ZERO(&set);
  FD_SET(info->pipes.io.read, &set);

  timeout.tv_sec = 2;
  timeout.tv_usec = 0;

  status = select(FD_SETSIZE, &set, NULL, NULL, &timeout);
  debug_info("Select returned %d", status);
  if(status > 0) {
    this_read = read(info->pipes.io.read, ptr+last_read, limit);
    if(this_read > 0) {
      ptr[this_read+last_read] = '\0';
      debug("read %lu: %s", this_read, ptr);

      if(last_read > 0) {
        memcpy(ptr, ptr+last_read, last_read);
      }

      last_read = this_read;
    }
  }

  return this_read;
}

void killAndWait(ProcessInfo* info) {

  int wpid = 0;
  do {
    wpid = waitpid(info->pid, &info->status, WNOHANG);
    if(wpid == 0) {
      debug("Sending SIGKILL to %d", info->pid);
      kill(info->pid, SIGKILL);
      usleep(200);
    }
  } while (wpid == 0);
  //} while (!WIFEXITED(info->status));
  debug("Child %d is dead", info->pid);

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
        execvp(info->argv[0], info->argv);
        fflush(stdout);
      }

      exit(status);
    } else {
      // parent
      debug_success("(parent)");
      close(info->pipes.io.write);
      // for "polling" nonblocking read
      /*status = fcntl(info->pipes.io.read, F_SETFL, fcntl(info->pipes.io.read, F_GETFL) | O_NONBLOCK);
      if(status != 0) {
        debug_warning("Failed to set O_NONBLOCK");
      }*/

      int wpid;
      do {
        wpid = waitpid(info->pid, &info->status, WNOHANG);
        if(wpid == 0) {
          ssize_t bytes_read = process_io(info);
          if(bytes_read == 0) {
            debug_info("Timeout exceeded for %d", info->pid);
            killAndWait(info);
          }
        }
      } while (wpid == 0);

      process_io(info);

    }
  }

  return status;
}


