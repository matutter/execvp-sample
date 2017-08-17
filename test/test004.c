
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

#include "cp.h"
#include "debug.h"

void run_shell(char * cmd) {
  char* argv[] = {
    "bash",
    "-c",
    cmd,
    NULL
  };

  ProcessInfo* info = ChildProcess.new(argv);

  int status = ChildProcess.run(info);

  if(status == -1) {
    debug_errno("Failed to run child process");
  }
}

int main(void) {

  debug_primary("--------- test 004 ---------");

  int status = 0;

  run_shell("for i in {30..1..5};do echo -n \"$i.\" && sleep 5; done");

  run_shell("for i in {1..5};do echo -n \"$i.\" && sleep 1; done");

  return status;
}
