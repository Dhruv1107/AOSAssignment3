#include "premierleague.h"
#include <stdio.h>
#include <stdlib.h>
#include<string.h>  

int main(int argc, char **argv){
	char str[20],temp[20];
	char teams[20];
	int schedule[90][2];
	int noOfTeams;
	char *token1, *token2;
	
	FILE *f;
	f = fopen(argv[1], "r");
	
	// if there was an error
	if(f == NULL){
		perror("Error opening file"); // print error
		return(-1); 
	}
	// if there was no error
	else{ 
		fgets(teams, 20, f); // read from file
	}
	noOfTeams = atoi(teams);
	printf("No of Teams:%d\n",noOfTeams);
	
	int i = 0;
	int j = 0;
	while (fgets(str,sizeof str,f) != NULL){
		token1 = strtok(str," ");
		token2 = strtok(NULL, " ");
		schedule[i][j] = atoi(token1);
		j++;
		schedule[i][j] = atoi(token2);
		i++;
		j=0;
		
	}
	simulateLeague(noOfTeams,i,j, schedule);
	fclose(f); // close file
	return 0;
}

