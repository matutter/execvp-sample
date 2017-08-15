
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

#include "cp.h"
#include "debug.h"

int main(void) {

  debug_primary("--------- test 001 ChildProcess.run ---------");

  char* argv[] = { "/bin/date", "+\%s", NULL};

  ProcessInfo p_info = {
    .argv = argv,
    .buffer = {
      .size = 1024,
      .double_buffer = true,
      .ptr = malloc(1024*2)
    }
  };

  int status = ChildProcess.run(&p_info);

  if(status == -1) {
    debug_errno("Failed to run child process");
  }

  return status;
}
