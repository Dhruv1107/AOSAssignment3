#include "premierleague.h"
#include <stdio.h>
#include<stdlib.h>
#include <limits.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <stdbool.h>


void printSchedule(int i, int schedule[i][2]){
	for(int row = 0; row<i; row++){
		for(int col =0; col<2; col++){
			printf("%d\t",schedule[row][col]);
		}
		printf("\n");	
	}
}

void simulateLeague(int noOfTeams,int i, int j, int schedule[][j]){
	printSchedule(i,schedule);
}
