/*
#  deactivate l'ASLR : echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
#  activate l'ASLR : echo 1 | sudo tee /proc/sys/kernel/randomize_va_space
#  gcc ./heapOvf.c -o heapOvf -Werror -m32 -no-pie
*/

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <time.h>

struct object {
  int id;
  char *name;
};

void winner() {
  int instant = time(NULL);
  printf("and we have a winner @ %d\n", instant);
}

int main(int argc, char **argv) {
  if (argc != 3) {
    printf("Usage : heapOvf arg1 arg2\n");
  }
  else {
    struct object *i1, *i2;

    i1 = malloc(sizeof(struct object));
    i1->id = 1;
    i1->name = malloc(8);

    i2 = malloc(sizeof(struct object));
    i2->id = 2;
    i2->name = malloc(8);

    strcpy(i1->name, argv[1]);
    strcpy(i2->name, argv[2]);

    // print pour la  fin du programme
    printf("And that's it !\n");
  }
}