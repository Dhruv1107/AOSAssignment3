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
	printf("Schedule is valid\n");
}

void simulateMatches(int stadiumManager, int i, int schedule[i][2]){
	for(int row = 0; row<i; row++){
		if(schedule[row][0] == stadiumManager){
				printf("Starting Match:Team %d vs Team %d\n",schedule[row][0],schedule[row][1]);
				/*int max = MAX(schedule[row][0],schedule[row][1]);
				int min = MIN(schedule[row][0],schedule[row][1]);
				int random = rand() % 2;
				int winner = random*(max - min) + min;
				min = 0;	//For draw
				random = rand() % 2;
				winner = random*(winner - min) + min;*/
				int numbers[3] = {0,schedule[row][0], schedule[row][1]};
				int index = rand() % 3;
				
				//printf("Winner:%d\n",winner);
				if(index != 0){
					int winner = numbers[index];
					int loser;
					if(winner == schedule[row][0]){
						loser = schedule[row][1];
					}
					else{
						loser = schedule[row][0];
					}
					teamsArray[winner].matchesWon++;
					int winnerGoals = rand() % 6;
					teamsArray[winner].goalsScoredinCurrentMatch = winnerGoals % 6;
					teamsArray[winner].goalsScored += winnerGoals;
					teamsArray[winner].points += 3;
					int loserGoals = (rand() % teamsArray[winner].goalsScored);
					teamsArray[loser].goalsScoredinCurrentMatch = loserGoals;
					teamsArray[loser].goalsScored += loserGoals;
					teamsArray[loser].matchesLost++;
				}
				else { //Draw
					teamsArray[schedule[row][0]].matchesDrawn++;
					teamsArray[schedule[row][1]].matchesDrawn++;
					int gs = rand() % 6;
					teamsArray[schedule[row][0]].goalsScored += gs;
					teamsArray[schedule[row][1]].goalsScored += gs;
					teamsArray[schedule[row][0]].points++;
					teamsArray[schedule[row][1]].points++;
					teamsArray[schedule[row][0]].goalsScoredinCurrentMatch = gs;
					teamsArray[schedule[row][1]].goalsScoredinCurrentMatch = gs;
				}
				sleep(1);
				printf("Match Ended:Team %d vs Team %d\t Result:%d-%d\n",schedule[row][0],schedule[row][1],teamsArray[schedule[row][0]].goalsScoredinCurrentMatch, teamsArray[schedule[row][1]].goalsScoredinCurrentMatch);
		}	
	}
}

void createStadiumManagerProcess(int noOfTeams,int i, int j, int schedule[][j]){
	pid_t pids[noOfTeams];
	for(int row = 0; row<=noOfTeams; row++){
		if ((pids[row] = fork()) < 0) {
			perror("fork");
			abort();
		} 
		else if (pids[row] == 0) {
			START:if(*cs_key == 1) {
				
				*cs_key = 0;
				//printf("cs_key value BEFORE %d for pid:%u\n",*cs_key,getpid());
				simulateMatches(row,i,schedule);
				*cs_key = 1;
				//printf("cs_key value AFTER %d for pid:%u\n",*cs_key,getpid());
				exit(0);
			
			}
			else{
				usleep(500);
				goto START;
			}
			
		}
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
	srand(time(NULL));
	initializeSharedMemory(noOfTeams);
	//printSchedule(i,schedule);
	checkScheduleValidity(i,schedule);
	createStadiumManagerProcess(noOfTeams,i,j,schedule);
	printLeagueTable(noOfTeams);

}
