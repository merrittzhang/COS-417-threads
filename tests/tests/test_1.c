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
      for(int i = 0; i < 5; i++) {
        x++;
        sleep(10);
      }
      exit();
    } else {
      // parent
      int y = 0;
      for(int i = 0; i < 20; i++) {
        if (y == x) {
          y++;
        }
        sleep(5);
      }
      ASSERT(y == 6, "expected `x` to hit every value 0-5");
      test_passed();
    }

    join();
    exit();
}
