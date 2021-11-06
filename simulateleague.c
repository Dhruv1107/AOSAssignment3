#include "premierleague.h"

void printSchedule(int i, int schedule[i][2]){
	for(int row = 0; row<i; row++){
		for(int col =0; col<2; col++){
			printf("%d\t",schedule[row][col]);
		}
		printf("\n");	
	}
}

void initializeSharedMemory(int noOfTeams){
	shmid = shmget(IPC_PRIVATE,noOfTeams*sizeof(team_info), 0777|IPC_CREAT);
	teamsArray = (team_info *) shmat(shmid,0,0);
	for(int var1 = 0; var1<noOfTeams; var1++){
		teamsArray[var1].teamId = var1;
		teamsArray[var1].matchesWon = 0;
		teamsArray[var1].matchesDrawn = 0;
		teamsArray[var1].matchesLost = 0;
		teamsArray[var1].goalsScored = 0;
		teamsArray[var1].points = 0;
	}
}

int checkAwayFixture(int homeTeam, int awayTeam,int i, int schedule[i][2]){
	for(int row = 0; row<i; row++){
		if(schedule[row][0] == awayTeam && schedule[row][1] == homeTeam)
		return 1;	
	}
	return 0;
}

void checkScheduleValidity(int i, int schedule[i][2]){
	for(int row = 0; row<i; row++){
		int homeTeam = schedule[row][0];
		int awayTeam = schedule[row][1];
		int awayFixturePresent = checkAwayFixture(homeTeam,awayTeam,i,schedule);
		if(!awayFixturePresent){
			printf("Wrong Schedule, you need to have an away game\n");
			exit(3);
		}	
	}
}

void simulateLeague(int noOfTeams,int i, int j, int schedule[][j]){
	initializeSharedMemory(noOfTeams);
	printSchedule(i,schedule);
	checkScheduleValidity(i,schedule);
}
