#include "PE.h"

int PE::steal_policy = POLICY_DO_STEAL;
int PE::sync_policy = SYNC_POLICY_LINE;
int PE::waiting_cnt = 0;
int PE::PE_num = 0;
PE* PE::head = NULL;

PE::PE(){
  task.clear();
  neighbor.clear();
  halt_cnt = 0;
  current_task = 0;
  task_sum = 0;
  idel_cnt = 0;
  sync_waiting = false;
  steal_cnt = 0;
}

void PE::init(int iid, int mul, int con_policy){
  id = iid;
  mul_num = mul;
  if(con_policy == POLICY_ALL_CONNECT){
    for(int i = 0; i < PE_num; ++i){
      if(i == iid)
      continue;
      neighbor.push_back(i);
    }
  }
  else if(con_policy == POLICY_ONE_CONNECT){
    if(iid != PE_num-1)
      neighbor.push_back(iid+1);
  }
  else
    error("Undefined Connection Policy");
}

void PE::cleanup(){
  task.clear();
  halt_cnt = 0;
  current_task = 0;
  task_sum = 0;
  idel_cnt = 0;
  sync_waiting = false;
  steal_cnt = 0;
}

void PE::addTask(int t){
  task.push_back(t);
  task_sum += t;
}

void PE::calculate(){
  if(halt_cnt > 0){
    --halt_cnt;
    ++idel_cnt;
    return;
  }
  if(sync_policy == SYNC_POLICY_ELEMENT){
    if(sync_waiting){
      ++idel_cnt;
      return;
    }
  }
  if(task_sum == 0){
    ++idel_cnt;
    return;
  }
  int mul_left = mul_num;
  int task_size = task.size();
  while(mul_left > 0 && current_task < task_size){
    if(task[current_task] <= mul_left){
      mul_left -= task[current_task];
      task_sum -= task[current_task];
      task[current_task] = 0;
      ++current_task;
      if(sync_policy == SYNC_POLICY_ELEMENT){
        ++waiting_cnt;
        sync_waiting = true;
        if(waiting_cnt == PE_num){
          for(int i = 0; i < PE_num; ++i)
            head[i].wakeup();
        }
      }
    }
    else{
      task[current_task] -= mul_left;
      task_sum -= mul_left;
      mul_left = 0;
    }
  }
}

void PE::wakeup(){
  sync_waiting = false;
  --waiting_cnt;
}

void PE::steal(){
  if(steal_policy == POLICY_NO_STEAL)
    return;
  if(task_sum == 0){
    int neighbor_size = neighbor.size();
    for(int i = 0; i < neighbor_size; ++i){
      int new_task = head[neighbor[i]].stolen();
      if(new_task != -1){
        task.push_back(new_task);
        task_sum += new_task;
        halt_cnt = 1;
        steal_cnt++;
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
  if(task_sum == 0){
    task.clear();
    current_task = 0;
    return true;
  }
  else return false;
}

void PE::report(){
  printf("PE id = %d, steal_cnt = %d, idel_cnt = %d\n", id, steal_cnt, idel_cnt);
  // printf("//////task_sum = %d, steal_policy = %d\n", task_sum, steal_policy);
}

void PE::static_init(PE* h, int pe_num){
  head = h;
  PE_num = pe_num;
  waiting_cnt = 0;
  // steal_policy = POLICY_NO_STEAL;
  // sync_policy = SYNC_POLICY_LINE;
}

void PE::error(string msg){
  printf("PE Error: %s\n", msg.c_str());
  exit(1);
}
