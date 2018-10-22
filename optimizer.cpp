/* Optimizer for the Navigation Point Location Optimization Problem

This class provides the needed functions for generating new location vectors and retrieving the minimum Average Trip Time for it.

@author Kay Wallaschek
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "stack.h"
#include "simpleLinkedList.h"
#include "sq3_data/sim05test.h"

int NUM_NODES = 3;

int minimalATT = 9999999;
int minimalCoordinates = 00;


// SOURCE: https://stackoverflow.com/questions/515612/convert-an-integer-number-into-an-array
int * convertOctettToArray(int number){
	int * arr = new int[NUM_NODES*2];
    int i = 0;
    do {
        arr[i] = number % 8;
        number /= 8;
        i++;
    } while (number != 0);
    return arr;
}

bool isThereAZeroInTheArray(int* array){
	
	for (int i = 0; i < NUM_NODES*2; i++){
		if (array[i] == 0){
			return true;
		}
	}
	return false;
}

bool positionsValid(int* array){
	
	int i = NUM_NODES*2-1;
	while (i>=0){
		if (array[i]%2==0){
			i--;
			if (array[i]%2==0){
				/*printf("Invalid: ");
				for(int j = NUM_NODES*2-1; j>=0; j--){
					printf("%o", array[j]);
				}
				printf("\n");
				*/return false;
			} else {
				i--;
			}
		} else {
			i-=2;
		}
		/*printf("Valid: ");
				for(int j = NUM_NODES*2-1; j>=0; j--){
					printf("%o", array[j]);
				}
				printf("\n");
		*/
	}

	return true;
}

bool checkForDuplicateSingleLocations(int* locs){
	
	NODE* head;
	NODE* node;
	DATA loc_node;
	init(&head);
	for (int i = 0; i < NUM_NODES*2; i+=2) {
		loc_node.x = locs[i];
		loc_node.y = locs[i+1];
		if (!find(head,loc_node)){
			head = add(head, loc_node);
		} else {
			/*printf("Invalid: ");
				for(int j = NUM_NODES*2-1; j>=0; j--){
					printf("%o", locs[j]);
				}
				printf("\n");
			*/
			return true;
		}
	}
	return false;
}


location * transformArrayToLocations(int* array){
	
	int i = NUM_NODES*2-1;
	int j = 0;
	location locs[44];

	NODE* head;
	NODE* node;
	DATA loc_node;
	init(&head);
	// Fill list with Locations
	while (i>=0){
		loc_node.x = array[i-1];
		loc_node.y = array[i];
		head = add(head, loc_node);
		//location loc = {array[i],array[i-1],4, {12,23,29,31}};
		//location loc = {array[i],array[i-1]};
		//locs[j] = loc;
		i-=2;
		j++;
	}
	location tmp = {0,0,0};
	locs[0] = tmp;
	location tmp2 = {8,0,0};
	locs[1] = tmp2;
	location tmp3 = {8,8,0};
	locs[2] = tmp3;
	location tmp4 = {0,8,0};
	locs[3] = tmp4;

	int counter = 4;
	//printf("2\n");
	for (int i = 1; i<8; i++){
		for (int j = 1; j<8; j++){
			if (!(i%2 == 0 && j%2 == 0)){


			//printf("%d %d %d \n", i,j,counter);
			loc_node.x = j;
			loc_node.y = i;
			if (find(head,loc_node)){
				location tmp = {j,i,4,{12,23,29,31}};
				locs[counter] = tmp;
			} else {
				location tmp = {j,i,0};
				locs[counter] = tmp;
			}
			counter++;
		}
		}
	}
	/*for (int i = 0; i<44; i++){
		printf("%d %d %d\n", locs[i].x,locs[i].y,locs[i].cn);
	}*/
	return locs;
}

bool isArrayValid(int* array){
	
	if (!isThereAZeroInTheArray(array) 
			&& positionsValid(array)
			&& !checkForDuplicateSingleLocations(array)){
		 return true;
	}
	return false;
}

bool saveResults(float att, int locs) {
	if (att<minimalATT){
		minimalATT = att;
		minimalCoordinates = locs;
		return true;
	}
	return false;
}

int main (int argc, char *argv[]) {

	if (argv[1] != NULL){
		NUM_NODES = atoi(argv[1]);
	}

	struct Stack* stack = createStack(pow(64,NUM_NODES));

	printf("Calculating Cases ...\n");
	for (int i = 00; i<pow(8,NUM_NODES*2+1); i++){
		push(stack,i);
	}

	struct Stack* stackOfLocations = createStack(pow(64,NUM_NODES));
	
	int size = 0;
	while (!isEmpty(stack)){
		int x = pop(stack);
		
		int * array = convertOctettToArray(x);
		if (isArrayValid(array)){
			size++;
			push(stackOfLocations, x);
		}
	}

	int counter = 1;

	while (!isEmpty(stackOfLocations)){
		int x = pop(stackOfLocations);
		fflush(stdout);
		printf("\r%c[2K Simulating %d of %d... Current minimal ATT: %d s Testing: %o",27,counter,size, minimalATT,x);
		fflush(stdout);
		location * s = transformArrayToLocations(convertOctettToArray(x));
		float att = simulate(s,NUM_NODES);
	
		saveResults(att, x);
		counter++;
	}
	printf("\n Finished \n");
	printf("Best Configuration: %o\n", minimalCoordinates);
	printf("ATT: %d\n", minimalATT);

	/*while (!isEmpty(stackOfLocations)){
		printf("%o\n", pop(stackOfLocations));
	}*/
	return(0);
} 