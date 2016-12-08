#if 0
echo Compiling...;
clang -Wall -std=c11 -g $0 -o disruptor;
exit;
#endif

// Try this disruptor ring buffer thing out for multiple consumers and producers
// https://news.ycombinator.com/item?id=12054503
// http://lmax-exchange.github.io/disruptor/files/Disruptor-1.0.pdf

#include <stdio.h>

int
main()
{
    printf("Hello World Disruptor!\n");
}
