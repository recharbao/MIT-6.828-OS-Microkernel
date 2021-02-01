// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.


// 分配器中的数据结构是一个由可分配物理内存页构成的空闲链表。
// 这个空闲页的链表的元素是结构体 struct run。
// 那么分配器从哪里获得内存来存放这些数据结构呢？实际上，
// 分配器将每个空闲页的 run 结构体保存在该空闲页本身中，因为空闲页中没有其他数据。
// 分配器还用一个 spin lock 来保护空闲链表。
// 链表和这个锁都封装在一个结构体中，这样逻辑就比较明晰：
// 锁保护了该结构体中的域。不过现在让我们先忽略这个锁，以及对 acquire 和 release 的调用

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
kinit()
{
  initlock(&kmem.lock, "kmem");
  freerange(end, (void*)PHYSTOP);  //将内存加入空闲链表
}


// 对每一页调用kfree()
void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release(&kmem.lock);
}

// 分配一个 4096-byte 物理内存的页面.
// 返回一个内核可以使用的指针
// 返回0如果内存无法被分配
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
