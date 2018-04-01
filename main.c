#include "PE.h"

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <vector>
#include <string.h>
#include <algorithm>
using namespace std;

int vec_full_length = 3*3*64;
int vec_row_num = 64;
int mat_row_num = vec_full_length;
int mat_col_num = 64;
int PE_num = 16;
float vec_sparsity = 0.5;
float mat_sparsity = 0.9;

int* PE_scale;
int multiplier_num = 64;
static int normalize_mul_num = 128;

int clk = 0;
int** sparse_task_mat;
vector<int> task;
int* mat_row_nnz;
int mat_nnz;
int* vec_row_nnz;
int vec_nnz;

PE* PE_list;

void Check(bool flag, string msg)
{
  if(!flag){
    printf("error: %s\n", msg.c_str());
    exit(1);
  }
}

void InitSettings(int* scale = NULL)
{
  PE_scale = new int[PE_num];
  PE_list = new PE[PE_num];

  PE::static_init(PE_list, PE_num);
  if(scale == NULL)
    for(int i = 0; i < PE_num; ++i)
      PE_scale[i] = 1;
      // PE_scale[i] = 1;
  else
    for(int i = 0; i < PE_num; ++i)
      PE_scale[i] = scale[i];
  int scale_all = 0;
  for(int i = 0; i < PE_num; ++i)
    scale_all += PE_scale[i];
  int multiplier_cnt = 0;
  for(int i = 0; i < PE_num; ++i){
    PE_list[i].init(i, multiplier_num * ((float)PE_scale[i]/scale_all), POLICY_ALL_CONNECT);
    multiplier_cnt +=  multiplier_num * ((float)PE_scale[i]/scale_all);
  }
  multiplier_num = multiplier_cnt;

  clk = 0;
  task.clear();
}

void SparsityMatrixGenerate(int*& row_nnz, int row, int col, float sparsity)
{
  int size = row * col;
  int nnz = row*col*(1-sparsity);
  int* flag = new int[size];
  for(int i = 0; i < size; ++i){
    flag[i] = i;
  }
  srand((unsigned)time(0));
  for(int i = 0; i < nnz; ++i){
    int r = (rand() % (nnz - i)) + i;
    int tmp = flag[r];
    flag[r] = flag[i];
    flag[i] = tmp;
  }
  for(int i = 0; i < nnz; ++i){
    row_nnz[flag[i]%row]++;
  }
  for(int i = 0; i < row; ++i){
    printf("%d ", row_nnz[i]);
  }
  printf("\n");
  delete [] flag;


  // srand((unsigned)time(0));
  // array[0] = 0;
  // for(int i = 1; i < length; ++i){
  //   array[i] = rand() % (range+1);
  // }
  // sort(array, array+length);
  // for(int i = 0; i < length-1; ++i){
  //   array[i] = array[i+1] - array[i];
  // }
  // array[length-1] = range - array[length-1];
  // int cnt = 0;
  // for(int i = 0; i < length; ++i)
  //   cnt += array[i];
  // printf("range = %d, cnt = %d\n", range, cnt);
}

void rand_pick_k_nums_from_n(int*& array, int k, int n)
{
  // int task_num = vec_row_nnz[id];
  int* flag = new int[n];
  for(int i = 0; i < n; ++i){
    flag[i] = i;
  }
  // printf("over0\n");
  srand((unsigned)time(0));
  for(int i = 0; i < k; ++i){
    // printf("i = %d\n", i);
    int r = (rand() % (n - i)) + i;
    int tmp = flag[r];
    flag[r] = flag[i];
    flag[i] = tmp;
  }
  // printf("over1\n");
  // task.clear();
  for(int i = 0; i < k; ++i){
    array[i] = flag[i];
  }
  delete [] flag;
}

