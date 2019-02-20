#ifndef TASK_H
#define TASK_H

class Task
{
private:
  int machine_id; //id of the machine on which the task has to be processed
  int duration;   //processing duration of the task
public:
  void setMachineId(int machine_id);
  void setDuration(int duration);

  int getMachineId();
  int getDuration();

  Task();
};

#endif