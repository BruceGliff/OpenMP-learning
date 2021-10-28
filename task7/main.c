#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <omp.h>

typedef struct _Frame {
  int heigh;
  int width;
  double ** data;
} Frame;

void createFrame(Frame * F);
void print(Frame * f, int it);
void dump(Frame * f, int it);
void deleteFrame(Frame * f);
double calculateTemp(Frame * Front, Frame * Back, int i, int j) {
  int const M = Back->heigh;
  int const N = Back->width;
  double Temp = 0.0;
  if (i == 0) {
    if (j == 0)
      Temp = (Back->data[i + 1][0] + Back->data[i][1]) / 2.0;
    else if (j == N - 1)
      Temp = (Back->data[i][j - 1] + Back->data[i + 1][j]) / 2.0;
    else
      Temp = (Back->data[i][j + 1] + Back->data[i][j - 1] + Back->data[i + 1][j]) / 3.0;
  } else if (i == M - 1) {
    if (j == 0)
      Temp = (Back->data[i - 1][0] + Back->data[i][1]) / 2.0;
    else if (j == N - 1)
      Temp = (Back->data[i][j - 1] + Back->data[i - 1][j]) / 2.0;
    else
      Temp = (Back->data[i][j + 1] + Back->data[i][j - 1] + Back->data[i - 1][j]) / 3.0;
  } else if (j == 0) {
    Temp = (Back->data[i - 1][j] + Back->data[i + 1][j] + Back->data[i][j + 1]) / 3.0;
  } else if (j == N - 1) {
    Temp = (Back->data[i - 1][j] + Back->data[i + 1][j] + Back->data[i][j - 1]) / 3.0;
  }
  else
    Temp = (Back->data[i][j - 1] + Back->data[i][j + 1] + Back->data[i - 1][j] + Back->data[i + 1][j]) / 4.0;

  Front->data[i][j] = Temp;
  return Temp;
}

void swap(Frame * Front, Frame * Back) {
  #pragma omp parallel for
  for (int i = 0; i != Front->heigh; ++i)
    memcpy(Back->data[i], Front->data[i], Front->width * sizeof(double));
}


int main(int argc, char *argv[]) {

  int const M = 50;
  int const N = 60;

  double e = 1e-2;

  // create frames
  Frame Front = {M, N};
  Frame Back = {M, N};

  createFrame(&Front);
  createFrame(&Back);
  
  // init state
  double err = 1.0;
  unsigned Iteration = 0;
  double start = omp_get_wtime();

  while (err > e) {
    double prev = Back.data[M/2][N/2]; // anchor temperature
    double loc_err = 0.f;
    // data are independent, so it is easy to parallel.
    #pragma omp parallel for
    for (int i = 0; i != M; ++i)
      for (int j = 0; j != N; ++ j) {
        double const curr = calculateTemp(&Front, &Back, i, j); // calculation of Temperature at pos i,j
        double const delta = fabs(curr - prev); // calculation delta of two frames
        prev = curr;
        loc_err = delta > loc_err ? delta : loc_err; // err
      }
    // update rendered frame
    swap(&Front, &Back);

    //print(&Front, Iteration); // to dump images!. ./dump dir has exist!.
    dump(&Front, Iteration); // dump in console.
    err = loc_err;
    Iteration++;
  }

  double end = omp_get_wtime();
  printf("time: %lf\n", end - start);

  deleteFrame(&Front);
  deleteFrame(&Back);

  return 0;
}

void dump(Frame * f, int it) {
  printf("It: %d\n", it);
  for (int i = 0; i != f->heigh; ++i) {
    for (int j = 0; j != f->width; ++j) {
      printf("%lf ", f->data[i][j]);
    }
    printf("\n");
  }
}

void deleteFrame(Frame * f) {
  for (int i = 0; i != f->heigh; ++i)
    free(f->data[i]);
  free(f->data);
}

void print(Frame * f, int it) {
  char Path[64];
  sprintf(Path, "dump/out_%d.ppm", it);
  unsigned const Size = f->width * f->heigh * 3 * sizeof(char);
  unsigned char * ColorOut = (unsigned char *) malloc (Size);
  int fd_out = open(Path,  O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
  if (fd_out == -1) {
    perror("Out file error: ");
    return;
  }

  char Buff[1024];
  int const PrefixSize = sprintf(Buff, "P6\n%d %d\n255\n", f->width, f->heigh);
  double weigh = 0.9;
  write(fd_out, Buff, PrefixSize);
  for (int i = 0; i != f->heigh; i++) {
    for (int j = 0; j != f->width; ++j) {
      ColorOut[(i * f->width + j) * 3] = (unsigned char)(2.55 * f->data[i][j] * weigh);
      ColorOut[(i * f->width + j) * 3 + 1] = 0;
      ColorOut[(i * f->width + j) * 3 + 2] = (unsigned char)(2.55 * (1.0 - weigh) * (100.0 - f->data[i][j]));
    }
  }
  write(fd_out, ColorOut, Size);

  close(fd_out);
  free(ColorOut);
}

void createFrame(Frame * F) {
  F->data = (double**) calloc (F->heigh, sizeof(double*));
  for (int i = 0; i != F->heigh; ++i)
    F->data[i] = (double*) calloc (F->width, sizeof(double));

  for (int i = 0; i != F->heigh; ++i) {
    F->data[i][0] = 100.0;
    F->data[i][F->width-1] = 100.0;
  }

  for (int i = 0; i != F->width; ++i) {
    F->data[0][i] = 0.f;
    F->data[F->heigh-1][i] = 100.0;
  }
}