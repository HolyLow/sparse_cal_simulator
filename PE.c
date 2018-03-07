#include "PE.h"

void PE::init(int iid, int all, int mul){
  id = iid;
  mul_num = mul;
  for(int i = 0; i < all; ++i){
    if(i == iid)
      continue;
    neighbor.push_back(i);
  }
}
