#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

int map_minerals = 5000;

int minerals = 0;

int n = 0;

int m = 0;

int k = -1;

int command_centers_minerals[12] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};


pthread_mutex_t mutex_workers;

pthread_mutex_t mutex_map_minerals;

pthread_mutex_t mutex_minerals;

pthread_mutex_t mutex_solders;

pthread_mutex_t mutex_command_center;

pthread_mutex_t mutex_command_centers[12];


void* worker (void) {

	pthread_mutex_lock(&mutex_workers);
	n++;
	pthread_mutex_unlock(&mutex_workers);
	
	char* name = "SVC " + (n + 49);
	
	int worker_minerals = 0;
	
	printf("SCV good to go, sir.");
	
	while (m < 20) {
		printf("%s", name);
		printf(" is mining\n");
		pthread_mutex_lock(&mutex_map_minerals);
		map_minerals -= 8;
		pthread_mutex_unlock(&mutex_map_minerals);		
		worker_minerals += 8;
		while (worker_minerals > 0) {
		
		
		}
		
		printf("%s", name);
		printf(" is transporting minerals\n	");
	
	
	
	}
	
	

}

void* command_center (void) {

	pthread_mutex_lock(&mutex_command_center);
	command_centers_minerals[++k] = 0;
	pthread_mutex_unlock(&mutex_command_center);
	if (pthread_mutex_init(&mutex_command_centers[k], NULL) != 0) {
		perror("Fail to initialize mutex: workers!");
	}

}

void* solder (void) {




}

int return_all_minerals (void) {

	int all = minerals;
	int i;
	for (i = 0; i <= k; i++) {
		all += command_centers_minerals[i];
	}
	return all;
}

int main (void) {

	char command;
	
	int all = 0;
	
	pthread_t workers[100];
	
	pthread_t solders[20];
	
	pthread_t centers[12];
	
	if (pthread_mutex_init(&mutex_workers, NULL) != 0) {
		perror("Fail to initialize mutex: workers!");
	}
	
	if (pthread_mutex_init(&mutex_command_center, NULL) != 0) {
		perror("Fail to initialize mutex: command_center!");
	}
	
	if (pthread_mutex_init(&mutex_solders, NULL) != 0) {
		perror("Fail to initialize mutex: solders!");
	}
	
	if (pthread_mutex_init(&mutex_map_minerals, NULL) != 0) {
		perror("Fail to initialize mutex: minerals!");
	}
	

	while(m < 20) {
		
		command = getchar();
		
		if ((command == 'm') || (command == 's') || (command == 'c')) {
			pthread_mutex_lock(&mutex_minerals);
			all = return_all_minerals();
		
		}
		
		if (command == 'm') {
			if (all > 50) {
				minerals -= 50;
				
			}
		}
		
		if (command == 's') {
			if (all > 50) {
				minerals -= 50;
				
			}
		}
		
		if (command == 'c') {
			if (all > 400) {
				minerals -= 400;
				
			}
		}
	
	
	
	
	}




	return 0;
}
