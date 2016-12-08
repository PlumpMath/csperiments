#if 0
cc -Wall -g -std=c99 -o something something.c && ./something;
exit;
#endif

#include <stdio.h>
#include <stdint.h>

int main()
{
    int foo = 1;

    int bar = ((struct {__typeof__(foo) dafoo;}) {.dafoo = (foo)}.dafoo);

    struct {
        int wow;
        int foo;
    } mythings[] = {
        {.wow = 1,
         .foo = 2},
        {.wow = 3,
         .foo = 4},
    };
    
    printf("Foo %i, Bar %i\n", foo, bar);
    printf("My things\n");
    for (int i = 0; i < 2; i++) {

        __typeof__(mythings[0]) *thingy = mythings + i;
        printf("wow: %i, foo %i\n", thingy->wow, thingy->foo);
    }

}

// I just need my fingers to get used to this keyboard. They definately are not used to it yet.


