/* Optimizer for the Navigation Point Location Optimization Problem

This class provides the needed functions for generating new location vectors and retrieving the minimum Average Trip Time for it.

@author Kay Wallaschek
*/


#include <stdio.h>
#include <math.h>
#include "stack.h"
#include "simpleLinkedList.h"
#include "sim05test.h"

int NUM_NODES = 3;

int minimalATT = 9999999;
int minimalCoordinates = 00;

struct Location{
	int x;
	int y;
};

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
	location* locs;
	while (i>=0){
		
		location loc = {array[i],array[i-1]};
		
		locs[j] = loc;
		
		i-=2;
		j++;
	}
	
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

int main () {

	struct Stack* stack = createStack(pow(64,NUM_NODES));


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
		printf("\r%c[2K Simulating %d of %d...",27,counter,size);
		float att = simulate(transformArrayToLocations(convertOctettToArray(x)),NUM_NODES);
	
		saveResults(0.0, x);
		counter++;
	}
	printf("\n Finished \n");
	/*while (!isEmpty(stackOfLocations)){
		printf("%o\n", pop(stackOfLocations));
	}*/
	return(0);
} 