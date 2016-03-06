/*
 * dataTypes.h
 *
 *  Created on: Feb 25, 2015
 *      Author: root
 */

#ifndef DATATYPES_H_
#define DATATYPES_H_

#define M 3 // M value for UUnifast Algorithm

// Typedefs used in the application
typedef enum Boolean_t {False, True} Boolean;
typedef enum Algorithm_t {RM, DM, LST} Algorithm;


// Structure representing a task
typedef struct Task_t
{
	float wcet;
	float deadline;
	float period;
	float priority;
} Task;

// Structure representing a taskset
typedef struct TaskSet_t
{
	int numTasks;
	Boolean dEqP;
	Algorithm algorithm;
	Task *tasks;
} TaskSet;

extern TaskSet genRandomTaskSet(int numTasks, float utilization, int deadlineOption);
extern void sortTaskSet(TaskSet *taskSet, Algorithm algorithm);

#endif /* DATATYPES_H_ */
