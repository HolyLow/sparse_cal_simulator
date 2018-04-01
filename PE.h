#ifndef _PE_H_
 #define _PE_H_

#include <vector>
#include <stdlib.h>
#include <string>
#include <stdio.h>
using namespace std;

#define POLICY_ALL_CONNECT 0
#define POLICY_ONE_CONNECT 1

#define POLICY_NO_STEAL 0
#define POLICY_DO_STEAL 1

#define SYNC_POLICY_ELEMENT 0
#define SYNC_POLICY_LINE    1
class PE{
public:
  PE();
  void init(int iid, int mul, int con_policy = POLICY_ALL_CONNECT);
  void cleanup();
  void addTask(int t);
  void calculate();
  void wakeup();
  void steal();
  int stolen();
  bool finished();
  void report();
  static void static_init(PE* h, int pe_num);
  static int overall_task();
  static int overall_idel_clk();
  static int steal_policy;
  static int sync_policy;


private:
  int clk;
  int id;
  int mul_num;
  int halt_cnt;
  int idel_cnt;
  vector<int> task;
  int current_task;
  int task_sum;
  int task_overall;
  int task_cnt;
  vector<int> neighbor;
  bool sync_waiting;
  int steal_cnt;
  static PE* head;
  static int PE_num;
  static int waiting_cnt;
  static int PE_task_overall;
  static int PE_idel_cnt;

  static void error(string msg);
};









#endif
