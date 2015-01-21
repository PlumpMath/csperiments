#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

int
main(int argc, char *argv[])
{
    for (int ctr=0; ctr < argc; ctr++ ) {
        puts( argv[ctr] );
    }
}
