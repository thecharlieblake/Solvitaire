
# Solvataire cheat-sheet for Windows users:
Install Docker, then

### Enter to environment:
Open PowerShell (as admin)  
cd path, for example:

    cd C:\Dev\Solvitaire\solvitaire1-master
    
    docker run -it -v ${PWD}:/home/Solvitaire --rm thecharlesblake/solvitaire:1.0

### Commands: 
Solve a random deal with seed 420:  

	./solvitaire --type klondike --random 420
	
Solves all the .json files in the examples folder:  

	./solvitaire --type klondike src/test/resources/klondike/*.json

Add a postfix command in order to send output to a text file: ( >, append >>)  
    
    >>output_g1.txt

For example:

	./solvitaire --type klondike --random 1 >>output_g1.txt

Read the last 11 rows of the file (those are the 11 parameters that the solver returns):

	tail -n 11 output_g1.txt

### Example of the output

	Solution Type: solved
	States Searched: 165
	Unique States Searched: 106
	Backtracks: 61
	Dominance Moves: 28
	States Removed From Cache: 0
	Final States In Cache: 78
	Final Buckets In Cache: 97
	Maximum Search Depth: 104
	Final Search Depth: 104
	Time Taken (milliseconds): 0