void GetInput()
{
  int nnz = 0;
  scanf("%d%d", &vec_row_num, &vec_full_length);
  vec_row_nnz = new int[vec_row_num];
  for(int i = 0; i < vec_row_num; ++i){
    scanf("%d", &vec_row_nnz[i]);
    nnz += vec_row_nnz[i];
  }
  vec_sparsity = 1 - nnz / (float)(vec_row_num*vec_full_length);
  vec_nnz = nnz;
  nnz = 0;
  scanf("%d%d", &mat_row_num, &mat_col_num);
  mat_row_nnz = new int[mat_row_num];
  for(int i = 0; i < mat_row_num; ++i){
    scanf("%d", &mat_row_nnz[i]);
    nnz += mat_row_nnz[i];
  }
  mat_nnz = nnz;
  mat_sparsity = 1 - nnz / (float)(mat_row_num*mat_col_num);
  printf("successfully read the input\n");

  sparse_task_mat = new int*[vec_row_num];
  srand(time(0));
  for(int i = 0; i < vec_row_num; ++i){
    sparse_task_mat[i] = new int[vec_row_nnz[i]];
    rand_pick_k_nums_from_n(sparse_task_mat[i], vec_row_nnz[i], vec_full_length);
    for(int j = 0; j < vec_row_nnz[i]; ++j){
      int idx = sparse_task_mat[i][j];
      sparse_task_mat[i][j] = mat_row_nnz[idx];
    }
  }
}

void GenerateInput(float A_sparsity, float B_sparsity)
{
  vec_row_nnz = new int[vec_row_num];
  memset(vec_row_nnz, 0, vec_row_num*sizeof(int));
  mat_row_nnz = new int[mat_row_num];
  memset(mat_row_nnz, 0, mat_row_num*sizeof(int));
  SparsityMatrixGenerate(vec_row_nnz, vec_row_num, vec_full_length, A_sparsity);
  SparsityMatrixGenerate(mat_row_nnz, mat_row_num, mat_col_num, B_sparsity);
  // int* vec_gen = new int[vec_full_length];
  // SparsityGenerate(vec_gen, vec_full_length, vec_full_length*(1-A_sparsity));
  // srand((unsigned)time(0));
  // for(int i = 0; i < vec_full_length; ++i){
  //   if(vec_gen[i] != 0){
  //     task.push_back(mat_row_nnz[rand()%mat_row_num]);
  //   }
  // }
}

void InitTask(int id)
{
  int task_num = vec_row_nnz[id];
  // int* flag = new int[vec_full_length];
  // for(int i = 0; i < vec_full_length; ++i){
  //   flag[i] = i;
  // }
  // // printf("over0\n");
  // srand((unsigned)time(0));
  // for(int i = 0; i < task_num; ++i){
  //   // printf("i = %d\n", i);
  //   int r = (rand() % (vec_full_length - i)) + i;
  //   int tmp = flag[r];
  //   flag[r] = flag[i];
  //   flag[i] = tmp;
  // }
  // printf("over1\n");
  task.clear();
  for(int i = 0; i < task_num; ++i){
    task.push_back(sparse_task_mat[id][i]);
    // task.push_back(mat_row_nnz[flag[i]]);
  }
  // delete [] flag;
  // printf("over2\n");
}

#define DIS_THRESHOLD 0
#define DIS_AVERAGE   1
#define DIS_GROUP     2
int dis_policy = DIS_AVERAGE;
int group_num = 4;
void Distribute()
{
  int task_num = task.size();
  int* threshold = new int[PE_num];
  int cnt = 0;
  int p = 0;
  switch (dis_policy) {
    case DIS_THRESHOLD:
      for(int i = 1; i <= PE_num; ++i){
        threshold[i] = (float)i/PE_num * mat_col_num;
      }
      for(int i = 0; i < task_num; ++i){
        for(int j = 0; j < PE_num; ++j){
          if(task[i] <= threshold[j]){
            PE_list[j].addTask(task[i]);
            break;
          }
        }
      }
      break;
    case DIS_AVERAGE:
      while(cnt < task_num){
        for(int i = 0; i < PE_num; ++i){
          for(int j = 0; j < PE_scale[i]; ++j){
            PE_list[i].addTask(task[cnt]);
            ++cnt;
            if(cnt >= task_num)
              break;
          }
          if(cnt >= task_num)
            break;
        }
      }
      break;
    case DIS_GROUP:
      while(cnt < task_num){
        p = 0;
        while(p < PE_num){
          for(int i = 0; i < PE_num/group_num; ++i){
            int* buffer = new int[group_num];
            int length = 0;
            int mark = cnt + group_num;
            for(int tail = cnt; tail < mark && tail < task_num; ++tail, ++length, ++cnt){
              buffer[length] = task[cnt];
              // printf("in, length = %d\n", length);
            }
            // printf("length = %d\n", length);
            sort(buffer, buffer+length);
            for(int j = 0; j < length; ++j){
              PE_list[p].addTask(buffer[j]);
              ++p;
            }
            delete [] buffer;
            if(cnt >= task_num) break;
          }
          if(cnt >= task_num) break;
        }
      }
      break;
    default:
      Check(false, "Undefined dis_policy");
  }
  delete [] threshold;
  // for(int i = 0; i < PE_num; ++i){
  //   PE_list[i].report();
  // }
  // printf("distributed successfully\n\n\n");

}

