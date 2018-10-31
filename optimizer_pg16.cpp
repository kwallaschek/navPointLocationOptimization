// gcc -fopenmp -o go go.c
// ./go
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

#define NUM_LINKS 105

int NUM_NODES = 1;
int NUM_POSSIBILITIES = 0;
float minimalATT = 9999999;
int* min_locs;





int main (int argc, char *argv[]) {
	clock_t start, startSim, endSim, end;
	double cpu_time_used;
	int num_threads = 1;
	start = clock();
	if (argv[2] != NULL){
		num_threads = atoi(argv[2]);
	}
	if (argv[1] != NULL){
		NUM_NODES = atoi(argv[1]);
	}

	int size = 0;
	min_locs = (int*) malloc(NUM_NODES+1*sizeof(int));
	NODE* head;
	NODE* node;
	DATA data_node;
	init(&head);

	for (int i = 0; i<NUM_LINKS-1; i++){

		if (NUM_NODES>1){
			for (int j = 0; j<NUM_LINKS-1; j++){
				if (NUM_NODES>2){
					for (int k = 0; k<NUM_LINKS-1; k++){
						if(i != j && i != k && k != j){
							data_node.x = i;
							data_node.y = j;
							data_node.z = k;
							head = add(head, data_node);
							size++;
					}	
					}
				}
				else{
					if(i != j){
						data_node.x = i;
						data_node.y = j;
						head = add(head, data_node);
						size++;
					}
				}
				}
			}
		 else {
			data_node.x = i;
			head = add(head, data_node);
			size++;
		}

	}
	
	NODE * p = head;

	// init Array and allocate space for it.
	int** cases;
  	cases = (int**) malloc(size*sizeof(int*));
  		for (int i = 0; i < size; i++)
  				cases[i] = (int*) malloc(NUM_NODES+1*sizeof(int));

	int i = 0;
	// fill Array with the values of the list
	while (p != NULL){
		cases[i][0]=p->data.x;
		if (NUM_NODES>1){
			cases[i][1]=p->data.y;
				if (NUM_NODES>2){
					cases[i][2]=p->data.z;
				}
		}
		i++;
		p = p->next;
	}
	// destroy list
	free_list(head);

	int counter = 1;
	int lastATT = 0;
	startSim = clock();
	endSim = clock();

	int nthreads, tid;
	omp_set_num_threads(num_threads);
	nthreads = omp_get_num_threads();

	#pragma omp parallel for private(tid)
	for (int i = 0; i<size ; i++){
		/*fflush(stdout);
		double eta = (double)(((endSim-startSim)/counter)*(size-counter))/(double)CLOCKS_PER_SEC;
		int minutes = eta/60;
		int seconds = eta-minutes*60;
		printf("\r%c[2KSimulating %d of %d... Current minimal ATT: %d s |Last ATT: %d s | %d %d %d | ETA: %d:%02dm",27,counter,size, minimalATT, lastATT,cases[i][0],cases[i][1],cases[i][2], minutes, seconds);
		fflush(stdout);*/
		tid = omp_get_thread_num();
		
		//printf("Thread ID%d did: %d \n", tid, i);
		//float att = 0;
		int locations[NUM_NODES];
		#pragma omp threadsave(minimalATT,locations)
		//locations = (int*)calloc(sizeof(int),NUM_NODES);
		for (int j = 0; j<NUM_NODES; j++)
			locations[j] = cases[i][j];
		
		
		float att;
		//#pragma omp threadsave(att)
		
		att  = simulate(locations, NUM_NODES);	
		
		

		if (minimalATT>att){
			minimalATT = att;
			min_locs = locations;
			
		}

		#pragma omp atomic
		counter++;
		int seconds = (double)(clock()-start)/(double)CLOCKS_PER_SEC;
		seconds = (seconds/counter)*(size-counter);
		int hours = seconds/3600;
		int minutes = (seconds/60)%60;
		seconds = seconds%60;
		fflush(stdout);
		printf("\r Simulated %d of %d...  Current minATT: %f  ETA: %d:%02d:%02dh",counter, size, minimalATT, hours, minutes, seconds);
		fflush(stdout);
		endSim = clock();
		
	}
	#pragma omp barrier
	end = clock();
	int seconds = (double)(end-start)/(double)CLOCKS_PER_SEC;
	int hours = seconds/3600;
	int minutes = (seconds/60)%60;
	seconds = seconds%60;
	printf("\nFinished in %d:%02d:%02dh \n", hours, minutes, seconds);
	printf("Best Configuration: ");
	for (int i = 0; i<NUM_NODES; i++){
		printf(" %d ", min_locs[i]);
	}
	printf("ATT: %fs\n", minimalATT);

	return(0);
} 