//-------------------------------------
// FILE NAME: sc3.c
// FILE PURPOSE:
// Simulates simple game from Starcraft series. 
// The real purpose is to master multy thread programing by writing a really simple game.
//---------------------------------------------
// THe whole development procces can be found at: https://github.com/NoHomey/StarCraft3_school_homework.
// Finalizated program can be found at https://github.com/NoHomey/StarCraft3_school_homework/final or https://github.com/NoHomey/StarCraft3_school_homework/os/final.
// If you do visit one of github links please read the README.md file on that page first!
#include <unistd.h>	//In order to use sleep().
#include <stdio.h>	//In order to use stdin, out and err.
#include <pthread.h>	//In oder to use all pthread family system functions and basicaly to use threads.

unsigned map_minerals = 5000;	//Those are minerals on the map (Global so all threads have accses to it.)
unsigned minerals = 0;	//Those are minerals currently scladed in the main command center (Global so all threads have accses to it.)
unsigned collected_minerals = 0;	//Those are minerals that are currently stored from all command centers (Global so all threads have accses to it.)
unsigned all_minerals = 0;	//Those are minerals collected by all commnad centers (Global so all threads have accses to it.)
unsigned n = 0;	//This is the number of currently created workers (Global so all threads have accses to it.)
unsigned m = 0;	//This is the number of currently created soldiers (Global so all threads have accses to it.)
unsigned k = 0;	//This is the number of currently created command centers (excluding the main one wich is created by default) & (Global so all threads have accses to it.)
unsigned command_centers_minerals[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};	//Those are minerals in all player created commnad centers.

pthread_mutex_t mutex_workers;	//Mutex that is used to lock n only.
pthread_mutex_t mutex_soldiers;  //Mutex that is used to lock m only.
pthread_mutex_t mutex_all_minerals; //Mutex that is used to lock collected_minerals & all_minerals.
pthread_mutex_t mutex_map_minerals; //Mutex that is used to lock map_minerals only.
pthread_mutex_t mutex_minerals;	//Mutex that is used to lock minerals only.
pthread_mutex_t mutex_command_centers;  //Mutex that is used to lock k only.
pthread_mutex_t mutex_command_centers_minerals[12];	//Mutex that is used to lock the coresponding command_centers_minerals[] only.

//--------------------------------------------
// FUNCTION: game_over
// game_over() is used in order to set m to 22 so all threads can end.
// PARAMETERS:
// None.
//----------------------------------------------
void game_over () {
	m = 22;
	printf("An ERROR occurred which forced Game to Over!\n"); 
	return;
}

