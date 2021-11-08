#include "premierleague.h"
#define MAX(x,y) ((x>y)?x:y)
#define MIN(x,y) ((x<y)?x:y)

void printSchedule(int i, int schedule[i][2]){
	for(int row = 0; row<i; row++){
		for(int col =0; col<2; col++){
			printf("%d\t",schedule[row][col]);
		}
		printf("\n");	
	}
}

void initializeSharedMemory(int noOfTeams){
	shmid = shmget(IPC_PRIVATE,(noOfTeams+1)*sizeof(team_info), 0777|IPC_CREAT);
	teamsArray = (team_info *) shmat(shmid,0,0);
	for(int var1 = 1; var1<=noOfTeams; var1++){
		teamsArray[var1].teamId = var1;
		teamsArray[var1].matchesWon = 0;
		teamsArray[var1].matchesDrawn = 0;
		teamsArray[var1].matchesLost = 0;
		teamsArray[var1].goalsScored = 0;
		teamsArray[var1].goalsScoredinCurrentMatch = 0;
		teamsArray[var1].points = 0;
	}
	csKeyShmId = shmget(IPC_PRIVATE,sizeof(int), 0777|IPC_CREAT);
	cs_key = (int *) shmat(csKeyShmId,0,0);
	*cs_key = 1;
	homeTeam = cs_key +1;
	*homeTeam = 0;
	awayTeam = homeTeam + 1;
	*awayTeam = 0;
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
			printf("Wrong Schedule, you need to have an away game with home team = %d and away team = %d\n",awayTeam,homeTeam);
			exit(3);
		}	
	}
	//printf("Schedule is valid\n");
}

int getManagerPid(int hT,int index,int managerArray[index][2]) {
	for(int i=0;i<index;i++){
		if(managerArray[i][1] == hT)
			return managerArray[i][0];
	}
}
void simulateMatch() {
	printf("Starting Match:Team %d vs Team %d\n",*homeTeam,*awayTeam);
	
	int gs1,gs2;
	srand(time(NULL));
	gs1 = rand() % 6;
	gs2 = rand() % 6;
	//printf("GS1:%d\tGS2:%d\n",gs1,gs2);
	if(gs1 != gs2){
		int winner, loser, winnerGoals, loserGoals;
		if(gs1 > gs2){
			winner = *homeTeam;
			loser = *awayTeam;
			winnerGoals = gs1;
			loserGoals = gs2;
		}
		else{
			winner = *awayTeam;
			loser = *homeTeam;
			winnerGoals = gs2;
			loserGoals = gs1;
		}
		teamsArray[winner].matchesWon++;
		teamsArray[winner].goalsScoredinCurrentMatch = winnerGoals;
		teamsArray[winner].goalsScored += winnerGoals;
		teamsArray[winner].points += 3;
		teamsArray[loser].goalsScoredinCurrentMatch = loserGoals;
		teamsArray[loser].goalsScored += loserGoals;
		teamsArray[loser].matchesLost++;
	}
	else { //Draw
		teamsArray[*homeTeam].matchesDrawn++;
		teamsArray[*awayTeam].matchesDrawn++;
		teamsArray[*homeTeam].goalsScored += gs1;
		teamsArray[*awayTeam].goalsScored += gs1;
		teamsArray[*homeTeam].points++;
		teamsArray[*awayTeam].points++;
		teamsArray[*homeTeam].goalsScoredinCurrentMatch = gs1;
		teamsArray[*awayTeam].goalsScoredinCurrentMatch = gs1;
	}
	sleep(1);
	printf("Match Ended:Team %d vs Team %d\t Result:%d-%d\n",*homeTeam,*awayTeam,teamsArray[*homeTeam].goalsScoredinCurrentMatch, teamsArray[*awayTeam].goalsScoredinCurrentMatch);
	*cs_key = 1;
}

void createStadiumManagerProcess(int noOfTeams,int i, int j, int schedule[i][2]){
	pid_t pids[noOfTeams];
	int managerArray[noOfTeams][2];
	int index = 0;
	for(int row = 1; row<=noOfTeams; row++){
		
		if ((pids[row] = fork()) < 0) {
			perror("fork");
			abort();
		} 
		else if (pids[row] == 0) {
			AGAIN:kill(getpid(), SIGSTOP);
			simulateMatch();
			goto AGAIN;
			
		}
		managerArray[index][0] = pids[row];
		managerArray[index][1] = row;
		index++;
	}
	/*for(int row = 0; row<index; row++){
		for(int col =0; col<2; col++){
			printf("%d\t",managerArray[row][col]);
		}
		printf("\n");	
	}*/
	for(int row = 0; row<i; row++){
		int hT = schedule[row][0];
		int aT = schedule[row][1];
		int pid = getManagerPid(hT,index,managerArray);
		START:if(*cs_key == 1) {
			*cs_key = 0;
			*homeTeam = hT;
			*awayTeam = aT;
			kill(pid, SIGCONT);
		}
		else {
			usleep(500);
			goto START;
		}	
	}
	for(int row = 0; row<index; row++){
		kill(managerArray[row][0],SIGKILL);
	}
	while(wait(NULL)!=-1);
}

void printLeagueTable(int noOfTeams){
	printf("Team\tW\tD\tL\tGS\tPoints\n");
	printf("--------------------------------------------------------------\n");
	team_info temp;
	for(int i = 1; i<=noOfTeams; i++){
		for(int j = i+1; j<= noOfTeams;j++){
			if(teamsArray[i].points < teamsArray[j].points){
				temp = teamsArray[i];
				teamsArray[i] = teamsArray[j];
				teamsArray[j] = temp;
			}
				
		}
	}
	for(int i = 1; i<=noOfTeams; i++){
		for(int j = i+1; j<= noOfTeams;j++){
			if(teamsArray[i].points == teamsArray[j].points && teamsArray[i].goalsScored < teamsArray[j].goalsScored){
				temp = teamsArray[i];
				teamsArray[i] = teamsArray[j];
				teamsArray[j] = temp;
			}
				
		}
	}
	for(int i = 1; i<=noOfTeams; i++){
		for(int j = i+1; j<= noOfTeams;j++){
			if(teamsArray[i].points == teamsArray[j].points && teamsArray[i].goalsScored == teamsArray[j].goalsScored && teamsArray[i].teamId > teamsArray[j].teamId ){
				temp = teamsArray[i];
				teamsArray[i] = teamsArray[j];
				teamsArray[j] = temp;
			}
				
		}
	}
	for(int i = 1;i<=noOfTeams; i++){
		printf("%d\t%d\t%d\t%d\t%d\t%d\n",teamsArray[i].teamId,teamsArray[i].matchesWon,teamsArray[i].matchesDrawn,teamsArray[i].matchesLost,teamsArray[i].goalsScored,teamsArray[i].points);
	}
	
}

void simulateLeague(int noOfTeams,int i, int j, int schedule[][j]){
	
	initializeSharedMemory(noOfTeams);
	//printSchedule(i,schedule);
	checkScheduleValidity(i,schedule);
	createStadiumManagerProcess(noOfTeams,i,j,schedule);
	printLeagueTable(noOfTeams);

}
