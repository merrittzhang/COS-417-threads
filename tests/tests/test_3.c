#include "types.h"
#include "stat.h"
#include "user.h"
#include "test_helper.h"

int stack[4096] __attribute__ ((aligned (4096)));
int x = 0;

int
main(int argc, char* argv[])
{

    int tid = clone(stack);

    if (tid < 0) {
      exit();
    } else if (tid == 0) {
      // child
      x = 0xdeadbeef;
      exit();
    } else {
      join();
      ASSERT(x == 0xdeadbeef, "expected join to wait until child thread assigns `x` ");
      test_passed();
    }

    wait();
    exit();
}
