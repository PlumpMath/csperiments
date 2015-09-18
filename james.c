/*
in C, reverse every word in a string without reversing the order of the
words, with no more than 3 pointers, and no imports except stdio
*/
#include <stdio.h>

void reverse_words(char* the_string)
{
    char* word = the_string;
    char* tail = the_string;

    for (char* word = the_string; ;) {
        for (char* tail = word;
             *tail != ' ';
             ++tail) {
            
        }
    }

}

int main(int argc, char *argv[])
{
    char* the_string = "This is a string to do this.";
    printf("%s\n", the_string);
    reverse_words(the_string);
    printf("%s\n", the_string);
}
