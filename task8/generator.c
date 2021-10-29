#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf("Put size of an array as the 1st param\n\tand out file as the "
           "2nd(optional)\n");
    return -1;
  }

  int size = atoi(argv[1]);
  srand(time(0));
  if (argc == 3) {
    // print in file;
    FILE *f_out = fopen(argv[2], "w");
    if (!f_out) {
      printf("%s\n", argv[1]);
      perror("No file");
      return -1;
    }
    while (size--)
      fprintf(f_out, "%d ", rand() % 10000 + 1);
    fclose(f_out);
  } else
    // printinf in console
    while (size--)
      printf("%d ", rand() % 10000 + 1);

  return 0;
}