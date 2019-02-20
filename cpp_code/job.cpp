#include "job.h"

Job::Job()
{
}

Job::~Job()
{
    for (int i = 0; i < tasks.size(); i++)
        delete tasks[i];
    this->tasks.clear();
}

void Job::addTask(Task *task)
{
    this->tasks.push_back(task);    
}

Task *Job::getTask(int task_id)
{
    return this->tasks[task_id];
}

int Job::getTaskCount()
{
    return this->tasks.size();
}
