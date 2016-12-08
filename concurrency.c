#if 0
cc -Wall -g -std=c99 -o concurrency concurrency.c && ./concurrency;
exit;
#endif

#include <stdio.h>
#include <stdint.h>

// primitives that "low level threading junk" uses to describe stuff.
// StoreRelaxed
// StoreRelease(ptr, x)
//   *ptr = x

// store release means all stores before it dont come after it.


// LoadAcquire(ptr)
//   x = *ptr

// loadRexed
// load aquire means no loads after it can come before it.

// on msvc
// loadAcquire(ptr):
// mov x, *ptr
// ReadBarrier()

// storeRelease(ptr, x):
// WriteBarrier();
// mov *ptr, x;

// readbarrier and writebarrier are just compiler barriers, not real instructions.
// on x86 mov is atomic on atomically sized types.

// now what about changing values?
// the interlocked instructions are how you do this stuff on windows.
// CompareAndSwap is the biggest one, need to watch out because InterlockedCompareExcange is opposite of args that the standard library uses.
// CAS(pvar, old, new)
// how to use
// old = Load(pvar)
// do {
//   new = old + 1
// } while(!cas(pvar,old,new))

// Exchange()
//
// also you probably want to define how you're doing CAS, like CAS aquire, CAS release or both.

typedef struct {
    Node *m_next;
} Node;

Node *g_stack = 0;

void
nodePush(Node *node)
{
    Node *localStack = LoadRelaxed(&g_stack);
    do {
        node->m_next = localStack;
    } while (!CAS_Release(&g_stack, &localStack, node));
}

Node *
nodePop()
{
    Node *localStack = LoadAcquire(&g_stack);
    do {
        if (localStack == NULL) return NULL;
        Node *nextStack = localStack->m_next;
    } while (!CAS_Release(&g_stack, localStack, nextStack));

    return localStack;
}

int
main()
{
    printf("Hello concurrency\n");
}
