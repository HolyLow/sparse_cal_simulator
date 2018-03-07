#include "PE.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <vector>
using namespace std;

int vec_full_length = 72;
int multiplier_num = 200;
int mat_row_num = 64;
int mat_col_num = 128;

int clock = 0;
vector<int> task;
int* mat_row_nnz;


void Init()
{
  clock = 0;
  task.clear();
  mat_row_nnz = new int[mat_row_num];
}

void GetInput()
{
  srand((unsigned)time(0));
  for(int i = 0; i < mat_row_num; ++i){
    mat_row_nnz[i] = rand() % (mat_col_num + 1);
  }
  for(int i = 0; i < vec_full_length; ++i){
    if(rand() % 2 == 0){
      task.push_back(mat_row_nnz[rand() % mat_row_num]);
    }
  }
}

void Arrange()
{

}

void Calculate()
{
  
}

int main()
{
  Init();
  GetInput();
  Arrange();
  Calculate();
  return 0;
}
