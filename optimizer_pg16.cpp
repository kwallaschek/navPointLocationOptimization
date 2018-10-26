/* Optimizer for the Navigation Point Location Optimization Problem

This class provides the needed functions for generating new location vectors and retrieving the minimum Average Trip Time for it.

@author Kay Wallaschek
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>
#include "stack.h"
#include "simpleLinkedList.h"
#include "sq3_data/pg16sim01.h"

#define NUM_LINKS 105 // number of links + 1

int NUM_NODES = 3;  // !NEED CHANGE
int NUM_POSSIBILITIES = 0;
float minimalATT = 9999999;

//!NEED CHANGE Add more or delete.
int min_x = 0;
int min_y = 0;
int min_z = 0;





int main (int argc, char *argv[]) {
	clock_t start, end;
	double cpu_time_used;
	int num_threads = 1;
	start = clock();
	if (argv[1] != NULL){
		num_threads = atoi(argv[1]);
	}

	int size = 0;

	NODE* head;
	NODE* node;
	DATA data_node;
	init(&head);

	// !NEED CHANGE Add or remove loops according to number of nodes
	for (int i = 0; i<NUM_LINKS-1; i++){
		for (int j = 0; j<NUM_LINKS-1; j++){
			for (int k = 0; k<NUM_LINKS-1; k++){
				// Need to check for duplicates
				if(i != j && i != k && k != j){
					data_node.x = i;
					data_node.y = j;
					data_node.z = k;
					// Here it could be checked if some other order of the same locations are already in the list
					// Currently not implemented: Need support in simpleLinkedList.cpp
					head = add(head, data_node);
					size++;
				}
			}
		}
	}
	/*
		// Example for 1 node only
		for (int i = 0; i<NUM_LINKS-1; i++){
		
					data_node.x = i;
					data_node.y = 0;
					data_node.z = 0;
					head = add(head, data_node);
					size++;
				
			
	}*/
	
	NODE * p = head;

	// init Array and allocate space for it.
	int** cases;
  	cases = (int**) malloc(size*sizeof(int*));
  		for (int i = 0; i < size; i++)
  				cases[i] = (int*) malloc(NUM_NODES+1*sizeof(int));

	int i = 0;
	// fill Array with the values of the list  !NEED CHANGE
	while (p != NULL){
		cases[i][0]=p->data.x;
		cases[i][1]=p->data.y;
		cases[i][2]=p->data.z;
		i++;
		p = p->next;
	}
	// destroy list
	free_list(head);

	int counter = 0;
	int nthreads, tid;
	omp_set_num_threads(num_threads);
	nthreads = omp_get_num_threads();

	#pragma omp parallel for private(tid)
	for (int i = 0; i<size ; i++){
		tid = omp_get_thread_num();
		int x,y,z;	//!NEED CHANGE
		float att;
		#pragma omp threadsave(att)
		// !NEED CHANGE
		x = cases[i][0];
		y = cases[i][1];
		z = cases[i][2];
		#pragma omp threadsave(NUM_NODES,minimalATT,min_x,min_y,min_z,simulate())
		//!NEED CHANGE
		att  = simulate(x,y,z);	
		
		//!NEED CHANGE
		if (minimalATT>att){
			minimalATT = att;
			min_x = x;
			min_y = y;
			min_z = z;
			
		}
		#pragma omp atomic
		counter++;
		fflush(stdout);
		printf("\r Simulated %d of %d...",counter, size);
		fflush(stdout);
		
	}
	
	#pragma omp barrier
	end = clock();
	int seconds = (double)(end-start)/(double)CLOCKS_PER_SEC;
	int hours = seconds/3600;
	int minutes = (seconds/360)%60;
	seconds = seconds%60;
	printf("\nFinished in %d:%02d:%02dh \n", hours, minutes, seconds);
	printf("Best Configuration: %d %d %d \n", min_x,min_y,min_z); //!NEED CHANGE
	printf("ATT: %fs\n", minimalATT);

	return(0);
} 