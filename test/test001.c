
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

#include "cp.h"
#include "debug.h"

int main(void) {

  debug_primary("--------- test 001 ---------");

  char* argv[] = { "+\%s" };

  ProcessInfo p_info = {
    .executable = "/bin/date",
    .argv = argv,
    .argc = 1,
    .buffer = {
      .size = 1024,
      .double_buffer = true
    }
  };

  int status = ChildProcess.run(&p_info);

  if(status == -1) {
    debug_errno("Failed to run child process");
  }

  return status;
}
