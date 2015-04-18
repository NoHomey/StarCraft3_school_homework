//--------------------------------------------
// NAME: Ivo Stratev
// CLASS: XIb
// NUMBER: 16
// PROBLEM: #3
// FILE NAME: sc3.c
// FILE PURPOSE:
// няколко реда, които описват накратко
// предназначението на файла
// ...
//---------------------------------------------
//#include <time.h>
#include <unistd.h>	//In order to use sleep().
#include <stdio.h>	//In order to use stdin, out and err.
#include <pthread.h>	//In oder to use all pthread family system functions and basicaly to use threads.

unsigned map_minerals = 5000;	//Those are minerals on the map (Global so all threads have accses to it.)
unsigned minerals = 0;	//Those are minerals currently scladed in the main command center (Global so all threads have accses to it.)
unsigned collected_minerals = 0;	//Those are minerals that are currently scladed from all command centers (Global so all threads have accses to it.)
unsigned all_minerals = 0;	//Those are minerals collected by all commnad centers (Global so all threads have accses to it.)
unsigned n = 0;
unsigned m = 0;
unsigned k = 0;
unsigned command_centers_minerals[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

pthread_mutex_t mutex_workers;
pthread_mutex_t mutex_solders;
pthread_mutex_t mutex_all_minerals;
pthread_mutex_t mutex_map_minerals;
pthread_mutex_t mutex_minerals;
pthread_mutex_t mutex_command_centers;
pthread_mutex_t mutex_command_centers_minerals[12];

//--------------------------------------------
// FUNCTION: xxyyzz (име на функцията)
// предназначение на функцията
// PARAMETERS:
// списък с параметрите на функцията
// и тяхното значение
//----------------------------------------------
void game_over () {
	m = 22;
	printf("An ERROR occurred which forced Game to Over!\n"); 
	return;
}

//--------------------------------------------
// FUNCTION: xxyyzz (име на функцията)
// предназначение на функцията
// PARAMETERS:
// списък с параметрите на функцията
// и тяхното значение
//----------------------------------------------
void* worker () {
	if (pthread_mutex_lock(&mutex_workers) != 0) {
		printf("Failed to lock a mutex: mutex_workers!\n");
		game_over();
		return;	
	}
	const int number = n++ + 1;
	if (pthread_mutex_unlock(&mutex_workers) != 0) {
		printf("Failed to unlock mutex: mutex_workers!\n");
		game_over();
		return;
	}
	int worker_minerals, i , flag, main_status, others_status;
	worker_minerals = i = flag = 0;
	if (n > 5) printf("SCV good to go, sir.\n");
	while ((m < 20) && (map_minerals > 0)) {
		printf("SVC %d is mining\n", number);
		if (pthread_mutex_lock(&mutex_map_minerals) != 0) {
			printf("Failed to lock mutex: mutex_map_minerals!\n");
			game_over();
			return;
		}
		if (map_minerals > 0) map_minerals -= 8;
		if (pthread_mutex_unlock(&mutex_map_minerals) != 0) {
			printf("Failed to unlock mutex: mutex_map_minerals!\n");
			game_over();
			return;
		}
		worker_minerals += 8;
		while (worker_minerals > 0) {
			main_status = pthread_mutex_trylock(&mutex_minerals);
			if (main_status == 0) {
				printf("SVC %d is transporting minerals\n", number);
				//	sleep(2);
				minerals += worker_minerals;
				if (pthread_mutex_unlock(&mutex_minerals) != 0) {
					printf("Failed to unlock mutex: mutex_minerals!\n");
					game_over();
					return;
				}	
				flag = 1;
				printf("SVC %d delivered minerals to Command Center 1\n", number);	
			} else {
				if (main_status == 16) {
					while(i < k) {
						others_status = pthread_mutex_trylock(&mutex_command_centers_minerals[i]);
						if (others_status == 0) {
							printf("SVC %d is transporting minerals\n", number);
							//sleep(2);
							command_centers_minerals[i] += worker_minerals;
							if (pthread_mutex_unlock(&mutex_command_centers_minerals[i]) != 0) {
								printf("Failed to unlock mutex: mutex_command_centers_minerals %d!\n", (i + 2));
								game_over();
								return;
							}
							flag = 1;
							printf("SVC %d delivered minerals to Command Center %d\n", number, (i + 2));
							break;	
						} else {
							if (others_status == 16) i++;
							else {
								printf("Failed to tylock mutex:mutex_command_centers_minerals %d!\n", (i + 2));
								game_over();
								return;
							}
						}
					}
				} else {
					printf("Failed to tylock mutex: mutex_minerals!\n");
					game_over();
					return;
				}
			}
			if (flag == 1) {
				if (pthread_mutex_lock(&mutex_all_minerals) != 0) {
					printf("Failed to lock mutex: mutex_all_minerals!\n");
					game_over();
					return;
				}
				collected_minerals += worker_minerals;
				all_minerals += worker_minerals;
				if (pthread_mutex_unlock(&mutex_all_minerals) != 0) {
					printf("Failed to unlock mutex: mutex_all_minerals!\n");
					game_over();
					return;
				}
				worker_minerals = 0;
				flag = 0;
			}
			//sleep(1);	
		}

	}
	return;
}

//--------------------------------------------
// FUNCTION: xxyyzz (име на функцията)
// предназначение на функцията
// PARAMETERS:
// списък с параметрите на функцията
// и тяхното значение
//----------------------------------------------
void* command_center () {
	if (pthread_mutex_init(&mutex_command_centers_minerals[k], NULL) != 0) {
		printf("Failed to initialize mutex: mutex_command_centers_minerals %d\n", (k + 2));
		game_over();
		return;
	} else {
		if (pthread_mutex_lock(&mutex_command_centers) != 0) {
		printf("Failed to lock mutex: mutex_command_centers_minerals %d\n", (k + 2));
		game_over();
		return;
		}
		k++;
		if (pthread_mutex_unlock(&mutex_command_centers) != 0) {
			printf("Failed to unlock mutex: mutex_command_centers_minerals %d\n", (k + 1));
			game_over();
			return;
		}
		printf("Command center %d created.\n", (k + 1));
	}
	return;
}

//--------------------------------------------
// FUNCTION: xxyyzz (име на функцията)
// предназначение на функцията
// PARAMETERS:
// списък с параметрите на функцията
// и тяхното значение
//----------------------------------------------
void* solder () {
	//sleep(1);
	if (pthread_mutex_lock(&mutex_solders) != 0) {
		printf("Failed to lock mutex: solders!\n");
		game_over();
		return;
	}
	m++;
	if (pthread_mutex_unlock(&mutex_solders) != 0) {
		printf("Failed to unlock mutex: solders!\n");
		game_over();
		return;
	}	
	printf("You wanna piece of me, boy?\n");
	return;
}

//--------------------------------------------
// FUNCTION: xxyyzz (име на функцията)
// предназначение на функцията
// PARAMETERS:
// списък с параметрите на функцията
// и тяхното значение
//----------------------------------------------
int main () {
	char command;
	int i;
	
	pthread_t workers[100],  solders[20], centers[12];
	
	if (pthread_mutex_init(&mutex_workers, NULL) != 0) {
		printf("Failed to initialize mutex: mutex_workers!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_command_centers, NULL) != 0) {
		printf("Failed to initialize mutex: mutex_command_center!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_solders, NULL) != 0) {
		printf("Failed to initialize mutex: mutex_solders!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_map_minerals, NULL) != 0) {
		printf("Failed to initialize mutex: mutex_map_minerals!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_all_minerals, NULL) != 0) {
		printf("Failed to initialize mutex: mutex_all_minerals!\n");
		game_over();
		return 0;
	}
	for(i = 0; i < 5;i++) {
		if (pthread_create(&workers[i], NULL, worker, NULL) != 0) {
			printf("Failed to crate new worker!\n");
			game_over();
			return 0;
		}	
	}
	while(m < 20) {
		command = getchar();
		if ((command == 'm') || (command == 's') || (command == 'c')) {
			if (pthread_mutex_lock(&mutex_all_minerals) != 0) {
				printf("Failed to lock mutex: mutex_all_minerals!\n");
				game_over();
				return 0;
			}
			if (command == 'm') {
				if ((all_minerals >= 50) && (m < 20)) {
					all_minerals -= 50;
					if (pthread_create(&solders[m], NULL, solder, NULL) != 0) {
						printf("Failed to crate new sodler!\n");
						game_over();
						return 0;
					}
				} else printf("No minerals to do that now!\n");
			}
			if (command == 's') {
				if ((all_minerals >= 50) && (n < 100)) {
					all_minerals -= 50;
					if (pthread_create(&workers[n], NULL, worker, NULL) != 0) {
						printf("Failed to crate new worker!\n");
						game_over();
						return 0;
					}
				
				} else printf("No minerals to do that now!\n");
			}
			if (command == 'c') {
				if ((all_minerals >= 400) && (k < 12)) {
					all_minerals -= 400;
					if (pthread_create(&centers[k], NULL, command_center, NULL) != 0) {
						printf("Failed to crate new command_center!\n");
						game_over();
						return 0;
					}
				} else printf("No minerals to do that now!\n");
			}
			if (pthread_mutex_unlock(&mutex_all_minerals) != 0 ) {
				printf("Fail to unlock mutex: minerals!\n");
				game_over();
				return 0;
			}
		}
		if ((all_minerals == 0) && (map_minerals == 0)) {
			command = '~';
			break;
		}
	}
	for(i = 0; i < n;i++) {
		if (pthread_join(workers[i], NULL) != 0) {
			printf("Failed to join main and a worker!\n");
			game_over();
			return 0;
		}
	}
	for(i = 0; i < k;i++) {
		if (pthread_join(centers[i], NULL) != 0) {
			printf("Failed to join main and a center!\n");
			game_over();
			return 0;
		}
	}
	for(i = 0; i < m;i++) {
		if (pthread_join(solders[i], NULL) != 0 ) {
			printf("Failed to join main and a sodler!\n");
			game_over();
			return 0;
		}
	}
	if (command == '~') printf("You Failed to create 20 solders Game Over!\n");
	else {
		printf("Game Won !, You successfuly created 20 solders\n");
		printf("Game started with 5000 minerals on the map, %d minerals left on map and you successfuly collected %d minerals", map_minerals, collected_minerals);
	}
	return 0;
}
