# AOSAssignment3
1) Name = Dhruv Bhatnagar<br />
   BITS Student ID No: 2021H1030047G<br /><br />
2) Files:<br />
	main.c = Driver file it is used to call other modules<br />
	simulateleague.c = Used to simulate all the matches in the league and to print the league table<br />
	premierleague.h = Interface file that contains method signatures and shared memory variables<br />
	makefile = creates linkable files and executables<br />
	input.txt = first line contains num of teams, and the remaining lines has the matches<br />
	<br />
3) Compilation = make<br /><br />

4) Execution = ./premierleague input.txt<br /><br />
	
5) Structure of the Program( the whole logic is in simulateleague function in simulateLeague.c)<br />
	a)	Create Shared Memory variables to store the details of every team<br />
	b)	Check if the given schedule provided in input.txt is valid or not<br />
	c)	Create manager processes for every team, the manager process simulates the matches that are taking place in the home ground<br />
	d)	After simulating all the matches, we print the league table<br />
	e)	At the end we detach the shared memory that we have created<br /><br />

6) Completed:<br />
	a) Schedule each match and notify the manager of the stadium on which the match is to be played<br />
	b) Randomly generating goals scored by each team and simulating the match<br />
	c) All the stadium manager processes are created before the matches start and each process is alive until all the matches to be played at the corresponding stadium are completed.<br />
	d) Printing the results of each and every match<br />
	e) Printing the league table after all the matches are completed<br />
   Not Completed: N/A<br /><br />
   
8) Bugs: In my short period of testing I couldn't find any bugs<br />


		
			

