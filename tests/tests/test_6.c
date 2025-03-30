#include "types.h"
#include "stat.h"
#include "user.h"
#include "test_helper.h"

int lk = 0;
int i = 0;
int j = 0;

static void cb1(void* arg) {
  int *shared = (int*)arg;
  lock(&lk);
  sleep(100);
  i = *shared;
  *shared += 1;
  unlock(&lk);
}

static void cb2(void* arg) {
  int *shared = (int*)arg;
  sleep(50);
  lock(&lk);
  j = *shared;
  *shared += 1;
  unlock(&lk);
}

int
main(int argc, char* argv[])
{

    int shared = 0;
    thread_create(&cb1, (void*)&shared);
    thread_create(&cb2, (void*)&shared);

    thread_join();
    thread_join();

    ASSERT(i == 0, "this thread should get the lock, and execute first (%d)", i);
    ASSERT(j == 1, "this thread should get the lock, and execute second (%d)", j);

    ASSERT(shared == 2, "both threads should have executed");
    test_passed();
    exit();
}
