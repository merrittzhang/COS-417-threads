#include "user.h"
#include "param.h"

#define DEFAULT_TICKETS 8
#define TEST_PREFIX "P4_TESTER"

#define ASSERT(exp, msg, ...) if (!(exp)) { \
                        printf(1, "%s:%s:%d ", TEST_PREFIX, __FILE__, __LINE__); \
                        printf(1, msg, ##__VA_ARGS__); \
                        printf(1, "\n"); \
                        exit(); }

#define PRINTF(...)  printf(1, "%s: ", TEST_PREFIX); \
                        printf(1, __VA_ARGS__); \
                        printf(1, "\n");


#define SUCCESS_MSG "TEST PASSED"
static void test_passed() {
    PRINTF("%s", SUCCESS_MSG);
}
