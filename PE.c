#include "PE.h"

PE::PE(){
  task.clear();
  neighbor.clear();
  halt_cnt = 0;
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
  task.push_back[t];
}
