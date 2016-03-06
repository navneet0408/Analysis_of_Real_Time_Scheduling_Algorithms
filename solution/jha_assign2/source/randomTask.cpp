/*
 * randomTask.cpp
 *
 *  Created on: Feb 25, 2015
 *      Author: root
 */


#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "dataTypes.h"

// Function to generate random numbers in a given range
double rand_range(double min, double max)
{
	return (min + rand() * (max-min)/ (RAND_MAX));
}

// Function to sort the tasks in a taskset based on their priorities.
// The priorities are assigned by the specific algorithm.
void sortTaskSet(TaskSet *taskSet, Algorithm algorithm)
{
	int i,j, min;
	Task temp;

	for(i=0; i<taskSet->numTasks; ++i)
	{
		switch(algorithm)
		{
		case RM:
			taskSet->tasks[i].priority = taskSet->tasks[i].period;
			break;
		case DM:
			taskSet->tasks[i].priority = taskSet->tasks[i].deadline;
			break;
		case LST:
			taskSet->tasks[i].priority = taskSet->tasks[i].deadline - taskSet->tasks[i].wcet;
			break;
		}
	}

	for(i=0; i<taskSet->numTasks; ++i)
	{
		min = i;
		for(j=i+1; j<taskSet->numTasks; ++j)
		{
			if(taskSet->tasks[min].priority > taskSet->tasks[j].priority)
			{
				min = j;
			}
		}
		temp = taskSet->tasks[min];
		taskSet->tasks[min] = taskSet->tasks[i];
		taskSet->tasks[i] = temp;
	}
}

// Function to generate a random task set
TaskSet genRandomTaskSet(int numTasks, float utilization, int deadlineOption)
{
	TaskSet taskSet;
	int i;
	float w,p;
	float sumUtil = utilization, nextSumUtil, period;

	taskSet.numTasks = numTasks;
	taskSet.dEqP = False;
	taskSet.tasks = (Task *) malloc(numTasks * sizeof(Task));
	for(i=0; i<(numTasks); ++i)
	{
		period  = rand_range(pow(10,(i*M/numTasks+1)), pow(10,(i*M/numTasks+2)));
		taskSet.tasks[i].period = period;
		nextSumUtil = sumUtil * pow((float) rand()/(RAND_MAX), (float) (1.0/(numTasks-i)));
		if(i!=(numTasks-1))
		{
			taskSet.tasks[i].wcet = period * (sumUtil - nextSumUtil);
		}
		else
		{
			taskSet.tasks[i].wcet = period * sumUtil;
		}
		sumUtil = nextSumUtil;

		w = taskSet.tasks[i].wcet;
		p = taskSet.tasks[i].period;
		if(deadlineOption ==1)
		{
			taskSet.tasks[i].deadline = rand_range(w, p);
		}
		else
		{
			taskSet.tasks[i].deadline = rand_range(w + (p-w)/2.0, p);
		}



	}


	return taskSet;
}


