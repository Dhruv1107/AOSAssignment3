#ifndef _PREMIERLEAGUE_HEADER
#define _PREMIERLEAGUE_HEADER

#include <stdio.h>
#include <stdlib.h>
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
#include <string.h>

typedef struct team
{
	int teamId;
	int matchesWon;
	int matchesDrawn;
	int matchesLost;
	int goalsScored;
	int goalsScoredinCurrentMatch;
	int points;
} team_info;
team_info *teamsArray;
int *cs_key,*homeTeam, *awayTeam;
int shmid,csKeyShmId;

/*
  Pre-condition: no of teams is le 10, 
  		 every match should have a return leg i.e if 1 2 is present 2 1 should also be present
  		 total number of matches = n*(n-1) here n= no of teams
  Post-condition: Iterative function which simulates all the matches by the respective manager process and 
  		   prints the league table after all the matches are completed
*/
extern void simulateLeague(int noOfTeams, int i, int j, int schedule[][j]);

#endif
