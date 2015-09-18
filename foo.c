#include <stdio.h>

int main(int argc, char *argv[])
{
  char *rules = argv[1];
  char *str = argv[2];

  printf("The values\n");
  printf("%s : %s\n", rules, str);
  
  return 0;
}


// I want to interact with what I'm creating in a smaller feedback loop...
