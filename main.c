#include "PE.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <vector>
using namespace std;

int vec_full_length = 72;
int mat_row_num = 64;
int mat_col_num = 128;
int PE_num = 4;
int* PE_scale;
int multiplier_num = 200;

int clock = 0;
vector<int> task;
int* mat_row_nnz;
PE* PE_list;



void Init()
{
  PE_scale = new int[PE_num];
  PE_list = new PE[PE_num];

  for(int i = 0; i < PE_num; ++i)
    PE_scale[i] = i + 1;

  int scale_all = 0;
  for(int i = 0; i < PE_num; ++i)
    scale_all += PE_scale[i];
  for(int i = 0; i < PE_num; ++i){
    PE_list[i].init(i, PE_num, multiplier_num * ((float)PE_scale[i]/scale_all));
  }

  clock = 0;
  task.clear();
  mat_row_nnz = new int[mat_row_num];
}

void GetInput()
{
  srand((unsigned)time(0));
  for(int i = 0; i < mat_row_num; ++i){
    mat_row_nnz[i] = rand() % (mat_col_num+1);
  }
  for(int i = 0; i < vec_full_length; ++i){
    if(rand()%2 == 0){
      task.push_back(mat_row_nnz[rand()%mat_row_num]);
    }
  }
}

void Distribute()
{
  int task_num = task.size();
  for(int i = 0; i < task_num; ++i){
    for(int j = 0; j < PE_num; ++j){
      for(int k = 0; k < PE_scale[j]; ++k){
        PE_list[j].addTask(task[i]);
      }
    }
  }
}

void Calculate()
{
  while(++clock){
    
  }
}

int main()
{
  Init();
  GetInput();
  Distribute();
  Calculate();
  return 0;
}
