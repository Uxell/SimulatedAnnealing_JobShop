#ifndef JOB_H
#define JOB_H
#include <vector>
#include "task.h"

class Job
{
private:
  std::vector<Task *> tasks;

public:
  void addTask(Task *Task);   //adds Task on the end of tasks vector of the job
  Task *getTask(int task_id); //returns Task with given id
  int getTaskCount();

  Job();
  ~Job();
};

#endif