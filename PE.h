#ifndef _PE_H_
 #define _PE_H_

#include <vector>
using namespace std;

class PE{
public:
  PE();
  void init(int iid, int all, int mul);
  void addTask(int t);
  void calculate();
  void steal(PE* head);
  int stolen();
  bool finished();

private:
  int id;
  int mul_num;
  int halt_cnt;
  vector<int> task;
  int current_task;
  int task_sum;
  vector<int> neighbor;
};







#endif
