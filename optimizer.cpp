/* Optimizer for the Navigation Point Location Optimization Problem

This class provides the needed functions for generating new location vectors and retrieving the minimum Average Trip Time for it.

@author Kay Wallaschek
*/


struct locations{
	int x,
	int y,
	bool finished
};

// Returns new Location pairs for the Simulator
// If there are no new locations pairs it sets bool finished in location structure.
locations* getNewLocations(int numNodes)
{
	// TODO
}


// Checks if the provided ATT is the current minimum for the specified number of nodes.
// If yes it saves its locations.
bool saveResults(locations* locs, int numNodes, int ATT)
{
	// TODO
}

void main(){

	for (int i = 1; i<5; ++i){
		while(true){
			/* 
			locations* locs = getNewLocations;
			if (!locs[0].finished){
				int ATT = simulate();
				saveResults(locs, i, ATT);
			}else {
				break;
			}
			*/
		}
	}

}