#if 0
cc -Wall -g -std=c99 -o stack stack.c && ./stack;
exit;
#endif

#include <stdio.h>
#include <stdlib.h>

typedef struct StackFrame StackFrame;

struct StackFrame {
    int value;
    StackFrame *next;
};

StackFrame *
stack_frame_create()
{
    StackFrame *frame = (StackFrame *) malloc(sizeof(StackFrame));
    frame->next = NULL;
    return frame;
}

StackFrame*
stack_push(StackFrame *frame, int value)
{
    StackFrame *new_frame = stack_frame_create();
    new_frame->value = value;
    new_frame->next = frame;

    return new_frame;
}

StackFrame*
stack_pop(StackFrame *frame)
{
    if (frame) {
        StackFrame *next = frame->next;
        free(frame);
        return next;
    }
    return frame;
}

void
stack_destroy(StackFrame *frame)
{
    while (frame) {
        printf("free %d\n", (frame)->value);
        stack_pop(frame);
    }
}

int 
main(int argc, char *argv[])
{
    StackFrame *top = NULL;

    top = stack_push(top, 2);
    top = stack_push(top, 4);
    top = stack_push(top, 8);

    StackFrame *curr = top;
    while (curr) {
        printf("%d\n", curr->value);
        curr = curr->next;
    }

    stack_destroy(top);
}
