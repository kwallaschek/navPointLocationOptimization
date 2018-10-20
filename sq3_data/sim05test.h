struct location{
	int x;
	int y;
	int cn;
	int np[44];
	bool finished;
};

float simulate(location* locations, int numNodes);