//--------------------------------------------
// FUNCTION: worker
// Creates new worker thread.
// PARAMETERS:
// None.
//----------------------------------------------
void* worker () {
	if (pthread_mutex_lock(&mutex_workers) != 0) { //Check if pthread_mutex_lock fail and if so print error massage and force game_over().
		printf("Failed to lock a mutex: mutex_workers!\n");
		game_over();
		return NULL;	
	}
	const int number = n++ + 1;
	if (pthread_mutex_unlock(&mutex_workers) != 0) {	//Check if pthread_mutex_unlocklock fail and if so print error massage and force game_over().
		printf("Failed to unlock mutex: mutex_workers!\n");
		game_over();
		return NULL;
	}
	int worker_minerals, i , flag, main_status, others_status;	//Define worker_minerals (the minerals that the worker currently holds), i (a counter), flag (a bool like varible used inorder to reset worker's mining cycle), main_status (the status from trylocking mutex_minerals) & others_status (the status from trylocking command_centers_minerals).
	worker_minerals = i = flag = 0;	//Inicializate worker_minerals i flag with 0.
	if (n > 5) printf("SCV good to go, sir.\n");	//If the player created that worker print the creation massage.
	while ((m < 20) && (map_minerals > 0)) {	//Loop while soldiers are less than 20 & minerals on map are more than 0.
		if (pthread_mutex_lock(&mutex_map_minerals) != 0) {	//Check if pthread_mutex_lock fail and if so print error massage and force game_over().
			printf("Failed to lock mutex: mutex_map_minerals!\n");
			game_over();
			return NULL;
		}
		if (map_minerals >= 8) {	//If ther are minerals left on map mine 8 of them.
			printf("SVC %d is mining\n", number);	//Indicate that the worker is mining.
			map_minerals -= 8;
			worker_minerals += 8;
		} else map_minerals = 0;	//Else protect aggainst fake mining.
		if (pthread_mutex_unlock(&mutex_map_minerals) != 0) { //Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
			printf("Failed to unlock mutex: mutex_map_minerals!\n");
			game_over();
			return NULL;
		}
		while (worker_minerals > 0) {	//Loop while worker has minerals carried by him.
			main_status = pthread_mutex_trylock(&mutex_minerals);	//Trylock mutex_minerals and assign the result to main_status.
			if (main_status == 0) {	//If mutex_minerals has been successfuly locked than deliver minerals to minerals.
				printf("SVC %d is transporting minerals\n", number);	//Indicate that the worker is transporting minerals.
				sleep(2);	//Sleep 2 secs so other workers can store minerals in other command centers.
				minerals += worker_minerals;	//Increment minerals with worker_minerals.
				if (pthread_mutex_unlock(&mutex_minerals) != 0) { //Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
					printf("Failed to unlock mutex: mutex_minerals!\n");
					game_over();
					return NULL;
				}	
				flag = 1;	//Set flag to 1 (true) to indicate successfuly transported minerals.
				printf("SVC %d delivered minerals to Command Center 1\n", number);		//Indicate the successful deliverment to Command Center 1.
			} else {	//Else check if the mutex is locked by other thread and check other commnad centers if there are so.
				if (main_status == 16) {	//If main_status is 16 this means that is currently locked by other thread.
					i = 0;	//Reset i;
					while(i < k) {	//Loop througth other commnad centers.
						others_status = pthread_mutex_trylock(&mutex_command_centers_minerals[i]);	//Trylock mutex_command_centers_minerals[i] and assign the result to others_status.
						if (others_status == 0) {	//If others_status is 0 that means that it can be locked.
							printf("SVC %d is transporting minerals\n", number);	//Indicate that the worker is transporting minerals.
							sleep(2);	//Sleep 2 secs so other workers can store minerals in other command centers.
							command_centers_minerals[i] += worker_minerals; //increment command_centers_minerals[i] with worker_minerals.
							if (pthread_mutex_unlock(&mutex_command_centers_minerals[i]) != 0) {	//Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
								printf("Failed to unlock mutex: mutex_command_centers_minerals %d!\n", (i + 2));
								game_over();
								return NULL;
							}
							flag = 1; //Set flag to 1 (true) to indicate successfuly transported minerals.
							printf("SVC %d delivered minerals to Command Center %d\n", number, (i + 2)); //Indicate the successful deliverment to Command Center i + 2.
							break;	
						} else {
							if (others_status == 16) i++;	//If others_status is 16 this means that the mutex is currently locked by other thread, so increment i 
							else {	//Else pthread_mutex_trylock fail and if so print error massage and force game_over().
								printf("Failed to tylock mutex:mutex_command_centers_minerals %d!\n", (i + 2));
								game_over();
								return NULL;
							}
						}
					}
				} else {	//Else pthread_mutex_trylock fail and if so print error massage and force game_over().
					printf("Failed to tylock mutex: mutex_minerals!\n");
					game_over();
					return NULL;
				}
			}
			if (flag == 1) {	//If flag is setted to 1 increment collected_minerals, all_minerals & reset worker_minerals & flag.
				if (pthread_mutex_lock(&mutex_all_minerals) != 0) {	//Check if pthread_mutex_lock fail and if so print error massage and force game_over().
					printf("Failed to lock mutex: mutex_all_minerals!\n");
					game_over();
					return NULL;
				}
				collected_minerals += worker_minerals;	//Increment collected_minerals with worker_minerals.
				all_minerals += worker_minerals;	//increment all_minerals with worker_minerals.
				if (pthread_mutex_unlock(&mutex_all_minerals) != 0) {	//Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
					printf("Failed to unlock mutex: mutex_all_minerals!\n");
					game_over();
					return NULL;
				}
				worker_minerals = 0;	//Reset worker_minerals.
				flag = 0;	//Reset flag.
			}
			else sleep(1);	//Else sleep for 1 sec and try again to deliver the minerals to some command center.	
		}

	}
	return NULL;	//Return from worker.
}

