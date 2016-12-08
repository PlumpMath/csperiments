// @TODO: you need to use interlocked-compare-exchange for checking if there's work to do, not just increment because that's a serious business bug




/*
  Using casey style stuff. x64 intrinsics.


  So like here's a thing. Even though I know this is wrong the results are actually almost right.
  That means it's hard to see that it's wrong and maybe I need some sort of good concurrent
  property testing system that would let me find bugs like this.

 */
// Write a work queue using x64 primitives.
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <unistd.h>
#include <libkern/OSAtomic.h> // @TODO: Use SDL_Atomic.h or hopefully some day stdatomic.h
//#include <stdatomic.h> where this header at doe?

// API to test
// pull_item
// push_item
// create_worker_thread

typedef struct {
    char* string_to_print;
} WorkQueueEntry;

// This stuff is locked.
static uint32_t volatile work_available;
static pthread_mutex_t work_available_mx;
static pthread_cond_t new_work_available;

// This stuff uses atomics.
static uint32_t volatile next_entry_to_do;
static uint32_t volatile entry_count;
WorkQueueEntry entries[256];

void push_string(char* s) {
    assert(entry_count < (sizeof(entries) / sizeof(entries[0])));
    WorkQueueEntry* entry = entries + entry_count;
    entry->string_to_print = s;
    
    OSMemoryBarrier();

    entry_count++;

    // another way to do this would be to enqueue all the work up front.
    // broadcast to all threads to begin work
    // and just have them sleep when the work runs out

    pthread_mutex_lock(&work_available_mx);
    work_available++;
    pthread_cond_signal(&new_work_available);
    pthread_mutex_unlock(&work_available_mx);
}

typedef struct {
    pthread_t handle;
    int logical_thread_index;
} ThreadInfo;

void* thread_process(void* param)
{
    ThreadInfo* thread_info = (ThreadInfo*)param;

    for (;;) {
        // If there is work to do just keep working.

        // @BUG: This is bad wrong bruh, another thread can grab the next entry before we do.

        // right here us interlocked compare exchange        
        if (next_entry_to_do < entry_count) {
            int entry_index = OSAtomicIncrement32((int32_t*)&next_entry_to_do) - 1;

            OSMemoryBarrier();

            WorkQueueEntry* entry = entries + entry_index;
            printf("Thread %i: %s\n", thread_info->logical_thread_index, entry->string_to_print);
        // If not then sleep until there's more work to do.
        } else {
            pthread_mutex_lock(&work_available_mx);
            while(!work_available) {
                fprintf(stderr, "Thread %i sleeping\n", thread_info->logical_thread_index);
                // wait for work to be available.
                pthread_cond_wait(&new_work_available, &work_available_mx);
                work_available--;
            }
            pthread_mutex_unlock(&work_available_mx);
        }
    }
}

int main(int argc, char* argv[])
{
    ThreadInfo thread_info[4];

    pthread_mutex_init(&work_available_mx, 0);
    pthread_cond_init(&new_work_available, 0);
    
    for (int thread_index = 0;
         thread_index < sizeof(thread_info) / sizeof(thread_info[0]);
         thread_index++) {
        ThreadInfo* info = thread_info + thread_index;
        info->logical_thread_index = thread_index;

        pthread_create(&info->handle, NULL, thread_process, (void*)info);
    }

    work_available = 0;

    OSMemoryBarrier();

    sleep(5);

    push_string("A0");
    push_string("A1");
    push_string("A2");
    push_string("A3");
    push_string("A4");
    push_string("A5");
    push_string("A6");
    push_string("A7");
    push_string("A8");
    push_string("A9");
    
    sleep(5);

    push_string("B0");
    push_string("B1");
    push_string("B2");
    push_string("B3");
    push_string("B4");
    push_string("B5");
    push_string("B6");
    push_string("B7");
    push_string("B8");
    push_string("B9");

    sleep(5);

    push_string("C0");
    push_string("C1");
    push_string("C2");
    push_string("C3");
    push_string("C4");
    push_string("C5");
    push_string("C6");
    push_string("C7");
    push_string("C8");
    push_string("C9");

    sleep(1);
    
    return 0;
}

// Hey steve. How would you property test this code?
// Properties to test.
// 1) No matter how many threads you use, every thing will be processed exactly once.
// 2) 
