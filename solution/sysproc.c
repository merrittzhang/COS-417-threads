#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

extern struct {
  struct spinlock lock;
  struct proc proc[NPROC];
} ptable;

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
clone(void *stack)
{
  struct proc *np;
  struct proc *curproc = myproc();

  if(stack == 0)
    return -1;

  if((np = allocproc()) == 0)
    return -1;

  np->pgdir = curproc->pgdir;
  np->sz = curproc->sz;
  np->parent = curproc;

  *np->tf = *curproc->tf;
  np->tf->esp = (uint)stack;
  np->tf->eax = 0;

  np->isThread = 1;

  for (int i = 0; i < NOFILE; i++)
    np->ofile[i] = curproc->ofile[i];
  np->cwd = idup(curproc->cwd);
  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  acquire(&ptable.lock);
  np->state = RUNNABLE;
  release(&ptable.lock);

  return np->pid;
}

int
sys_clone(void)
{
  void *stack;
  if(argptr(0, (void *)&stack, PGSIZE) < 0)
    return -1;
  return clone(stack);
}

int
join(void)
{
  struct proc *p;
  int haveThread;
  struct proc *curproc = myproc();

  acquire(&ptable.lock);
  for (;;) {
    haveThread = 0;
    for (p = ptable.proc; p < &ptable.proc[NPROC]; p++) {
      if (p->parent == curproc && p->isThread) {
        haveThread = 1;
        if (p->state == ZOMBIE) {
          int tid = p->pid;
          kfree(p->kstack);
          p->kstack = 0;
          p->state = UNUSED;
          release(&ptable.lock);
          return tid;
        }
      }
    }
    if (!haveThread) {
      release(&ptable.lock);
      return -1;
    }
    sleep(curproc, &ptable.lock);
  }
}

int
sys_join(void)
{
  return join();
}

int
lock(int *l)
{
  if(l == 0)
    return -1;
  while(xchg(l, 1) != 0) {
    acquire(&ptable.lock);
    sleep(l, &ptable.lock);
    release(&ptable.lock);
  }
  return 0;
}

int
sys_lock(void)
{
  int *l;
  if(argptr(0, (void *)&l, sizeof(int)) < 0)
    return -1;
  return lock(l);
}

int
unlock(int *l)
{
  if(l == 0)
    return -1;
  *l = 0;
  acquire(&ptable.lock);
  wakeup(l);
  release(&ptable.lock);
  return 0;
}

int
sys_unlock(void)
{
  int *l;
  if(argptr(0, (void *)&l, sizeof(int)) < 0)
    return -1;
  return unlock(l);
}

int 
thread_create(void (*fn)(void *), void *arg) {
  void *stack = malloc(4096);
  if(stack == 0)
    return -1;
  stack = (void *)(((uint)stack + 4095) & ~(4095));
  int tid = clone(stack);
  if(tid < 0) {
    free(stack);
    return -1;
  }
  if(tid == 0) {
    fn(arg);
    free(stack);
    exit();
  }
  return tid;
}
