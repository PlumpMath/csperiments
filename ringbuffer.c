#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>
/* #include <libkern/OSAtomic.h> */
#include <SDL2/Atomic.h>

#include <unistd.h>

typedef void(*WorkFunction)(void* work_data);

typedef struct {
    void *data;
    WorkFunction f;
} WorkQueueEntry;

typedef struct {
    pthread_mutex_t work_available_mx;
    pthread_cond_t new_work_available;
    int threads_sleeping;

    // @TODO: circular buffer
    int volatile next_entry_to_do;
    int volatile entry_count;
    WorkQueueEntry entries[256];
} WorkQueue;

typedef struct {
    pthread_t handle;
    int thread_index;
    WorkQueue *queue;
} ThreadInfo;

void* work_queue_process(void *param)
{
    ThreadInfo* info = (ThreadInfo*)param;
    WorkQueue* queue = info->queue;

    fprintf(stderr, "Thread %i beginning\n", info->thread_index);
    
    for (;;) {
        int next_entry_to_do = queue->next_entry_to_do;
        if (next_entry_to_do < queue->entry_count) {
            if (!OSAtomicCompareAndSwap32Barrier(next_entry_to_do,
                                                 next_entry_to_do + 1,
                                                 &queue->next_entry_to_do)) {
                // somebody else got it, try again
                continue;
            }

            fprintf(stderr, "thread: %i, working on %i\n", info->thread_index, next_entry_to_do);
            
            // Do the work!
            WorkQueueEntry *entry = queue->entries + next_entry_to_do;
            entry->f(entry->data);

        } else {
            // Sleep until there is work to do.
            // If there is work to do the threads will just keep working so that's good.
            pthread_mutex_lock(&queue->work_available_mx);
            queue->threads_sleeping++;
            fprintf(stderr, "Thread %i sleeping\n", info->thread_index);
            pthread_cond_wait(&queue->new_work_available, &queue->work_available_mx);
            fprintf(stderr, "Thread %i waking up\n", info->thread_index);
            queue->threads_sleeping--;
            pthread_mutex_unlock(&queue->work_available_mx);
        }
    }
}

// Single producer.
void
add_entry(WorkQueue *queue, WorkFunction f, void *data)
{
    // @TODO: circular buffer
    assert(queue->entry_count < sizeof(queue->entries) / sizeof(queue->entries[0]));
    WorkQueueEntry *new_entry = queue->entries + queue->entry_count;
    new_entry->f = f;
    new_entry->data = data;
    fprintf(stderr, "adding %i\n", queue->entry_count);

    OSAtomicIncrement32(&queue->entry_count);

    pthread_mutex_lock(&queue->work_available_mx);
    if (queue->threads_sleeping > 0) {
        pthread_cond_broadcast(&queue->new_work_available);
    }
    pthread_mutex_unlock(&queue->work_available_mx);
}

// test

void print_string(void* s)
{
    char* str = (char*)s;
    printf("%s\n", str);
}

int main() {
    WorkQueue q;
    
    pthread_mutex_init(&q.work_available_mx, 0);
    pthread_cond_init(&q.new_work_available, 0);

    q.next_entry_to_do = 0;
    q.entry_count = 0;

    ThreadInfo thread_info[4];

    for (int thread_index = 0;
         thread_index < (sizeof(thread_info) / sizeof(thread_info[0]));
         thread_index++) {
        ThreadInfo* info = thread_info + thread_index;
        info->thread_index = thread_index;
        info->queue = &q;
        pthread_create(&info->handle, NULL, work_queue_process, (void*)info);
    }

    add_entry(&q, print_string, "Hello");
    add_entry(&q, print_string, "World");

    sleep(1);
    
    add_entry(&q, print_string, "A1");
    add_entry(&q, print_string, "A2");
    add_entry(&q, print_string, "A3");
    add_entry(&q, print_string, "A4");
    add_entry(&q, print_string, "A5");

    sleep(1);

    add_entry(&q, print_string, "B1");
    add_entry(&q, print_string, "B2");
    add_entry(&q, print_string, "B3");
    add_entry(&q, print_string, "B4");
    add_entry(&q, print_string, "B5");

    sleep(1);

}
 
