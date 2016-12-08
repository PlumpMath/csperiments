/*
  I still really want to get this working.
  Here are some things I think I can do.
  - I can try to put a small interface in front of some of the threading stuff. Then maybe I can
    come up with a way to property test it.
  - I can rename the can_read_to can_write_to stuff with 2 vars from reading and writing from->to
    maybe that will help me write the logic in a better what that doesn't block it up.

    Try to get this working with the mutex's and make it lock free.
    Think about doing the same thing with atomics later.



    Ugh, why doesn't this worrrrrk.
    Ok, next idea is I have a queue and I have a set of buffers that can be used.
    Writer pulls a buffer.
    When writer is done it puts the position of that buffer on the queue and pulls a new buffer.
    When there isn't a buffer free to use it blocks.
    Reader pulls locations of the buffers off the queue.
    Once reader has read a buffer it "frees" it by putting it onto a freelist.
 */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>

#include <assert.h> // DO MORE OF THIS BRUH!

#define TEST_CASES 1000000
#define TEST_RUNS 30

#define INTS_PER_CACHE_LINE 16

// Channels for passing data between threads. We pack a whole cache line of ints before
// giving it to the consumer to get this to run as efficiently as possible and avoid
// sharing
typedef struct IntBuffer {
    int size;
    int* data;
    struct IntBuffer* next;
} IntBuffer;

typedef struct {
    int q_head;
    int q_tail;
    int q_capacity;
    int q_size;
    IntBuffer* queue[5];

    int buffer_count;   // must be > 2
    int max_buffer_count;
    IntBuffer buffers[3];
    IntBuffer* first_free_buffer;

    void* memptr;
    
    pthread_mutex_t mx;

    int readers_waiting;
    pthread_cond_t reading;

    int writers_waiting;
    pthread_cond_t writing;
} Chan;

// Job state for the worker threads
typedef enum {
    JOB_BUFFER,    // 4 of these
    JOB_MIDDLEMAN, // 3 of these
    JOB_PRINTER    // 1 of these
} Job;

typedef struct {
    Job job;
    int id;
    union {
        struct {int* buffer_start; int buffer_count; Chan* out;} buffer;
        struct {Chan* left; Chan* right; Chan* out;} middleman;
        struct {Chan* in;} printer;
    };
    pthread_mutex_t wait;
    pthread_cond_t go;
} Task;


// why this blocking
void
write_val(IntBuffer* buffer, int* index, Chan* chan, int val)
{
    if (buffer == NULL) {
        pthread_mutex_lock(&chan->mx);
        while(chan->buffer_count == chan->max_buffer_count &&
              chan->first_free_buffer == NULL) {
            // Wait for new buffer to be available.
            chan->writers_waiting++;
            pthread_cond_wait(&chan->writing, &chan->mx);
            chan->writers_waiting--;
        }

        // Get new buffer.
        if (chan->first_free_buffer) {
            buffer = chan->first_free_buffer;
            chan->first_free_buffer = buffer->next;
        } else if (chan->buffer_count < chan->max_buffer_count) {
            buffer = chan->buffers + chan->buffer_count++;
        }
        
        *index = 0;
        pthread_mutex_unlock(&chan->mx);
    }

    fprintf(stderr, "Writing %i to %i\n", val, *index);
    buffer->data[*index] = val;
    *index = (*index) + 1;
    fprintf(stderr, "Index now %i, buffer->size is %i\n", *index, buffer->size);

    if (*index == buffer->size) {
        pthread_mutex_lock(&chan->mx);
        while(chan->q_capacity == chan->q_size) {
            chan->writers_waiting++;
            pthread_cond_wait(&chan->writing, &chan->mx);
            chan->writers_waiting--;
        }

        // Put buffer on queue.
        chan->queue[chan->q_tail++] = buffer;
        if (chan->q_tail == chan->q_capacity) {
            chan->q_tail = 0;
        }
        chan->q_size++;

        if (chan->readers_waiting) {
            pthread_cond_signal(&chan->reading);
        }
        
        pthread_mutex_unlock(&chan->mx);
    }
}

