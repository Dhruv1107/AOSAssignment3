#ifndef _PREMIERLEAGUE_HEADER
#define _PREMIERLEAGUE_HEADER

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

typedef struct team
{
	int teamId;
	int matchesWon;
	int matchesDrawn;
	int matchesLost;
	int goalsScored;
	int points;
} team_info;
team_info *teamsArray;
int shmid;

extern void simulateLeague(int noOfTeams, int i, int j, int schedule[][j]);

#endif
