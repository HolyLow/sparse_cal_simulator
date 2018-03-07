#include "PE.h"

PE::PE(){
  task.clear();
  neighbor.clear();
  halt_cnt = 0;
  current_task = 0;
  task_sum = 0;
}

void PE::init(int iid, int all, int mul){
  id = iid;
  mul_num = mul;
  for(int i = 0; i < all; ++i){
    if(i == iid)
      continue;
    neighbor.push_back(i);
  }
}

void PE::addTask(int t){
  task.push_back(t);
  task_sum += t;
}

void PE::calculate(){
  int mul_left = mul_num;
  int task_size = task.size();
  if(halt_cnt > 0){
    --halt_cnt;
    return;
  }
  while(mul_left > 0 && current_task < task_size){
    if(task[current_task] <= mul_left){
    mul_left -= task[current_task];
    task_sum -= task[current_task];
    task[current_task] = 0;
      ++current_task;
    }
    else{
      task[current_task] -= mul_left;
      task_sum -= mul_left;
      mul_left = 0;
    }
  }
}

void PE::steal(PE* head){
  if(task_sum == 0){
    int neighbor_size = neighbor.size();
    for(int i = 0; i < neighbor_size; ++i){
      int new_task = head[neighbor[i]].stolen();
      if(new_task != -1){
        task.push_back(new_task);
        task_sum += new_task;
        halt_cnt = 1;
        break;
      }
    }
  }
}

int PE::stolen(){
  if(task_sum <= mul_num || current_task == task.size()-1)
    return -1;
  int return_value = task.back();
  task_sum -= return_value;
  task.pop_back();
  return return_value;
}

bool PE::finished(){
  return task_sum == 0;
}