int
read_val(IntBuffer* buffer, int* index, Chan* chan)
{
    if (buffer == NULL) {
        pthread_mutex_lock(&chan->mx);
        while(chan->q_size == 0) {
            chan->readers_waiting++;
            pthread_cond_wait(&chan->reading, &chan->mx);
            chan->readers_waiting--;
        }

        // Grab buffer from queue.
        buffer = chan->queue[chan->q_head++];
        *index = 0;
        if (chan->q_head == chan->q_capacity) {
            chan->q_head = 0;
        }
        chan->q_size--;

        if (chan->writers_waiting) {
            pthread_cond_signal(&chan->writing);
        }
        pthread_mutex_unlock(&chan->mx);
    }

    int value = buffer->data[(*index)++];

    if (*index == buffer->size) {
        pthread_mutex_lock(&chan->mx);
        buffer->next = chan->first_free_buffer;
        chan->first_free_buffer = buffer;
        buffer = NULL;
        *index = 0;
        if (chan->writers_waiting) {
            pthread_cond_signal(&chan->writing);
        }
        pthread_mutex_unlock(&chan->mx);
    }

    return value;
}

void*
task_worker(void* task_data)
{
    Task* task = (Task*)task_data;
    pthread_mutex_lock(&task->wait);
    pthread_cond_wait(&task->go, &task->wait);
    pthread_mutex_unlock(&task->wait);

    char* task_name;

    switch(task->job) {
    case(JOB_BUFFER): {
        /* 
         * Search buffer for the smallest element.
         * Put smallest element on q.
         * Continue until out of elements.
         */
        task_name = "BUFFER";

        Chan* out = task->buffer.out;

        IntBuffer* out_buffer = NULL;
        int out_index = 0;

        for(;;) {
            fprintf(stderr, "index starts as: %i\n", out_index);
            // Find the smallest element.
            int smallest_index = -1;
            int smallest = RAND_MAX;
            for (int i=0; i<task->buffer.buffer_count; i++) {
                if (task->buffer.buffer_start[i] >= 0 &&
                    task->buffer.buffer_start[i] <= smallest) {
                    smallest_index = i;
                    smallest = task->buffer.buffer_start[i];
                }
            }

            if (smallest_index == -1) {
                do {
                    write_val(out_buffer, &out_index, out, -1);
                } while (out_index != 0);

                break;
            } else {
                write_val(out_buffer, &out_index, out, smallest);
                task->buffer.buffer_start[smallest_index] = -1;
            }
            fprintf(stderr, "index ends as: %i\n", out_index);
        }
        
    } break;
    case(JOB_MIDDLEMAN): {
        /*
         * Pull 1 element from each input queue.
         * When you have two elements, put smallest value on output queue and pull new value.
         * Continue until both input queue's are empty.
         */
        task_name = "MIDDLEMAN";

        Chan* left = task->middleman.left;
        Chan* right = task->middleman.right;
        Chan* out = task->middleman.out;

        int left_drained = 0;
        int right_drained = 0;

        int left_element = -1;
        int right_element = -1;

        IntBuffer* left_buffer = NULL;
        int left_index = 0;

        IntBuffer* right_buffer = NULL;
        int right_index = 0;

        IntBuffer* out_buffer = NULL;
        int out_index = 0;

        int need_new_left = 1;
        int need_new_right = 1;

        for (;;) {
            int out_element = -1;
            // pull needed elements
            if (need_new_left) {
                left_element = read_val(left_buffer, &left_index, left);
            }

            if (need_new_right) {
                right_element = read_val(right_buffer, &right_index, right);
            }

            // pick smallest element
            need_new_right = 0;
            need_new_left = 0;
            if (left_drained && right_drained) {
                out_element = -1;
            } else if (left_drained) {
                out_element = right_element;
                need_new_right = 1;
            } else if (right_drained) {
                out_element = left_element;
                need_new_left = 1;
            } else if (left_element < right_element) {
                out_element = left_element;
                need_new_left = 1;
            } else {
                out_element = right_element;
                need_new_right = 1;
            }

            if (out_element == -1) {
                do {
                    write_val(out_buffer, &out_index, out, -1);
                } while (out_index != 0);
                break;

            } else {
                write_val(out_buffer, &out_index, out, out_element);
            }
        }
    } break;

    case(JOB_PRINTER): {
        /*
         * Pull elements from input queue and print them.
         */
        task_name = "PRINTER";

        Chan* in = task->printer.in;

        int in_index = 0;
        IntBuffer* in_buffer = NULL;
        for (;;) {
            int element = read_val(in_buffer, &in_index, in);
            if (element == -1) {
                break;
            }
            printf("%i\n", element);
        }
    } break;
    };
    
    return NULL;
}

