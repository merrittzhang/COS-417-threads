#include "types.h"
#include "stat.h"
#include "user.h"
#include "test_helper.h"

int stack[4096] __attribute__ ((aligned (4096)));

int
main(int argc, char* argv[])
{
    ASSERT(join() == -1, "expected join before clone to return -1");
    int tid_c = clone(stack);
    if (tid_c <= 0) {
      exit();
    } else {
      ASSERT(tid_c != 0, "expected clone to return non-zero thread id");
      int tid_j = join();
      ASSERT(tid_c == tid_j, "expected join to return the same thread id as clone did.");
      int tid_j2 = join();
      ASSERT(tid_j2 == -1, "expected second join to return -1 %d", tid_j2);

      test_passed();
      exit();
    }
}
