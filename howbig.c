#if 0
cc -Wall -g -std=c99 -O2 -o howbig $0 && ./howbig;
exit;
#endif

#include <stdio.h>
#include <stdlib.h>

int main() {
    printf("How big an int %zu\n", sizeof(int));
    printf("How big an int %zu\n", sizeof(float)); 
}

    
