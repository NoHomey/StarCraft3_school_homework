#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <errno.h>

int map_minerals = 5000;

int minerals = 0;

int collected_minerals = 0;

int all_minerals = 0;

int n = 0;

int m = 0;

int k = 0;

int command_centers_minerals[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

pthread_mutex_t mutex_workers;

pthread_mutex_t mutex_all_minerals;

pthread_mutex_t mutex_map_minerals;

pthread_mutex_t mutex_minerals;

pthread_mutex_t mutex_solders;

pthread_mutex_t mutex_command_centers;

pthread_mutex_t mutex_command_centers_minerals[10];

void game_over () {
	m = 22;
	printf("An ERROR ocured which forced Game Over!\n"); 
	return;
}

void* worker () {

	if (pthread_mutex_lock(&mutex_workers) != 0) {
		perror("Failed to lock mutex: workers!\n");
		game_over();
		return;	
	}
	
	int number = n++ + 1;
	
	if (pthread_mutex_unlock(&mutex_workers) != 0) {
		perror("Failed to unlock mutex: workers!\n");
		game_over();
		return;
	}
	
	int worker_minerals = 0;
	
	int i = 0;
	
	int flag = 0;
	
	int main_status;
	
	int others_status;
	
	if (n > 5) {
		printf("SCV good to go, sir.\n");
	}
	
	while ((m < 20) && (map_minerals > 0)) {
	
		printf("SVC %d is mining\n", number);
		if (pthread_mutex_lock(&mutex_map_minerals) != 0) {
			perror("Failed to lock mutex: map_minerals!\n");
			game_over();
			return;
		}
		
		if (map_minerals >= 8) {
			map_minerals -= 8;
			worker_minerals += 8;
		} else {
			worker_minerals += map_minerals;
			map_minerals = 0;
		}
		
		if (pthread_mutex_unlock(&mutex_map_minerals) != 0) {
			perror("Failed to unlock mutex: map_minerals!\n");
			game_over();
			return;
		}
				
		
		printf("SVC %d is transporting minerals\n", number);
		printf ("minerals: %d, k: %d, m: %d\n", all_minerals, k, m);
		
		while (worker_minerals > 0) {
			main_status = pthread_mutex_trylock(&mutex_minerals);
			if (main_status == 0) {
				sleep(1);
				minerals += worker_minerals;
				pthread_mutex_unlock(&mutex_minerals);
				flag = 1;
				printf("SVC %d delivered minerals to Command Center 1\n", number);
				
			} else {
			
				if (main_status == 16) {
				
					while(i < k) {
						others_status = pthread_mutex_trylock(&mutex_command_centers_minerals[i]);
						if (others_status == 0) {
							sleep(1);
							command_centers_minerals[i] += worker_minerals;
							if (pthread_mutex_unlock(&mutex_command_centers_minerals[i]) != 0) {
								perror("Failed to unlock mutex: command_centers_minerals M!\n");
								game_over();
								return;
							}
							flag = 1;
							printf("SVC %d delivered minerals to Command Center %d\n", number, (i + 2));
							break;	
						} else {
					
							if (others_status == 16) i++;
							else {
								perror("Failed to tylock mutex: all_minerals!\n");
								game_over();
								return;
							}
						}
					}
				} else {
					perror("Failed to tylock mutex: all_minerals!\n");
					game_over();
					return;
				}
			}
			
			if (flag == 1) {
				if (pthread_mutex_lock(&mutex_all_minerals) != 0) {
					perror("Failed to lock mutex: all_minerals!\n");
					game_over();
					return;
				}
				collected_minerals += worker_minerals;
				all_minerals += worker_minerals;
				if (pthread_mutex_unlock(&mutex_all_minerals) != 0) {
					perror("Failed to unlock mutex: all_minerals!\n");
					game_over();
					return;
				}
				
				worker_minerals = 0;
				flag = 0;
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

	//sleep(1);

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

int main () {

	char command;
	
	int i;
	
	pthread_t workers[80];
	
	pthread_t solders[20];
	
	pthread_t centers[10];
	
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
		perror("Fail to initialize mutex: map_minerals!\n");
		game_over();
		return 0;
	}
	
	if (pthread_mutex_init(&mutex_all_minerals, NULL) != 0) {
		perror("Fail to initialize mutex: all_minerals!\n");
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
			if (pthread_mutex_lock(&mutex_all_minerals) != 0) {
				perror("Fail to lock mutex: minerals!\n");
				game_over();
				return 0;
			}
		
			if (command == 'm') {
				if ((all_minerals >= 50) && (m < 20)) {
					all_minerals -= 50;
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
				if ((all_minerals >= 50) && (n < 80)) {
					all_minerals -= 50;
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
				if ((all_minerals >= 400) && (k < 10)) {
					all_minerals -= 400;
					if (pthread_create(&centers[k], NULL, command_center, NULL) != 0) {
						printf("Fail to crate new command_center!\n");
						game_over();
						return 0;
					}
				
				} else {
					printf("No minerals!\n");
				}
			}
			
			if (pthread_mutex_unlock(&mutex_all_minerals) != 0 ) {
				perror("Fail to unlock mutex: minerals!\n");
				game_over();
				return 0;
			}
		}
		
		if ((all_minerals == 0) && (map_minerals == 0)) {
			command = '~';
			break;
		}
	}
	printf("Press Enter if nothing happen in next 10 secs!\n");
	for(i = 0; i < n;i++) {
		if (pthread_join(workers[i], NULL) != 0) {
			perror("Fail to join main and a worker!\n");
			game_over();
			return 0;
		}
	
	}
	for(i = 0; i < k;i++) {
		if (pthread_join(centers[i], NULL) != 0) {
			perror("Fail to join main and a center!\n");
			game_over();
			return 0;
		}
	}
	for(i = 0; i < m;i++) {
		if (pthread_join(solders[i], NULL) != 0 ) {
			
			perror("Fail to join main and a sodler!\n");
			game_over();
			return 0;
		}
	}
	
	if (command == '~')
		printf("You Failed to create 20 solders Game Over!\n");
	else {
		printf("Game Won !, You successfuly created 20 solders\n");
		printf("Game started with 5000 minerals on the map, %d minerals left on map and you successfuly collected %d minerals", map_minerals, collected_minerals);
	}
	
	return 0;
}
