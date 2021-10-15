#include <assert.h>
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define HEIGHT_A 1200
#define WIDTH_A 1015
#define HEIGHT_B WIDTH_A
#define WIDTH_B 1200

// Uniform matrix to handle indices and values.
typedef struct _Matrix {
  int height;
  int width;
  union {
    float **dataF;
    int **dataI;
  };
} Matrix;

// Block is matrix of two elements - idices. So it is represented by two
// matricies, which are interpret data as int.
typedef struct _Block {
  Matrix m1;
  Matrix m2;
} Block;

// Allocates and frees memory
void create_matrix(Matrix *M);
void delete_matrix(Matrix *M);

// Fills Matrix with zeros.
void fill_with_zero(Matrix *M);
// Fills Matrix with random values.
void fill(Matrix *M);

// Prints Matrices and Blocks
void OutMatrix(Matrix *M);
void OutBlock(Block *B);

// Sets values of Block
void SetBlockValues(Block *B, int block_height, int block_width, Matrix *M);
// Calculates the most stupid way.
void linear_calculation(Matrix *A, Matrix *B, Matrix *C);
// Calculates parallel.
void matrix_multiply(Matrix *A, Matrix *B, Matrix *C);

int main(int argc, char *argv[]) {
  Matrix A = {HEIGHT_A, WIDTH_A};
  Matrix B = {HEIGHT_B, WIDTH_B};
  Matrix C = {HEIGHT_A, WIDTH_B};
  create_matrix(&A);
  create_matrix(&B);
  create_matrix(&C);
  fill(&A);
  fill(&B);
  fill_with_zero(&C);

  // Matrix B by default will be transposed.
  int const IntsInMB = 1024 * 1024 / sizeof(int);
  int const WidthValueBlockA = sqrtf(3 * IntsInMB);
  int const HeightValueBlockA =
      WidthValueBlockA + WidthValueBlockA % omp_get_max_threads();
  int const WidthValueBlockB = HeightValueBlockA;
  int const HeightValueBlockB = WidthValueBlockA;

  // Sizes of the blocks
  int const inc_ah = (HEIGHT_A % HeightValueBlockA != 0) ? 1 : 0;
  int const BlockMatrixHeightA = HEIGHT_A / HeightValueBlockA + inc_ah;
  int const inc_aw = (WIDTH_A % WidthValueBlockA != 0) ? 1 : 0;
  int const BlockMatrixWidthA = WIDTH_A / WidthValueBlockA + inc_aw;
  int const inc_bh = (HEIGHT_B % HeightValueBlockB != 0) ? 1 : 0;
  int const BlockMatrixHeightB = HEIGHT_B / HeightValueBlockB + inc_bh;
  int const inc_bw = (WIDTH_B % WidthValueBlockB != 0) ? 1 : 0;
  int const BlockMatrixWidthB = WIDTH_B / WidthValueBlockB + inc_bw;

  // Blocks
  Block BlockA = {{BlockMatrixHeightA, BlockMatrixWidthA},
                  {BlockMatrixHeightA, BlockMatrixWidthA}};
  Block BlockB = {{BlockMatrixHeightB, BlockMatrixWidthB},
                  {BlockMatrixHeightB, BlockMatrixWidthB}};
  create_matrix(&BlockA.m1);
  create_matrix(&BlockA.m2);
  create_matrix(&BlockB.m1);
  create_matrix(&BlockB.m2);
  SetBlockValues(&BlockA, HeightValueBlockA, WidthValueBlockA, &A);
  SetBlockValues(&BlockB, HeightValueBlockB, WidthValueBlockB, &B);

  double Begin = omp_get_wtime();

  int MaxHeight = 0;
  int MaxWidth = 0;

  for (int ix = 0; ix != BlockMatrixHeightA; ++ix)
    for (int iy = 0; iy != BlockMatrixWidthB; ++iy)
      for (int i = 0; i != BlockMatrixWidthA; ++i) {
        MaxHeight = MaxHeight > BlockA.m1.dataI[ix][i] ? MaxHeight
                                                       : BlockA.m1.dataI[ix][i];
        MaxHeight = MaxHeight > BlockB.m2.dataI[i][iy] ? MaxHeight
                                                       : BlockB.m2.dataI[i][iy];
        MaxHeight = MaxHeight > BlockA.m1.dataI[ix][iy]
                        ? MaxHeight
                        : BlockA.m1.dataI[ix][iy];

        MaxWidth = MaxWidth > BlockA.m2.dataI[ix][i] ? MaxWidth
                                                     : BlockA.m2.dataI[ix][i];
        MaxWidth = MaxWidth > BlockB.m1.dataI[i][iy] ? MaxWidth
                                                     : BlockB.m1.dataI[i][iy];
        MaxWidth = MaxWidth > BlockB.m2.dataI[ix][iy] ? MaxWidth
                                                      : BlockB.m2.dataI[ix][iy];
      }

  // Preallocates memory for blocks to avoid realocations in main loop.
  // Little memory overhead in some cases.
  Matrix BlockMatrixARawMem = {MaxHeight, MaxWidth};
  Matrix BlockMatrixBRawMem = {MaxHeight, MaxWidth};
  Matrix BlockMatrixCRawMem = {MaxHeight, MaxWidth};
  create_matrix(&BlockMatrixARawMem);
  create_matrix(&BlockMatrixBRawMem);
  create_matrix(&BlockMatrixCRawMem);

  // Main loop
  for (int ix = 0; ix != BlockMatrixHeightA; ++ix)
    for (int iy = 0; iy != BlockMatrixWidthB; ++iy)
      for (int i = 0; i != BlockMatrixWidthA; ++i) {

        Matrix BlockMatrixA = {BlockA.m1.dataI[ix][i], BlockA.m2.dataI[ix][i]};
        BlockMatrixA.dataI = BlockMatrixARawMem.dataI;
        for (int Ax = 0; Ax != BlockA.m1.dataI[ix][i]; ++Ax)
          for (int Ay = 0; Ay != BlockA.m2.dataI[ix][i]; ++Ay)
            BlockMatrixA.dataI[Ax][Ay] = A.dataI[Ax + (ix * HeightValueBlockA)]
                                                [Ay + (i * WidthValueBlockA)];

        Matrix BlockMatrixB = {BlockB.m2.dataI[i][iy], BlockB.m1.dataI[i][iy]};
        BlockMatrixB.dataI = BlockMatrixBRawMem.dataI;
        for (int Bx = 0; Bx != BlockB.m1.dataI[i][iy]; ++Bx)
          for (int By = 0; By != BlockB.m2.dataI[i][iy]; ++By)
            BlockMatrixB.dataI[By][Bx] = B.dataI[Bx + (i * HeightValueBlockB)]
                                                [By + (iy * WidthValueBlockB)];

        Matrix BlockMatrixC = {BlockA.m1.dataI[ix][iy],
                               BlockB.m2.dataI[ix][iy]};
        BlockMatrixC.dataI = BlockMatrixCRawMem.dataI;
        fill_with_zero(&BlockMatrixC);

        matrix_multiply(&BlockMatrixA, &BlockMatrixB, &BlockMatrixC);

        for (int Cx = 0; Cx < BlockA.m1.dataI[ix][iy]; ++Cx)
          for (int Cy = 0; Cy < BlockB.m2.dataI[ix][iy]; ++Cy)
            C.dataF[Cx + (ix * HeightValueBlockA)]
                   [Cy + (iy * WidthValueBlockB)] += BlockMatrixC.dataF[Cx][Cy];
      }

  double End = omp_get_wtime();
  printf("Time: %fs\n", End - Begin);
  fflush(NULL);

  delete_matrix(&BlockA.m1);
  delete_matrix(&BlockA.m2);
  delete_matrix(&BlockB.m1);
  delete_matrix(&BlockB.m2);
  delete_matrix(&BlockMatrixARawMem);
  delete_matrix(&BlockMatrixBRawMem);
  delete_matrix(&BlockMatrixCRawMem);

  // If there is any argument
  if (argc > 1) {
    // DO checking
    Matrix C_original = {HEIGHT_A, WIDTH_B};
    create_matrix(&C_original);
    fill_with_zero(&C_original);
    linear_calculation(&A, &B, &C_original);
    for (int i = 0; i != C_original.height; ++i)
      for (int j = 0; j != C_original.width; ++j)
        if (C.dataF[i][j] != C_original.dataF[i][j]) {
          printf("Missmatching at pos: [%d][%d]!\n Expected: %f. Gooted: %f.\n",
                 i, j, C_original.dataF[i][j], C.dataF[i][j]);

          delete_matrix(&A);
          delete_matrix(&B);
          delete_matrix(&C);
          delete_matrix(&C_original);
          return -1;
        }
    printf("Matrix has been checked!\n");
    delete_matrix(&C_original);
  }

  printf("Matrix has been calculated!\n");

  delete_matrix(&A);
  delete_matrix(&B);
  delete_matrix(&C);

  return 0;
}

