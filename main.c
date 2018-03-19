#include "PE.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
using namespace std;

int vec_full_length = 3*3*256;
int mat_row_num = vec_full_length;
int mat_col_num = 384;
int PE_num = 3;
int* PE_scale;
int multiplier_num = 700;

int clk = 0;
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
    PE_list[i].init(i, PE_num, multiplier_num * ((float)PE_scale[i]/scale_all), POLICY_ONE_CONNECT);
  }

  clk = 0;
  task.clear();
  mat_row_nnz = new int[mat_row_num];
}

void SparsityGenerate(int*& array, int length, int range)
{
  srand((unsigned)time(0));
  array[0] = 0;
  for(int i = 1; i < length; ++i){
    array[i] = rand() % (range+1);
  }
  sort(array, array+length);
  for(int i = 0; i < length-1; ++i){
    array[i] = array[i+1] - array[i];
  }
  array[length-1] = range - array[length-1];
  int cnt = 0;
  for(int i = 0; i < length; ++i)
    cnt += array[i];
  // printf("range = %d, cnt = %d\n", range, cnt);
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

void GenerateInput(float A_sparsity, float B_sparsity)
{
  SparsityGenerate(mat_row_nnz, mat_row_num, mat_col_num*mat_row_num*(1-B_sparsity));
  int* vec_gen = new int[vec_full_length];
  SparsityGenerate(vec_gen, vec_full_length, vec_full_length*(1-A_sparsity));
  srand((unsigned)time(0));
  for(int i = 0; i < vec_full_length; ++i){
    if(vec_gen[i] != 0){
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

void Calculate(bool doSteal = true)
{
  while(++clk){
    for(int i = 0; i < PE_num; ++i){
      PE_list[i].calculate();
    }
    if(doSteal){
      for(int i = 0; i < PE_num; ++i){
        PE_list[i].steal(PE_list);
      }
    }
    int cnt = 0;
    for(; cnt < PE_num && PE_list[cnt].finished(); ++cnt);
    if(cnt == PE_num)
      break;
  }
}

void CleanUp()
{
  delete [] mat_row_nnz;
  delete [] PE_list;
}

typedef struct Parameter{
  float A_sparsity;
  float B_sparsity;
  bool doSteal;
}Parameter;

void Test(Parameter p)
{
  int cnt_clk = 0;
  int pass = 200000;
  for(int i = 0; i < pass; ++i){
    Init();
    // GetInput();
    GenerateInput(p.A_sparsity, p.B_sparsity);
    Distribute();
    Calculate(p.doSteal);
    cnt_clk += clk;
    CleanUp();
  }
  printf("clock = %f\n", (float)cnt_clk/pass);
}

int main()
{
  Parameter param;
  // param.A_sparsity = 0.7;
  // param.B_sparsity = 0.7;
  // param.doSteal = false;
  // Test(param);
  for(float a = 0.4; a < 0.71; a += 0.1){
    for(float b = 0.6; b < 0.91; b += 0.1){
      printf("\nA_sparsity = %f, B_sparsity = %f, doSteal = false\n", a, b);
      param.A_sparsity = a;
      param.B_sparsity = b;
      param.doSteal = false;
      Test(param);
      printf("\nA_sparsity = %f, B_sparsity = %f, doSteal = true\n", a, b);
      param.doSteal = true;
      Test(param);
    }
  }
  return 0;
}