//--------------------------------------------
// FUNCTION: command_center
// Creates new worker thread.
// PARAMETERS:
// None.
//----------------------------------------------
void* command_center () {
	if (pthread_mutex_init(&mutex_command_centers_minerals[k], NULL) != 0) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_command_centers_minerals %d\n", (k + 2));
		game_over();
		return NULL;
	} else {
		if (pthread_mutex_lock(&mutex_command_centers) != 0) {	//Check if pthread_mutex_lock fail and if so print error massage and force game_over().
		printf("Failed to lock mutex: mutex_command_centers_minerals %d\n", (k + 2));
		game_over();
		return NULL;
		}
		k++;	//increment k;
		if (pthread_mutex_unlock(&mutex_command_centers) != 0) {	//Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
			printf("Failed to unlock mutex: mutex_command_centers_minerals %d\n", (k + 1));
			game_over();
			return NULL;
		}
		printf("Command center %d created.\n", (k + 1));	//Indicate that new command centers was successfuly created.
	}
	return NULL;	//Return from command_center.
}

//--------------------------------------------
// FUNCTION: soldier
// Create ne soldier.
// PARAMETERS:
// None.
//----------------------------------------------
void* soldier () {
	sleep(1);	//Sleep for 1 sec.
	if (pthread_mutex_lock(&mutex_soldiers) != 0) {	//Check if pthread_mutex_lock fail and if so print error massage and force game_over().
		printf("Failed to lock mutex: soldiers!\n");
		game_over();
		return NULL;
	}
	m++;	//Increment m.
	if (pthread_mutex_unlock(&mutex_soldiers) != 0) {	//Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
		printf("Failed to unlock mutex: soldiers!\n");
		game_over();
		return NULL;
	}	
	printf("You wanna piece of me, boy?\n");	//Indicate that new soldier was successfuly created.
	return NULL;
}