void create_matrix(Matrix *M) {
  assert(M && !M->dataI && "Matrix is NULL or Matrix is not empty!");
  M->dataI = (int **)malloc(sizeof(int *) * M->height);
  for (int i = 0; i != M->height; ++i)
    M->dataI[i] = (int *)malloc(sizeof(int) * M->width);
}

void delete_matrix(Matrix *M) {
  assert(M && M->dataI && "Matrix is NULL or Matrix is empty!");
  for (int i = 0; i != M->height; ++i)
    free(M->dataI[i]);
  free(M->dataI);
}

void fill(Matrix *M) {
  assert(M && M->dataI && "Matrix is NULL or Matrix is empty!");
  srand((unsigned int)time(NULL));
  for (int i = 0; i != M->height; ++i)
    for (int j = 0; j != M->width; ++j)
      M->dataF[i][j] =
          (float)(rand() % 100); //((float)rand()/(float)(RAND_MAX)) * 4.f;
}

void fill_with_zero(Matrix *M) {
  assert(M && M->dataI && "Matrix is NULL or Matrix is empty!");
  for (int i = 0; i != M->height; ++i)
    memset(M->dataI[i], 0, M->width * sizeof(int));
}

void SetBlockValues(Block *B, int block_height, int block_width, Matrix *M) {
  assert(B && M && "Block or Matrix are NULL");
  int mat_b_w = B->m1.width;
  int mat_b_h = B->m1.height;
  for (int i = 0; i != mat_b_h - 1; ++i) {
    for (int j = 0; j != mat_b_w - 1; ++j) {
      B->m1.dataI[i][j] = block_height;
      B->m2.dataI[i][j] = block_width;
    }
    B->m1.dataI[i][mat_b_w - 1] = block_height;
    B->m2.dataI[i][mat_b_w - 1] = M->width - (mat_b_w - 1) * block_width;
  }
  for (int i = 0; i != mat_b_w - 1; ++i) {
    B->m1.dataI[mat_b_h - 1][i] = M->height - (mat_b_h - 1) * block_height;
    B->m2.dataI[mat_b_h - 1][i] = block_width;
  }
  B->m1.dataI[mat_b_h - 1][mat_b_w - 1] =
      M->height - (mat_b_h - 1) * block_height;
  B->m2.dataI[mat_b_h - 1][mat_b_w - 1] =
      M->width - (mat_b_w - 1) * block_width;
}

