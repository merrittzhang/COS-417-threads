# COS417 Spring 2025: Threads

# xv6 Threads

## Changelog

  * Add simple test, `testthreads.c.`

## Overview
Overview

In this project, you'll be adding real kernel threads to xv6. Sound
like fun? Well, it should. Because you are on your way to becoming a
real kernel hacker. And what could be more fun than that?

Specifically, you'll add four new system calls:

  * `int clone(void *stack)`
  * `int lock(int *l)`
  * `int unlock(int *l)`
  * `int join()`

You'll also add one user-space function that wraps `clone()` for convenience:

  * `int thread_create(void (*fn) (void *), void *arg)`

Finally, depending on your implementation strategy, you may need to
modify some kernel data structures and related system call
implementations, such as the process struct and `wait()`.

Note: We're starting with a clean kernel; no need for your new fancy
address space with the stack at the bottom, for example.

## Details

Each new function returns -1 on failure. The locking functions (`lock`
and `unlock`) return 0 on success, and the other three (clone, join,
and thread_create) return a thread ID.

Getting `clone` working is the first step, and will probably take the
most time. This call should behave very much like fork, except that
instead of copying the address space to a new page directory, clone
initializes the new process so that the new process and cloned process
use the same page directory. Thus, memory will be shared, and the two
"processes" are really actually threads.

Although the two threads will share the same address space, they each
need their own stack; the cloned process allocates space for this,
probably with `malloc`. Suppose thread T1 is making a clone of itself,
T2. T1 will first allocate a 1-page piece of page-aligned memory and
pass a pointer to that memory to the clone system call. The `clone`
system call will then copy T1's stack to that location, and update
T2's stack and base registers to use the new stack. It's admittedly a
bit strange to allocate T2's stack in a heap object allocated by T1 in
this way, but the alternative would be to integrate with p3b
(relocated stacks), and that would require you to do much more work.

The `thread_create` function wraps clone, and takes a function pointer
and thread argument as arguments. This call should do the following:

 * allocate page-aligned space for the new stack
 * call clone, returning the thread ID to the parent
 * in the child, thread_create should call the function pointer, passing it the provided argument
 * when the provided function returns, thread_create should free the stack and call exit

Note that functions passed to `thread_create` should not quit by
exiting; they should quit by returning. Otherwise, `thread_create`
will not have an opportunity to free the space used for the stack.

Another new system call is `int join(void)`, which is very similar to
the already existing `int wait(void)` call; `join` waits for a thread
child to finish, and `wait` waits for a process child to finish.

The `lock` and `unlock` calls should have the normal behavior. Any
code using these calls should initialize the lock variable to
`0`. Internally, a thread waiting on lock should go to sleep rather
than spin.

To test your code, use the TAs tests, as usual! But of course you
should write your own little code snippets to test pieces as you go.

Have fun!

## Hints

For `getpid`, it is acceptable (but not required) for each thread of a
process to return a different pid. You may wish to use pids as thread
IDs to minimize the changes you must make.

You need to make sure you do not free the page table when a thread
exits if there are still other threads running. The elegant way to
handle this is with reference counting. The quick but hacky way is to
scan the process table looking for other references before freeing the
page table.

When implementing `lock` and `unlock`, the `sleep` and `wakeup` calls
in `proc.c` may be useful, perhaps in combination with a "guard"
spinlock.

One thing you need to be careful with is when an address space is
grown by a thread in a multi-threaded process. Trace this code path
carefully and see where a new lock is needed and what else needs to be
updated to grow an address space in a multi-threaded process
correctly.

It is difficult to write `clone` incrementally. Two tricky cases you
have to handle are (1) avoiding race conditions upon address-space
growth, and (2) not freeing a page table until all threads are
finished. We recommend starting with simple test code that avoids
these issues by never growing the address space and never exiting.

## Testing

Please refer to the readme file in the `tests` directory for the specs
of our testing framework, including how to add your own tests, run
only one test, run all tests even after one fails, and more!

## Submission/Deliverables

You'll be uploading a .zip file of the `solution` directory (which
should contain all your code changes) to TigerFile under the `threads`
assignment - all test cases will be run whenever you click on 'check
submitted files'.
