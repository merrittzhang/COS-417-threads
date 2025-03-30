#include "types.h"
#include "stat.h"
#include "user.h"
#include "test_helper.h"

int shared = 0xdead;

static void cb(void* arg) {
  shared = (int)arg;
}

int
main(int argc, char* argv[])
{

    ASSERT(shared == 0xdead, "if this fails, something has gone dramatically wrong");
    thread_create(&cb, (void*)0xbeef);
    thread_join();
    ASSERT(shared == 0xbeef, "expected join to wait until child thread assigns `shared` ");

    test_passed();
    exit();
}
