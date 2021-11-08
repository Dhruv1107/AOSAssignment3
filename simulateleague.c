#include "premierleague.h"

/*
  Print the Schedule of all matches
*/
void printSchedule(int i, int schedule[i][2]){
	for(int row = 0; row<i; row++){
		for(int col =0; col<2; col++){
			printf("%d\t",schedule[row][col]);
		}
		printf("\n");	
	}
}

/*
  Initialize all the shared memory variables
*/
void initializeSharedMemory(int noOfTeams){

	//Create a shared teams Array of type team_info which stores all the info about particular team
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
	
	//Create a Critical Section Variable, It is used so that only one process can modify the CS at a time
	csKeyShmId = shmget(IPC_PRIVATE,sizeof(int), 0777|IPC_CREAT);
	cs_key = (int *) shmat(csKeyShmId,0,0);
	*cs_key = 1;
	
	//Create homeTeam and awayTeam, to store the respective teams for simulation
	homeTeam = cs_key +1;
	*homeTeam = 0;
	awayTeam = homeTeam + 1;
	*awayTeam = 0;
}

/*
  Checks if the match has an away fixture,
  If it has an away fixture well and good,
  If it doesn't have an away fixture the provided schedule is wrong
*/
int checkAwayFixture(int homeTeam, int awayTeam,int i, int schedule[i][2]){
	for(int row = 0; row<i; row++){
		if(schedule[row][0] == awayTeam && schedule[row][1] == homeTeam)
		return 1;	
	}
	return 0;
}

/*
  Checks if the given schedule is valid or not
*/
void checkScheduleValidity(int i, int noOfTeams, int schedule[i][2]){
	
	//Total number of matches should be n*(n-1), here n = no of teams
	if(i != noOfTeams*(noOfTeams - 1)) {
		printf("You are missing some matches. Check the schedule again\n");
		exit(4);
	}
	
	//Check if each and every match has an away fixture, if it doesn't exit throwing an error
	for(int row = 0; row<i; row++) {
		int homeTeam = schedule[row][0];
		int awayTeam = schedule[row][1];
		int awayFixturePresent = checkAwayFixture(homeTeam,awayTeam,i,schedule);
		if(!awayFixturePresent) {
			printf("Wrong Schedule, you need to have an away game with home team = %d and away team = %d\n",awayTeam,homeTeam);
			exit(3);
		}	
	}
}

/*
  Returns the process Id of manager process whose homeTeam = homeTeam1
*/
int getManagerPid(int homeTeam1,int index,int managerArray[index][2]) {
	for(int i=0;i<index;i++) {
		if(managerArray[i][1] == homeTeam1)
			return managerArray[i][0];
	}
}

/*
  Simulate a match between 2 teams
  Here hometeam is stored in shared memory variable of homeTeam
  and awayTeam is stored in shared memory variable of awayTeam
*/
void simulateMatch() {
	printf("Starting Match:Team %d vs Team %d\n",*homeTeam,*awayTeam);
	
	/*
	  gs1 = goals scored by home team
	  gs2 = goals scored by away team
	*/
	int gs1,gs2;
	srand(time(NULL));
	gs1 = rand() % 6;
	gs2 = rand() % 6;
	
	
	/*
	  If the match is not drawn, find the winner, loser, goals scored by
	  winning, losing team and update the shared memory with respective values
	*/
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
	
	/*
	 In case of a draw, update matchesDrawn, goalsScored, points
	 for both the teams
	*/
	else {
		teamsArray[*homeTeam].matchesDrawn++;
		teamsArray[*awayTeam].matchesDrawn++;
		teamsArray[*homeTeam].goalsScored += gs1;
		teamsArray[*awayTeam].goalsScored += gs1;
		teamsArray[*homeTeam].points++;
		teamsArray[*awayTeam].points++;
		teamsArray[*homeTeam].goalsScoredinCurrentMatch = gs1;
		teamsArray[*awayTeam].goalsScoredinCurrentMatch = gs1;
	}
	// Each match runs for 3 seconds
	sleep(3);
	//Update the result after 3 seconds
	printf("Match Ended:Team %d vs Team %d\t Result:%d-%d\n",*homeTeam,*awayTeam,teamsArray[*homeTeam].goalsScoredinCurrentMatch, teamsArray[*awayTeam].goalsScoredinCurrentMatch);
	//release the critical section so that other matches can be simulated
	*cs_key = 1;
}

