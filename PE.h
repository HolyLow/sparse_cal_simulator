#ifndef _PE_H_
 #define _PE_H_

#include <vector>
using namespace std;

class PE{
public:
  PE(){ task.clear(); neighbor.clear(); }
  void Init(int id, int all, int );

private:
  int id;
  int mul_num;
  int halt_cnt;
  vector<int> task;
  vector<int> neighbor;
};







#endif
