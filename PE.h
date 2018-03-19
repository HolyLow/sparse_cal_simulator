#ifndef _PE_H_
 #define _PE_H_

#include <vector>
#include <stdlib.h>
#include <string>
#include <stdio.h>
using namespace std;

#define POLICY_ALL_CONNECT 0
#define POLICY_ONE_CONNECT 1
class PE{
public:
  PE();
  void init(int iid, int all, int mul, int policy = POLICY_ALL_CONNECT);
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
  static void error(string msg);
};







#endif