void
chan_alloc(Chan* c)
{
    c->q_head = 0;
    c->q_tail = 0;
    c->q_capacity = 5;
    c->q_size = 0;

    c->buffer_count = 0;
    c->max_buffer_count = 3;

    c->memptr = malloc(INTS_PER_CACHE_LINE*3*sizeof(int)+63);
    int* aligned_data = (int*)((long)c->memptr+63 & ~63);
    for (int i=0; i<3; i++) {
        c->buffers[i].size = INTS_PER_CACHE_LINE;
        c->buffers[i].data = aligned_data + (i * INTS_PER_CACHE_LINE);
    }
    c->first_free_buffer = NULL;

    c->readers_waiting = 0;
    c->writers_waiting = 0;

    pthread_mutex_init(&c->mx, NULL);
    pthread_cond_init(&c->reading, NULL);
    pthread_cond_init(&c->writing, NULL);
}

int
main(void)
{   
    fprintf(stderr, "Starting Test\n");
    srand(time(NULL));

    int* num_buffer = malloc(sizeof(int) * TEST_CASES);
    int num_count = 0;

    // Set up threads
    Task tasks[8];
    pthread_t threads[8];
    Chan chans[7];

    int i;
    for (i=0; i<8; i++) {
        Chan* c = chans + i;
        chan_alloc(c);
    }
    
    Task* task;
    pthread_t* thread;
    // Buffer threads
    for (i=0;i<4;i++) {
        task = tasks + i;
        thread = threads + i;

        task->id = i;
        task->job = JOB_BUFFER;
        task->buffer.buffer_start = num_buffer + (i * TEST_CASES / 4);
        task->buffer.buffer_count = TEST_CASES / 4;
        task->buffer.out = chans + i;
        pthread_mutex_init(&task->wait, NULL);
        pthread_cond_init(&task->go, NULL);
        
        pthread_create(thread, NULL, task_worker, (void*)task);
    }

    // Middlemen
    task = tasks + i;
    thread = threads + i;
    task->id = i;
    task->job = JOB_MIDDLEMAN;
    task->middleman.left = &chans[0];
    task->middleman.right = &chans[1];
    task->middleman.out = &chans[4];
    pthread_mutex_init(&task->wait, NULL);
    pthread_cond_init(&task->go, NULL);
    pthread_create(thread, NULL, task_worker, (void*)task);
    i++;

    task = tasks + i;
    thread = threads + i;
    task->job = JOB_MIDDLEMAN;
        task->id = i;
    task->middleman.left = &chans[2];
    task->middleman.right = &chans[3];
    task->middleman.out = &chans[5];
    pthread_mutex_init(&task->wait, NULL);
    pthread_cond_init(&task->go, NULL);
    pthread_create(thread, NULL, task_worker, (void*)task);
    i++;

    task = tasks + i;
    thread = threads + i;
    task->job = JOB_MIDDLEMAN;
    task->id = i;
    task->middleman.left = &chans[4];
    task->middleman.right = &chans[5];
    task->middleman.out = &chans[6];
    pthread_mutex_init(&task->wait, NULL);
    pthread_cond_init(&task->go, NULL);
    pthread_create(thread, NULL, task_worker, (void*)task);
    i++;

    // Printer
    task = tasks + i;
    thread = threads + i;
    task->id = i;
    task->job = JOB_PRINTER;
    task->printer.in = &chans[6];
    pthread_create(thread, NULL, task_worker, (void*)task);

    // @TODO: I need a way to signal to all the threads that it's time to begin.

//    for (int run=0; run<TEST_RUNS; run++) {
    for (int run=0; run<1; run++) {
        // Random numbers in (0 to RAND_MAX)
        for (int i=0; i<TEST_CASES; i++) {
            num_buffer[num_count++] = rand();
        }

        fprintf(stderr, "Running test case: %i\n", run);

        // Start threads
        for (int i=0; i<8; i++) {
            pthread_mutex_lock(&tasks[i].wait);
            pthread_cond_signal(&tasks[i].go);
            pthread_mutex_unlock(&tasks[i].wait);
        }
        // Wait for threads to complete.
        for (int i=0; i<8; i++) {
            pthread_join(threads[i], NULL);

        }

        num_count = 0;
    }
    
    fprintf(stderr, "Done\n");
    
}
 
