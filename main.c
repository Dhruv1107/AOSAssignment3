#include "premierleague.h" 

/*
  Expected Command line arguments:
  1) Input file that is to be read
*/
int main(int argc, char **argv){
	char str[20],temp[20];
	char teams[20];
	int schedule[91][2];
	int noOfTeams;
	// token1 to store homeTeam and token2 to store awayTeam
	char *token1, *token2;
	
	if(argc != 2){
		printf("Execute it as: #executablename #inputfilename\n");
		exit(1);
	}
	
	FILE *f;
	f = fopen(argv[1], "r");
	
	// if there is an error, exit
	if(f == NULL){
		perror("Error opening file");
		return(-1); 
	}
	// if there is no error, read the first line which is no of teams
	else{ 
		fgets(teams, 20, f); // read from file
	}
	noOfTeams = atoi(teams);
	
	//If no of teams is greater than 10, exit by printing an error
	if(noOfTeams > 10){
		printf("No of teams should be less than or equal to 10\n");
		exit(2);
	}
	
	int i = 0;
	int j = 0;
	//Store the schedule of all matches in 2d Array schedule
	while (fgets(str,sizeof str,f) != NULL){
		token1 = strtok(str," ");
		token2 = strtok(NULL, " ");
		schedule[i][j] = atoi(token1);
		j++;
		schedule[i][j] = atoi(token2);
		i++;
		j=0;
		
	}
	//Simulate the League
	simulateLeague(noOfTeams,i,j, schedule);
	fclose(f);
	return 0;
}

