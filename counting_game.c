/*
  This is a very fun game that counts up from 0 unto infinity.
 */
#include <stdio.h>
#include <unistd.h>

static int number = 0;

int main()
{
    for (;;) {
        printf("%i\n", number++);
        sleep(3);
    }
}
