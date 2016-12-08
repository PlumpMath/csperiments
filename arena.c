#if 0
cc -Wall -g -o arena arena.c && ./arena;
exit;
#endif

#include <stdio.h>
#include <stdint.h>

typedef struct {
    uint8_t *base;
    uint64_t used;
    uint64_t capacity;
} Arena;

void
arenaPushSize(Arena *arena, uint64_t size)
{
    if (arena->used + size > arena->capacity) {
        // allocate a new block
    }
}

int main()
{
    printf("Hello arena\n");
}
