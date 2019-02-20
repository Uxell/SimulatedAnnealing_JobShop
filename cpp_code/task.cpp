#include "task.h"

Task::Task() {}

void Task::setMachineId(int machine_id)
{
	this->machine_id = machine_id;
}

void Task::setDuration(int duration)
{
	this->duration = duration;
}

int Task::getMachineId()
{
	return this->machine_id;
}

int Task::getDuration()
{
	return this->duration;
}