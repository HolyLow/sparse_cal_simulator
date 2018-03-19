#include "PE.h"

PE::PE(){
  task.clear();
  neighbor.clear();
  halt_cnt = 0;
  current_task = 0;
  task_sum = 0;
}

void PE::init(int iid, int all, int mul, int policy){
  id = iid;
  mul_num = mul;
  if(policy == POLICY_ALL_CONNECT){
    for(int i = 0; i < all; ++i){
      if(i == iid)
      continue;
      neighbor.push_back(i);
    }

  }
  else if(policy == POLICY_ONE_CONNECT){
    if(iid != all-1)
      neighbor.push_back(iid+1);
  }
  else
    error("Undefined Connection Policy");
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
  if(task_sum <= mul_num || current_task == task.size()-1 || halt_cnt > 0)
    return -1;
  int return_value = task.back();
  task_sum -= return_value;
  task.pop_back();
  return return_value;
}

bool PE::finished(){
  return task_sum == 0;
}

void PE::error(string msg){
  printf("PE Error: %s\n", msg.c_str());
  exit(1);
}
