#include "types.h"
#include "stat.h"
#include "user.h"
#include "test_helper.h"

int stack[4096] __attribute__ ((aligned (4096)));
int x = 0;

int
main(int argc, char* argv[])
{

    int tid = fork();

    if (tid < 0) {
      exit();
    } else if (tid == 0) {
      // child
      for(int i = 0; i < 5; i++) {
        x++;
        sleep(10);
      }
      exit();
    } else {
      // parent
      for(int i = 0; i < 20; i++) {
        sleep(5);
        ASSERT(x == 0, "expected `x` not to change since in difference processes");
      }
      test_passed();
    }

    wait();
    exit();
}