void Calculate()
{
  while(++clk){
    for(int i = 0; i < PE_num; ++i){
      PE_list[i].calculate();
    }
    for(int i = 0; i < PE_num; ++i){
      PE_list[i].steal();
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

void Run()
{
  int cnt_clk = 0;
  clk = 0;
  for(int i = 0; i < vec_row_num; ++i){
    InitTask(i);
    // printf("OK0\n");
    Distribute();
    // printf("OK1\n");
    Calculate();
    // printf("OK2\n");
    cnt_clk += clk;
    clk = 0;
  }
  printf("norm_clock = %d\n, B_dense norm_clock = %d(speedup = %f), A&N_dense norm_clock = %d(speedup = %f)\n", cnt_clk*multiplier_num/normalize_mul_num,
         vec_nnz*mat_col_num/normalize_mul_num, (float)vec_nnz*mat_col_num/multiplier_num/cnt_clk,
         vec_row_num*mat_row_num*mat_col_num/normalize_mul_num, (float)vec_row_num*mat_row_num*mat_col_num/multiplier_num/cnt_clk);
  printf("PE_task_overall = %d, optimal clock = %d(speedup = %f)\n", PE::overall_task(), PE::overall_task()/normalize_mul_num, (float)PE::overall_task()/multiplier_num/cnt_clk);
  for(int i = 0; i < PE_num; ++i){
    PE_list[i].report();
    PE_list[i].cleanup();
  }

  float idel_rate = (float)PE::overall_idel_clk() / (cnt_clk*PE_num);
  fprintf(stderr, "\"%d\",\"%.3f\"\n", cnt_clk, idel_rate);
}

void SettingReport(string msg)
{
  printf("\n\n%s, multiplier_num = %d, A_sparsity = %f, B_sparsity = %f\n", msg.c_str(), multiplier_num, vec_sparsity, mat_sparsity);

  fprintf(stderr, "\"%d\",\"%s\",\"%d\",\"%.3f\",\"%.3f\",", group_num, msg.c_str(), multiplier_num, vec_sparsity, mat_sparsity);
}

int main()
{
  GetInput();
  InitSettings();


  PE_num = 16;
  group_num = 4;

  // multiplier_num = 128;
  // int scale_schedule4_1[16] = {1, 2, 4, 8, 1, 2, 4, 8, 1, 2, 4, 8, 1, 2, 4, 8};
  // InitSettings(scale_schedule4_1);
  PE::sync_policy = SYNC_POLICY_ELEMENT;
  // dis_policy = DIS_GROUP;
  PE::steal_policy = POLICY_NO_STEAL;
  // SettingReport("1:2:4:8");
  // Run();

  multiplier_num = 128;
  int scale_naive4[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  InitSettings(scale_naive4);
  dis_policy = DIS_AVERAGE;
  // PE::steal_policy = POLICY_DO_STEAL;
  SettingReport("1:1:1:1");
  Run();

  multiplier_num = 128;
  int scale_schedule4_3[16] = {1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4, 1, 2, 3, 4};
  InitSettings(scale_schedule4_3);
  PE::sync_policy = SYNC_POLICY_ELEMENT;
  dis_policy = DIS_GROUP;
  PE::steal_policy = POLICY_NO_STEAL;
  SettingReport("1:2:3:4");
  Run();

  multiplier_num = 128;
  int scale_schedule4_2[16] = {2, 3, 4, 6, 2, 3, 4, 6, 2, 3, 4, 6, 2, 3, 4, 6};
  InitSettings(scale_schedule4_2);
  PE::sync_policy = SYNC_POLICY_ELEMENT;
  dis_policy = DIS_GROUP;
  PE::steal_policy = POLICY_NO_STEAL;
  SettingReport("2:3:4:6");
  Run();

  multiplier_num = 128;
  int scale_schedule4_4[16] = {3, 4, 4, 5, 3, 4, 4, 5, 3, 4, 4, 5, 3, 4, 4, 5};
  InitSettings(scale_schedule4_4);
  PE::sync_policy = SYNC_POLICY_ELEMENT;
  dis_policy = DIS_GROUP;
  PE::steal_policy = POLICY_NO_STEAL;
  SettingReport("3:4:4:5");
  Run();



  PE_num = 8;
  group_num = 2;
  PE::steal_policy = POLICY_NO_STEAL;
  PE::sync_policy = SYNC_POLICY_ELEMENT;

  multiplier_num = 128;
  int scale_naive2[8] = {1, 1, 1, 1, 1, 1, 1, 1};
  InitSettings(scale_naive2);
  dis_policy = DIS_AVERAGE;
  SettingReport("1:1");
  Run();

  multiplier_num = 128;
  int scale_schedule2_1[8] = {1, 2, 1, 2, 1, 2, 1, 2};
  InitSettings(scale_schedule2_1);
  // PE::sync_policy = SYNC_POLICY_ELEMENT;
  dis_policy = DIS_GROUP;
  // PE::steal_policy = POLICY_NO_STEAL;
  SettingReport("1:2");
  Run();

  multiplier_num = 128;
  int scale_schedule2_2[8] = {3, 4, 3, 4, 3, 4, 3, 4};
  InitSettings(scale_schedule2_2);
  // PE::sync_policy = SYNC_POLICY_ELEMENT;
  dis_policy = DIS_GROUP;
  // PE::steal_policy = POLICY_NO_STEAL;
  SettingReport("3:4");
  Run();

  multiplier_num = 128;
  int scale_schedule2_3[8] = {5, 6, 5, 6, 5, 6, 5, 6};
  InitSettings(scale_schedule2_3);
  // PE::sync_policy = SYNC_POLICY_ELEMENT;
  dis_policy = DIS_GROUP;
  // PE::steal_policy = POLICY_NO_STEAL;
  SettingReport("5:6");
  Run();


  PE_num = 32;
  group_num = 8;
  PE::steal_policy = POLICY_NO_STEAL;
  PE::sync_policy = SYNC_POLICY_ELEMENT;

  multiplier_num = 128;
  int scale_naive8[32] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
  InitSettings(scale_naive8);
  dis_policy = DIS_AVERAGE;
  SettingReport("1:1:1:1:1:1:1:1");
  Run();

  multiplier_num = 128;
  int scale_schedule8_1[32] = {1, 1, 2, 2, 3, 3, 4, 4, 1, 1, 2, 2, 3, 3, 4, 4, 1, 1, 2, 2, 3, 3, 4, 4, 1, 1, 2, 2, 3, 3, 4, 4};
  InitSettings(scale_schedule8_1);
  // PE::sync_policy = SYNC_POLICY_ELEMENT;
  dis_policy = DIS_GROUP;
  // PE::steal_policy = POLICY_NO_STEAL;
  SettingReport("1:1:2:2:3:3:4:4");
  Run();

  multiplier_num = 128;
  int scale_schedule8_2[32] = {2, 2, 3, 3, 4, 4, 6, 6, 2, 2, 3, 3, 4, 4, 6, 6, 2, 2, 3, 3, 4, 4, 6, 6, 2, 2, 3, 3, 4, 4, 6, 6};
  InitSettings(scale_schedule8_2);
  // PE::sync_policy = SYNC_POLICY_ELEMENT;
  dis_policy = DIS_GROUP;
  // PE::steal_policy = POLICY_NO_STEAL;
  SettingReport("2:2:3:3:4:4:6:6");
  Run();

  multiplier_num = 128;
  int scale_schedule8_3[32] = {3, 3, 4, 4, 4, 4, 5, 5, 3, 3, 4, 4, 4, 4, 5, 5, 3, 3, 4, 4, 4, 4, 5, 5, 3, 3, 4, 4, 4, 4, 5, 5};
  InitSettings(scale_schedule8_3);
  // PE::sync_policy = SYNC_POLICY_ELEMENT;
  dis_policy = DIS_GROUP;
  // PE::steal_policy = POLICY_NO_STEAL;
  SettingReport("3:3:4:4:4:4:5:5");
  Run();


  // for(float a = 0.4; a < 0.71; a += 0.1){
  //   for(float b = 0.6; b < 0.91; b += 0.1){
  //     GenerateInput(a, b);
  //     PE::steal_policy = POLICY_NO_STEAL;
  //     printf("\nA_sparsity = %f, B_sparsity = %f, doSteal = false\n", a, b);
  //     Run();
  //     PE::steal_policy = POLICY_DO_STEAL;
  //     printf("\nA_sparsity = %f, B_sparsity = %f, doSteal = true\n", a, b);
  //     Run();
  //   }
  // }

  CleanUp();
  return 0;
}