//--------------------------------------------
// FUNCTION: main
// To simulate the main command center initialize all mutex (except mutex_command_centers_minerals[]), join all hreads with main and destroy all mutex.
// PARAMETERS:
// None.
//----------------------------------------------
int main () {
	char command;	//Define command (the symbol writen form stdin).
	int i;	//Define i (counter).
	
	pthread_t workers[100],  soldiers[20], centers[12];	//Define the threads id arrays: workers[100], soldiers[20] & centers[12].
	
	if (pthread_mutex_init(&mutex_workers, NULL) != 0) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_workers!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_command_centers, NULL) != 0) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_command_center!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_soldiers, NULL) != 0) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_soldiers!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_map_minerals, NULL) != 0) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_map_minerals!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_minerals, NULL) != 0) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_minerals!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_all_minerals, NULL) != 0) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_all_minerals!\n");
		game_over();
		return 0;
	}
	for(i = 0; i < 5;i++) {	//Loop 5 times inorder to create the first 5 workers.
		if (pthread_create(&workers[i], NULL, worker, NULL) != 0) {	//Check if pthread_create fail and if so print error massage and force game_over().
			printf("Failed to crate new worker!\n");
			game_over();
			break;
		}	
	}
	while(m < 20) {
		command = getchar();
		if ((command == 'm') || (command == 's') || (command == 'c')) {
			if (pthread_mutex_lock(&mutex_all_minerals) != 0) {	//Check if pthread_mutex_lock fail and if so print error massage and force game_over().
				printf("Failed to lock mutex: mutex_all_minerals!\n");
				game_over();
				break;
			}
			if (command == 'm') {	//Check if written command is m and if so check is it possible to create new soldier.
				if ((all_minerals >= 50) && (m < 20)) {	//Check if there are enough minerals and if m is valid and if so decrement all_minerals with 50.
					all_minerals -= 50;	//Decrement all_minerals with 50.
					if (pthread_create(&soldiers[m], NULL, soldier, NULL) != 0) {	//Check if pthread_create fail and if so print error massage and force game_over().
						printf("Failed to create new sodler!\n");
						game_over();
						break;
					}
				} else printf("Not enough minerals.\n");	//Else indicate that that there are not enough minerals rigth now.
			}
			if (command == 's') {	//Check if written command is s and if so check is it possible to create new worker.
				if ((all_minerals >= 50) && (n < 100)) {	//Check if there are enough minerals and if n is valid and if so decrement all_minerals with 50.
					all_minerals -= 50;	//Decrement all_minerals with 50.
					if (pthread_create(&workers[n], NULL, worker, NULL) != 0) {	//Check if pthread_create fail and if so print error massage and force game_over().
						printf("Failed to create new worker!\n");
						game_over();
						break;
					}
				
				} else printf("Not enough minerals.\n");	//Else indicate that that there are not enough minerals rigth now.
			}
			if (command == 'c') {	///Check if written command is c  and if so check is it possible to create new command center.
				if ((all_minerals >= 400) && (k < 12)) {	//Check if there are enough minerals and if c is valid and if so decrement all_minerals with 400.
					all_minerals -= 400;	//Decrement all_minerals with 400.
					if (pthread_create(&centers[k], NULL, command_center, NULL) != 0) {	//Check if pthread_create fail and if so print error massage and force game_over().
						printf("Failed to create new command_center!\n");
						game_over();
						break;
					}
				} else printf("Not enough minerals.\n");	//Else indicate that that there are not enough minerals rigth now.
			}
			if (pthread_mutex_unlock(&mutex_all_minerals) != 0 ) {	//Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
				printf("Fail to unlock mutex: minerals!\n");
				game_over();
				break;
			}
		}
		if ((all_minerals == 0) && (map_minerals == 0)) {	//IF there is no way to create 20 soldiers assign ~ to command.
			command = '~';
			break;
		}
	}
	for(i = 0; i < n;i++) {	//Loop n times and join all workers with main.
		if (pthread_join(workers[i], NULL) != 0) {	//Check if pthread_join fail and if so print error massage and force game_over().
			printf("Failed to join main and a worker!\n");
			game_over();
			break;
		}
	}
	for(i = 0; i < m;i++) {	//Loop m times and join all soldiers with main.
		if (pthread_join(soldiers[i], NULL) != 0 ) {	//Check if pthread_join fail and if so print error massage and force game_over().
			printf("Failed to join main and a sodler!\n");
			game_over();
			break;
		}
	}
	for(i = 0; i < k;i++) {	//Loop k times and join all command_centers with main and destroy mutex_command_centers_minereals[].
		if (pthread_join(centers[i], NULL) != 0) {	//Check if pthread_join fail and if so print error massage and force game_over().
			printf("Failed to join main and a center!\n");
			game_over();
			break;
		}
		if (pthread_mutex_destroy(mutex_command_centers_minerals + i) != 0) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
			printf("Failed to destroy mutex: mutex_command_centers_minerals[%d]!\n", i);
			game_over();
			break;
		}
	}
	if (pthread_mutex_destroy(&mutex_workers) != 0) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
		printf("Failed to destroy mutex: mutex_workers!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_destroy(&mutex_command_centers) != 0) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
		printf("Failed to destroy mutex: mutex_command_center!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_destroy(&mutex_soldiers) != 0) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
		printf("Failed to destroy mutex: mutex_soldiers!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_destroy(&mutex_minerals) != 0) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
		printf("Failed to destroy mutex: mutex_minerals!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_destroy(&mutex_map_minerals) != 0) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
		printf("Failed to destroy mutex: mutex_map_minerals!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_destroy(&mutex_all_minerals) != 0) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
		printf("Failed to destroy mutex: mutex_all_minerals!\n");
		game_over();
		return 0;
	}
	if (command == '~') printf("You Failed to create 20 soldiers Game Over!\n");	//Message that game can't be won.
	else {
		printf("Game Won !, You successfuly created 20 soldiers\n");	//Message that game has been won.
		printf("Game started with 5000 minerals on the map, %d minerals left on map and you successfuly collected %d minerals", map_minerals, collected_minerals);	//Print starting minerals, those left on map and all collected.
	}
	return 0;
}
