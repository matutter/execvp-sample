// example of using args with a unit test
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>

#include "debug.h"

typedef void (* SelectCb)(int);

#define mssleep(x) usleep(x*1000)

struct FIZ {
  int mod;
  SelectCb cb;
};

void fiz(int i) {
  debug_danger("%02x Fiz!", i);
}

void bang(int i) {
  debug_primary("%02x Bang!", i);
}

void fark(int i) {
  debug_success("%02x Fark!", i);
  mssleep(450);
}

int main(int argc, char** argv) {

  debug_primary("--------- test 003 fizbang ---------");

  struct FIZ bangs[] = {
    { .mod=3, .cb=fiz },
    { .mod=5, .cb=bang },
    { .mod=8, .cb=fark }
  };
  int bang_count = sizeof(bangs) / sizeof(bangs[0]);

  int max = 21;

  // just read whatever # from arguments
  for(int i = 0; i < argc; i++) {
    int val = atoi(argv[i]);
    if(val > 0) {
      max = val;
      debug_info("Read max value %d", max);
      break;
    }
  }

  for(int i = 1; i <= max; i++) {
    for(int j = 0; j < bang_count; j++) {
      struct FIZ* fiz = &bangs[j];
      if(i % fiz->mod == 0) {
        fiz->cb(i);
      }
    }
  }

  return 0;
}
