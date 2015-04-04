#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

int map_minerals = 5000;

int minerals = 0;

int collected_minerals = 0;

int n = 0;

int m = 0;

int k = 0;

int command_centers_minerals[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

pthread_mutex_t mutex_workers;

pthread_mutex_t mutex_map_minerals;

pthread_mutex_t mutex_minerals;

pthread_mutex_t mutex_solders;

pthread_mutex_t mutex_command_centers;

pthread_mutex_t mutex_command_centers_minerals[12];

void game_over () {
	m = 21;
	printf("An ERROR ocured which forced game over!\n"); 
	return;
}

void* worker () {

	if (pthread_mutex_lock(&mutex_workers) != 0) {
		perror("Failed to lock mutex: workers!\n");
		game_over();
		return;	
	}
	
	int number = ++n;
	
	if (pthread_mutex_unlock(&mutex_workers) != 0) {
		perror("Failed to unlock mutex: workers!\n");
		game_over();
		return;
	}
	
	int worker_minerals = 0;
	
	int i;
	
	int main_status;
	
	int others_status[12];
	
	if (n > 5) {
		printf("SCV good to go, sir.\n");
	}
	
	while (m < 20) {
		printf("SVC %d is mining\n", number);
		if (pthread_mutex_lock(&mutex_map_minerals) != 0) {
			perror("Failed to lock mutex: map_minerals!\n");
			game_over();
			return;
		}
		
		map_minerals -= 8;
		
		if (pthread_mutex_unlock(&mutex_map_minerals) != 0) {
			perror("Failed to unlock mutex: map_minerals!\n");
			game_over();
			return;
		}
				
		worker_minerals += 8;
		
		printf("SVC %d is transporting minerals\n", number);
		
		while (worker_minerals > 0) {
			main_status = pthread_mutex_trylock(&mutex_minerals);
			if (main_status == 0) {
				sleep(1);
				minerals += worker_minerals;
				collected_minerals += worker_minerals;
				if (pthread_mutex_unlock(&mutex_minerals) != 0) {
					perror("Failed to unlock mutex: map_minerals!\n");
					game_over();
					return;
				}
				worker_minerals = 0;
				printf("SVC %d delivered minerals to Command Center 1\n", number);
				break;
			}else {
			
		
			for(i = 0; i < k; i++) {
				others_status[i] = pthread_mutex_trylock(&mutex_command_centers_minerals[i]);
				if (others_status[i] == 0) {
					sleep(1);
					command_centers_minerals[i] += worker_minerals;
					collected_minerals += worker_minerals;
					worker_minerals = 0;
					if (pthread_mutex_unlock(&mutex_command_centers_minerals[i]) != 0) {
						perror("Failed to unlock mutex: command_centers_minerals M!\n");
						game_over();
						return;
					}
					printf("SVC %d delivered minerals to Command Center %d\n", number, (i + 2));
					break;	
				}
			}
			}
			
		}

	}
	return;
}

void* command_center () {
	if (pthread_mutex_init(&mutex_command_centers_minerals[k], NULL) != 0) {
		perror("Fail to initialize mutex: command_centers_minerals M\n");
		game_over();
		return;
	} else {
		if (pthread_mutex_lock(&mutex_command_centers) != 0) {
		perror("Failed to lock mutex: commadn_centers!\n");
		game_over();
		return;
		}
		k++;
		if (pthread_mutex_unlock(&mutex_command_centers) != 0) {
			perror("Failed to unlock mutex: commadn_centers!\n");
			game_over();
			return;
		}
		printf("Command center %d created.\n", (k + 1));
	}
	
	return;
}

void* solder () {

	if (pthread_mutex_lock(&mutex_solders) != 0) {
		perror("Failed to lock mutex: solders!\n");
		game_over();
		return;
	}
	m++;
	if (pthread_mutex_unlock(&mutex_solders) != 0) {
		perror("Failed to unlock mutex: solders!\n");
		game_over();
		return;
	}	
	printf("You wanna piece of me, boy?\n");
	
	return;
}

int return_all_minerals () {

	int all = minerals;
	int i;
	for (i = 0; i < k; i++) {
		all += command_centers_minerals[i];
	}
	return all;
}

int main () {

	char command;
	
	int all = 0;
	
	int over;
	
	int i;
	
	pthread_t workers[100];
	
	pthread_t solders[20];
	
	pthread_t centers[12];
	
	if (pthread_mutex_init(&mutex_workers, NULL) != 0) {
		perror("Fail to initialize mutex: workers!\n");
		game_over();
		return 0;
	}
	
	if (pthread_mutex_init(&mutex_command_centers, NULL) != 0) {
		perror("Fail to initialize mutex: command_center!\n");
		game_over();
		return 0;
	}
	
	if (pthread_mutex_init(&mutex_solders, NULL) != 0) {
		perror("Fail to initialize mutex: solders!\n");
		game_over();
		return 0;
	}
	
	if (pthread_mutex_init(&mutex_map_minerals, NULL) != 0) {
		perror("Fail to initialize mutex: minerals!\n");
		game_over();
		return 0;
	}
	
	for(i = 0; i < 5;i++) {
		if (pthread_create(&workers[i], NULL, worker, NULL) != 0) {
			printf("Fail to crate new worker!\n");
			game_over();
			return 0;
		}	
	}
	
	while(m < 20) {
		
		command = getchar();
		
		if ((command == 'm') || (command == 's') || (command == 'c')) {
			all = return_all_minerals();
			printf ("map minerals: %d, collected: %d, k: %d\n", map_minerals, all, k);
			if (pthread_mutex_lock(&mutex_minerals) != 0) {
				perror("Fail to lock mutex: minerals!\n");
				game_over();
				return 0;
			}
		
			if (command == 'm') {
				if (all > 50) {
					minerals -= 50;
					if (pthread_create(&solders[m], NULL, solder, NULL) != 0) {
						printf("Fail to crate new sodler!\n");
						game_over();
						return 0;
					}
				
				} else {
					printf("No minerals!\n");
				}
			}
		
			if (command == 's') {
				if (all > 50) {
					minerals -= 50;
					if (pthread_create(&workers[n], NULL, worker, NULL) != 0) {
						printf("Fail to crate new worker!\n");
						game_over();
						return 0;
					}
				
				} else {
					printf("No minerals!\n");
				}
			}
		
			if (command == 'c') {
				if (all > 400) {
					minerals -= 400;
					if (pthread_create(&centers[k], NULL, command_center, NULL) != 0) {
						printf("Fail to crate new command_center!\n");
						game_over();
						return 0;
					}
				
				} else {
					printf("No minerals!\n");
				}
			}
			
			if (pthread_mutex_unlock(&mutex_minerals) != 0 ) {
				perror("Fail to unlock mutex: minerals!\n");
				game_over();
				return 0;
			}
		}
	
		if (map_minerals < 1000) {
			printf("No't enough minerals in order to create 20 solders Game Over!\n");
			game_over();
			return 0;
		
		}
	
	}
	
	for(i = 0; i < n;i++) {
		if (pthread_join(workers[i], NULL) != 0) {
			printf("Fail to join main and a worker!\n");
			game_over();
			return 0;
		}
	
	}
	
	for(i = 0; i < m;i++) {
		if (pthread_join(solders[i], NULL) != 0 ) {
			printf("Fail to join main and a sodler!\n");
			game_over();
			return 0;
		}
	}
	
	for(i = 0; i < k;i++) {
		if (pthread_join(centers[i], NULL) != 0) {
			printf("Fail to join main and a center!\n");
			game_over();
			return 0;
		}
	}
	
	printf("Game Won !, You successfuly created 20 solders\n");
	printf("Game started with 5000 minerals on the map, %d minerals left on map and you successfuly collected %d minerals", map_minerals, collected_minerals);

	return 0;
}
