#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int map_minerals = 5000;

int n = 0;

int m = 0;

int command_centers[13] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};


pthread_mutex_t mutex_workers;

pthread_mutex_t mutex_minerals;


void* worker (void) {

	pthread_mutex_lock(&mutex_workers);
	n++;
	pthread_mutex_unlock(&mutex_workers);
	
	char* name = "SVC" + (n + 49);
	
	

}


void* solder (void) {




}




int main (void) {

	char command;
	
	command_centers[0] = 0;
	
	pthread_t workers[100];
	
	pthread_t solders[20];
	
	pthread_t centeers[12];
	
	if (pthread_mutex_init(&mutex_workers, NULL) != 0) {
		perror("Fail to initialize mutex: workers!");
	}
	
	if (pthread_mutex_init(&mutex_minerals, NULL) != 0) {
		perror("Fail to initialize mutex: minerals!");
	}
	

	while(m < 20) {
		
		command = getchar();
		
		if (command == 'm') {
		
		}
		
		if (command == 's') {
		
		}
		
		if (command == 'c') {
		
		}
	
	
	
	
	}




	return 0;
}
