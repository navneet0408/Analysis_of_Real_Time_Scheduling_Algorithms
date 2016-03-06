/*
 * schedAnalysis.cpp
 *
 *  Created on: Feb 22, 2015
 *      Author: navneet
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "dataTypes.h"

// Function to parse the input file
void parseFile(char *fileName, TaskSet **retTaskSet, int *retNumTaskSets)
{
	FILE *file;
	TaskSet *taskSet;
	char buffer;
	int numTaskSets;
	int taskSetIndex;
	int taskIndex;
	file = fopen(fileName, "r");
	if(file == 0)
	{
		printf("File %s open failed!!!\n", fileName);
		exit(1);
	}

	fscanf(file, "%d%c", &numTaskSets, &buffer);

	taskSet = (TaskSet *) malloc(numTaskSets * sizeof(TaskSet));

	for(taskSetIndex = 0; taskSetIndex < numTaskSets; ++taskSetIndex)
	{
		fscanf(file, "%d%c", &taskSet[taskSetIndex].numTasks, &buffer);
		taskSet[taskSetIndex].tasks = (Task *) malloc(taskSet[taskSetIndex].numTasks * sizeof(Task));
		taskSet[taskSetIndex].dEqP = True;
		for(taskIndex = 0; taskIndex<taskSet[taskSetIndex].numTasks; ++taskIndex)
		{
			fscanf(file, "%f%c", &taskSet[taskSetIndex].tasks[taskIndex].wcet, &buffer);
			fscanf(file, "%f%c", &taskSet[taskSetIndex].tasks[taskIndex].deadline, &buffer);
			fscanf(file, "%f%c", &taskSet[taskSetIndex].tasks[taskIndex].period, &buffer);
			if(taskSet[taskSetIndex].tasks[taskIndex].deadline !=
					taskSet[taskSetIndex].tasks[taskIndex].period)
			{
				taskSet[taskSetIndex].dEqP = False;
			}
		}
	}

	(*retTaskSet) = taskSet;
	(*retNumTaskSets) = numTaskSets;

}

// Function for analyzing the schedulability of a taskset.
// The function returns a Boolean value, informing if the taskset is schedulable or not.
// If the taskset is not schedulable, it returns the task number of the first non-schedulable task
// 	and its response time, int the variables pointed by pointers "tasknum" and "responseTime"
// If "notify" is true, the function prints the detailed results of the analysis for every task.
Boolean analysis(TaskSet taskSet, int *taskNum, float *responseTime, Boolean notify)
{
	float u=0, uMin=0;
	for(int i=0; i<taskSet.numTasks; ++i)
	{
		uMin = (float) (i+1)*(pow(2, (1.0/(i+1)))-1);
		u = u + taskSet.tasks[i].wcet/taskSet.tasks[i].deadline;

		if((u>1) && (taskSet.dEqP == True))
		{
			if(notify == True)
			{
				printf("\nTask %d failed UB test", i);
				printf("\nTotal Utilization for task %d: %f", i, u);
				printf("\nMax Utilization if all Di== Pi: 1");
				printf("\nTaskSet not schedulable");
			}
			return False;
		}

		if(u>uMin)
		{
			float oldRt = taskSet.tasks[i].wcet, rt=0;
			while(oldRt != rt)
			{
				oldRt=rt;
				rt = taskSet.tasks[i].wcet;
				for(int j=0; j<i; ++j)
				{
					rt = rt + taskSet.tasks[j].wcet*ceil(oldRt/taskSet.tasks[j].period);
				}
			}
			if(notify == True)
			{
				printf("\nTask %d inconclusive UB test", i);
				printf("\nUtilization/Density: %f", u);
				printf("\nUtilization bound: %f", uMin);
				printf("\nDoing RT test for task %d", i);
				printf("\nRT: %f", rt);
				printf("\nDeadline: %f", taskSet.tasks[i].deadline);
			}


			if(rt > taskSet.tasks[i].deadline)
			{
				(*taskNum) = i;
				(*responseTime) = rt;
				return False;
			}
		}
		else
		{
			if(notify == True)
			{
				printf("\nTask %d passed UB test", i);
				printf("\nUtilization/Density: %f", u);
				printf("\nUtilization bound: %f", uMin);
			}
		}
	}
	return True;
}


// function to print usage directions, if proper arguments are not given.
void usage(char *s)
{
	printf("\nEnter first argument as 1 to read input from file");
	printf("\nEnter first argument as 2 to use random TaskSets");
	printf("\nUsage:\n%s 1 <input_file_name>\nor\n", s);
	printf("%s 2 <number_of_tasks_in_taskset> <Deadline_Option>", s);
	printf("\nDeadline option = 1 for deadline distribution of [Ci, Ti]");
	printf("\nDeadline option = 2 for deadline distribution of [Ci + (Ti-Ci)/2, Ti]");
}


// Function to print the detalis of a taskset
void printTaskSet(TaskSet taskSet)
{
	int j;
	float sum=0;
	printf("\nNumber of Tasks in Set: %d\n", taskSet.numTasks);
	printf("Deadline == Period ?? %s\n", (taskSet.dEqP == True) ? "True" : "False");
	for(j=0; j<taskSet.numTasks; ++j)
	{
		printf("%f %f %f\n", taskSet.tasks[j].wcet, taskSet.tasks[j].deadline,
				taskSet.tasks[j].period);
		//sum += taskSet.tasks[j].wcet/taskSet.tasks[j].deadline;
		sum += (taskSet.tasks[j].deadline - taskSet.tasks[j].wcet)/(taskSet.tasks[j].period - taskSet.tasks[j].wcet);

	}
	printf("Sum: %f\n", sum/taskSet.numTasks);
}

// Start of the main()
int main(int argc, char **argv)
{
	int i;
	float u;
	if(argc!=3 && argc !=4)
	{
		usage(argv[0]);
		exit(0);
	}

	int option = atoi(argv[1]);
	TaskSet *taskSet;
	int numTaskSets;
	int nonSchedulableTaskNo;
	float nonSchedulableTaskRT;

	if(option == 1) // Read input taskset from file
	{
		parseFile(argv[2], &taskSet, &numTaskSets);
		for(i=0; i<numTaskSets; ++i)
		{
			printf("\n\n*****Analyzing TaskSet %d :******\n", i);
			printf("\nRM Analysis");
			sortTaskSet(&taskSet[i], RM);
			if(analysis(taskSet[i], &nonSchedulableTaskNo, &nonSchedulableTaskRT, True) == True)
			{
				printf("\nTaskSet %d : Schedulable....", i);
			}
			else
			{
				printf("\nTaskSet %d: Not Schedulable", i );
				printf("\nTask %d Response Time: %f", nonSchedulableTaskNo, nonSchedulableTaskRT);
				printf("\nTask %d Deadline: %f", nonSchedulableTaskNo,
						taskSet[i].tasks[nonSchedulableTaskNo].deadline);
			}

			printf("\nDM Analysis");
			sortTaskSet(&taskSet[i], DM);
			if(analysis(taskSet[i], &nonSchedulableTaskNo, &nonSchedulableTaskRT, True) == True)
			{
				printf("\nTaskSet %d : Schedulable....", i);
			}
			else
			{
				printf("\nTaskSet %d: Not Schedulable", i );
				printf("\nTask %d Response Time: %f", nonSchedulableTaskNo, nonSchedulableTaskRT);
				printf("\nTask %d Deadline: %f", nonSchedulableTaskNo,
						taskSet[i].tasks[nonSchedulableTaskNo].deadline);
			}
			printf("\nLST Analysis");
			sortTaskSet(&taskSet[i], LST);
			if(analysis(taskSet[i], &nonSchedulableTaskNo, &nonSchedulableTaskRT, True) == True)
			{
				printf("\nTaskSet %d : Schedulable....", i);
			}
			else
			{
				printf("\nTaskSet %d: Not Schedulable", i );
				printf("\nTask %d Response Time: %f", nonSchedulableTaskNo, nonSchedulableTaskRT);
				printf("\nTask %d Deadline: %f", nonSchedulableTaskNo,
						taskSet[i].tasks[nonSchedulableTaskNo].deadline);
			}
		}
	}
	else // Generate random tasks
	{
		int numRM, numDM, numLST;
		int numTasks = atoi(argv[2]);
		int deadlineOption = atoi(argv[3]);
		taskSet = (TaskSet *) malloc(10000*sizeof(TaskSet));
//		taskSet[0] = genRandomTaskSet(numTasks, .5, deadlineOption);
//		sortTaskSet(&taskSet[0], RM);
//		analysis(taskSet[0], &nonSchedulableTaskNo, &nonSchedulableTaskRT, True );
//		printTaskSet(taskSet[0]);
		for(u=0.05; u<1; u+=.1)
		{
			numRM=0, numDM=0, numLST=0;
			for(i=0; i<10000; ++i)
			{
				taskSet[i] = genRandomTaskSet(numTasks, u, deadlineOption);
			}

			for(i=0; i<10000; ++i)
			{
				sortTaskSet(&taskSet[i], RM); // Sort tasks in taskset based on algorithm
				if(analysis(taskSet[i], &nonSchedulableTaskNo, &nonSchedulableTaskRT, False) == True)
				{
					numRM++;
				}
			}

			for(i=0; i<10000; ++i)
			{
				sortTaskSet(&taskSet[i], DM);
				if(analysis(taskSet[i], &nonSchedulableTaskNo, &nonSchedulableTaskRT, False) == True)
				{
					numDM++;
				}
			}

			for(i=0; i<10000; ++i)
			{
				sortTaskSet(&taskSet[i], LST);
				if(analysis(taskSet[i], &nonSchedulableTaskNo, &nonSchedulableTaskRT, False) == True)
				{
					numLST++;
				}
			}
			printf("\nUtilization: %f RM: %f DM: %f LST: %f", u, numRM/10000.0, numDM/10000.0
					, numLST/10000.0);
		}
	}


	// Free all dynamically allocated memories
	for(i=0; i<numTaskSets; ++i)
	{
		free(taskSet[i].tasks);
	}
	free(taskSet);
	printf("Hello\n");
	return 0;
}
