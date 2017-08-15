
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

#include "cp.h"
#include "debug.h"

int main(void) {

  debug_primary("--------- test 002 ChildProcess.new ---------");

  char* argv[] = { "/bin/date", "+\%s", NULL };

  ProcessInfo* info = ChildProcess.new(argv);

  int status = ChildProcess.run(info);

  if(status == -1) {
    debug_errno("Failed to run child process");
  }

  return status;
}