void OutMatrix(Matrix *M) {
  assert(M && "Matrix is NULL");
  for (int i = 0; i != M->height; ++i) {
    for (int j = 0; j != M->width; ++j)
      printf("%f ", M->dataF[i][j]);
    printf("\n");
  }
}

void OutBlock(Block *B) {
  assert(B && "Block is NULL");
  for (int i = 0; i != B->m1.height; ++i) {
    for (int j = 0; j != B->m1.width; ++j)
      printf("(%d %d) ", B->m1.dataI[i][j], B->m2.dataI[i][j]);
    printf("\n");
  }
  printf("\n");
}

void linear_calculation(Matrix *A, Matrix *B, Matrix *C) {
  assert(A && B && C && "Matrices are NULL");
  for (int ix = 0; ix < C->height; ++ix)
    for (int iy = 0; iy < C->width; ++iy)
      for (int i = 0; i < A->width; ++i)
        C->dataF[ix][iy] += A->dataF[ix][i] * B->dataF[i][iy];
}

void matrix_multiply(Matrix *A, Matrix *B, Matrix *C) {
  assert(A && B && C && "Matrices are NULL");

#pragma omp parallel for
  for (int ix = 0; ix < C->height; ++ix)
    for (int iy = 0; iy < C->width; ++iy)
      for (int i = 0; i < A->width; ++i)
        C->dataF[ix][iy] += A->dataF[ix][i] * B->dataF[iy][i];
}