/*
  Creates manager processes equal to the no of teams
  and the parent process sends a signal to the 
  respective manager if the game is played in his home ground
*/
void createStadiumManagerProcess(int noOfTeams,int i, int j, int schedule[i][2]){
	pid_t pids[noOfTeams];
	int managerArray[noOfTeams][2];
	int index = 0;
	
	//Create manager processes equal to number of teams
	for(int row = 1; row<=noOfTeams; row++){
	
		//If fork fails
		if ((pids[row] = fork()) < 0) {
			perror("fork");
			abort();
		} 
		//child
		else if (pids[row] == 0) {
			AGAIN:kill(getpid(), SIGSTOP);
			simulateMatch();
			goto AGAIN;
			
		}
		
		//Store child pid and teamId in an array
		managerArray[index][0] = pids[row];
		managerArray[index][1] = row;
		index++;
	}
	
	/*In the parent process try to read every match and send a signal to the
	respective manager process, so that the manager process can simulate the match*/
	for(int row = 0; row<=i; row++){
		int homeTeam1 = schedule[row][0];
		int awayTeam1 = schedule[row][1];
		int pid = getManagerPid(homeTeam1,index,managerArray);
		START:if(*cs_key == 1) {
			*cs_key = 0;
			*homeTeam = homeTeam1;
			*awayTeam = awayTeam1;
			kill(pid, SIGCONT);
		}
		else {
			usleep(500);
			goto START;
		}	
	}
	
	//When all matches are completed kill all manager processes
	for(int row = 0; row<=index; row++){
		kill(managerArray[row][0],SIGKILL);
	}
	//Wait for all the childs to be finished
	while(wait(NULL)!=-1);
}

/*
  Print the League Table, after completion of all matches
*/
void printLeagueTable(int noOfTeams){
	printf("\n\n");
	printf("Team\tW\tD\tL\tGS\tPoints\n");
	printf("--------------------------------------------------\n");
	team_info temp;
	
	//Sort the teams based on points in descending order
	for(int i = 1; i<=noOfTeams; i++){
		for(int j = i+1; j<= noOfTeams;j++){
			if(teamsArray[i].points < teamsArray[j].points){
				temp = teamsArray[i];
				teamsArray[i] = teamsArray[j];
				teamsArray[j] = temp;
			}
				
		}
	}
	
	//Sort the teams based on goalsScored if the teams have same points
	for(int i = 1; i<=noOfTeams; i++){
		for(int j = i+1; j<= noOfTeams;j++){
			if(teamsArray[i].points == teamsArray[j].points && teamsArray[i].goalsScored < teamsArray[j].goalsScored){
				temp = teamsArray[i];
				teamsArray[i] = teamsArray[j];
				teamsArray[j] = temp;
			}
				
		}
	}
	
	//Sort the teams based on index, if the team has same points, goalsScored
	for(int i = 1; i<=noOfTeams; i++){
		for(int j = i+1; j<= noOfTeams;j++){
			if(teamsArray[i].points == teamsArray[j].points && teamsArray[i].goalsScored == teamsArray[j].goalsScored && teamsArray[i].teamId > teamsArray[j].teamId ){
				temp = teamsArray[i];
				teamsArray[i] = teamsArray[j];
				teamsArray[j] = temp;
			}
				
		}
	}
	
	//Print the League table
	for(int i = 1;i<=noOfTeams; i++){
		printf("%d\t%d\t%d\t%d\t%d\t%d\n",teamsArray[i].teamId,teamsArray[i].matchesWon,teamsArray[i].matchesDrawn,teamsArray[i].matchesLost,teamsArray[i].goalsScored,teamsArray[i].points);
	}
	
}

/*
  Detaches the shared memory after simulating all the matches and
  after printing the league table
*/
void detachSharedMemory(){
	shmdt(teamsArray);
	shmdt(cs_key);
	shmdt(homeTeam);
	shmdt(awayTeam);
}

void simulateLeague(int noOfTeams,int i, int j, int schedule[][j]){
	initializeSharedMemory(noOfTeams);
	//printSchedule(i,schedule);
	checkScheduleValidity(i,noOfTeams,schedule);
	createStadiumManagerProcess(noOfTeams,i,j,schedule);
	printLeagueTable(noOfTeams);
	detachSharedMemory();

}
