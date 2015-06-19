//--------------------------------------------
// NAME: Ivo Stratev
// CLASS: XIb
// NUMBER: 16
// PROBLEM: #3
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
#include <stdlib.h>
size_t map_minerals = 5000;	//Those are minerals on the map (Global so all threads have accses to it.)
size_t collected_minerals = 0;	//Those are minerals that are currently stored from all command centers (Global so all threads have accses to it.)
size_t all_minerals = 0;	//Those are minerals collected by all commnad centers (Global so all threads have accses to it.)
size_t n = 0;	//This is the number of currently created workers (Global so all threads have accses to it.)
size_t m = 0;	//This is the number of currently created soldiers (Global so all threads have accses to it.)
size_t k = 0;	//This is the number of currently created command centers (excluding the main one wich is created by default) & (Global so all threads have accses to it.)
size_t* command_centers_minerals;	//Those are minerals in all player created commnad centers.
size_t error = 0;
pthread_mutex_t mutex_workers;	//Mutex that is used to lock n only.
pthread_mutex_t mutex_soldiers;  //Mutex that is used to lock m only.
pthread_mutex_t mutex_all_minerals; //Mutex that is used to lock collected_minerals & all_minerals.
pthread_mutex_t mutex_map_minerals; //Mutex that is used to lock map_minerals only.
pthread_mutex_t mutex_command_centers;  //Mutex that is used to lock k only.
pthread_mutex_t mutex_command_centers_minerals[13];	//Mutex that is used to lock the coresponding command_centers_minerals[] only.

//--------------------------------------------
// FUNCTION: game_over
// game_over() is used in order to set m to 22 so all threads can end.
// PARAMETERS:
// None.
//----------------------------------------------
void game_over () {
	m = 22;
	printf("An ERROR occurred which forced Game to Over!\n"); 
	error = 1;
	return;
}

//--------------------------------------------
// FUNCTION: worker
// Creates new worker thread.
// PARAMETERS:
// None.
//----------------------------------------------
void* worker () {
	if (pthread_mutex_lock(&mutex_workers)) { //Check if pthread_mutex_lock fail and if so print error massage and force game_over().
		printf("Failed to lock a mutex: mutex_workers!\n");
		game_over();
		return NULL;	
	}
	const int number = n++ + 1;
	if (pthread_mutex_unlock(&mutex_workers)) {	//Check if pthread_mutex_unlocklock fail and if so print error massage and force game_over().
		printf("Failed to unlock mutex: mutex_workers!\n");
		game_over();
		return NULL;
	}
	int worker_minerals, i, flag, status;	//Define worker_minerals (the minerals that the worker currently holds), i (a counter), flag (a bool like varible used inorder to reset worker's mining cycle), main_status (the status from trylocking mutex_minerals) & others_status (the status from trylocking command_centers_minerals).
	worker_minerals = i = flag = 0;	//Inicializate worker_minerals i flag with 0.
	if (n > 5) printf("SCV good to go, sir.\n");	//If the player created that worker print the creation massage.
	while ((m < 20) && (map_minerals > 0)) {	//Loop while soldiers are less than 20 & minerals on map are more than 0.
		if (pthread_mutex_lock(&mutex_map_minerals)) {	//Check if pthread_mutex_lock fail and if so print error massage and force game_over().
			printf("Failed to lock mutex: mutex_map_minerals!\n");
			game_over();
			return NULL;
		}
		if (map_minerals >= 8) {	//If ther are minerals left on map mine 8 of them.
			printf("SVC %d is mining\n", number);	//Indicate that the worker is mining.
			map_minerals -= 8;
			worker_minerals += 8;
		} else map_minerals = 0;	//Else protect aggainst fake mining.
		if (pthread_mutex_unlock(&mutex_map_minerals)) { //Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
			printf("Failed to unlock mutex: mutex_map_minerals!\n");
			game_over();
			return NULL;
		}
		while (worker_minerals) {	//Loop while worker has minerals carried by him.
			i = 0;
			while(i < k) {	//Loop througth other commnad centers.
				status = pthread_mutex_trylock(&mutex_command_centers_minerals[i]);	//Trylock mutex_command_centers_minerals[i] and assign the result to others_status.
				if (!status) {	//If others_status is 0 that means that it can be locked.
					printf("SVC %d is transporting minerals\n", number);	//Indicate that the worker is transporting minerals.
					sleep(2);	//Sleep 2 secs so other workers can store minerals in other command centers.
					command_centers_minerals[i] += worker_minerals; //increment command_centers_minerals[i] with worker_minerals.
					if (pthread_mutex_unlock(&mutex_command_centers_minerals[i])) {	//Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
						printf("Failed to unlock mutex: mutex_command_centers_minerals %d!\n", (i + 1));
						game_over();
						return NULL;
					}
					flag = 1; //Set flag to 1 (true) to indicate successfuly transported minerals.
					printf("SVC %d delivered minerals to Command Center %d\n", number, (i + 1)); //Indicate the successful deliverment to Command Center i + 2.
					break;	
				} else {
					if (status == 16) i++;	//If others_status is 16 this means that the mutex is currently locked by other thread, so increment i 
					else {	//Else pthread_mutex_trylock fail and if so print error massage and force game_over().
						printf("Failed to tylock mutex:mutex_command_centers_minerals %d!\n", (i + 1));
						game_over();
						return NULL;
					}
				}
			} 
			if (flag) {	//If flag is setted to 1 increment collected_minerals, all_minerals & reset worker_minerals & flag.
				if (pthread_mutex_lock(&mutex_all_minerals)) {	//Check if pthread_mutex_lock fail and if so print error massage and force game_over().
					printf("Failed to lock mutex: mutex_all_minerals!\n");
					game_over();
					return NULL;
				}
				collected_minerals += worker_minerals;	//Increment collected_minerals with worker_minerals.
				all_minerals += worker_minerals;	//increment all_minerals with worker_minerals.
				if (pthread_mutex_unlock(&mutex_all_minerals)) {	//Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
					printf("Failed to unlock mutex: mutex_all_minerals!\n");
					game_over();
					return NULL;
				}
				worker_minerals = 0;	//Reset worker_minerals.
				flag = 0;	//Reset flag.
			}
			sleep(1);	//Else sleep for 1 sec and try again to deliver the minerals to some command center.
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
	if (pthread_mutex_lock(&mutex_command_centers)) {	//Check if pthread_mutex_lock fail and if so print error massage and force game_over().
		printf("Failed to lock mutex: mutex_command_centers \n");
		game_over();
		return NULL;
	}
	if (!k) {
		command_centers_minerals = (size_t*) malloc(1 * sizeof(size_t));
		if (!command_centers_minerals) {
			printf("Failed to perform malloc on a pointer\n");
			game_over();
			return NULL;
		}
	} else {
		command_centers_minerals = (size_t*) realloc(command_centers_minerals, (k + 1) * sizeof(size_t));
		if (!command_centers_minerals) {
			printf("Failed to perform realloc on a pointer\n");
			game_over();
			return NULL;
		}
	}
	if (pthread_mutex_init(&mutex_command_centers_minerals[k], NULL)) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_command_centers_minerals \n");
		game_over();
		return NULL;
	}
	k++;
	if (pthread_mutex_unlock(&mutex_command_centers)) {	//Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
		printf("Failed to unlock mutex: mutex_command_centers \n");
		game_over();
		return NULL;
	}	
	if (k - 1)
		printf("Command center %d created.\n", k);	//Indicate that new command centers was successfuly created.
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
	if (pthread_mutex_lock(&mutex_soldiers)) {	//Check if pthread_mutex_lock fail and if so print error massage and force game_over().
		printf("Failed to lock mutex: soldiers!\n");
		game_over();
		return NULL;
	}
	m++;	//Increment m.
	if (pthread_mutex_unlock(&mutex_soldiers)) {	//Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
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
	size_t i;	//Define i (counter).
	
	pthread_t* workers = (pthread_t*) malloc(5 * sizeof(pthread_t));
	if (!workers) {
		printf("Failed to perform malloc on a pointer\n");
		game_over();
		return 0;
	}
	pthread_t* soldiers = (pthread_t*) malloc(1 * sizeof(pthread_t));
	if (!soldiers) {
		printf("Failed to perform malloc on a pointer\n");
		game_over();
		return 0;
	}
	pthread_t* centers = (pthread_t*) malloc(1 * sizeof(pthread_t));
	if (!centers) {
		printf("Failed to perform malloc on a pointer\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_workers, NULL)) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_workers!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_command_centers, NULL)) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_command_center!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_soldiers, NULL)) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_soldiers!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_map_minerals, NULL)) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_map_minerals!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_init(&mutex_all_minerals, NULL)) {	//Check if pthread_mutex_init fail and if so print error massage and force game_over().
		printf("Failed to initialize mutex: mutex_all_minerals!\n");
		game_over();
		return 0;
	}
	for(i = 0; i < 5;i++) {	//Loop 5 times inorder to create the first 5 workers.
		if (pthread_create(&workers[i], NULL, worker, NULL)) {	//Check if pthread_create fail and if so print error massage and force game_over().
			printf("Failed to crate new worker!\n");
			game_over();
			break;
		}	
	}
	if (pthread_create(&centers[k], NULL, command_center, NULL)) {	//Check if pthread_create fail and if so print error massage and force game_over().
		printf("Failed to create new command_center!\n");
		game_over();
		return 0;
	}
	while(m < 20) {
		command = getchar();
		if ((command == 'm') || (command == 's') || (command == 'c')) {
			if (pthread_mutex_lock(&mutex_all_minerals)) {	//Check if pthread_mutex_lock fail and if so print error massage and force game_over().
				printf("Failed to lock mutex: mutex_all_minerals!\n");
				game_over();
				break;
			}
			if (command == 'm') {	//Check if written command is m and if so check is it possible to create new soldier.
				if ((all_minerals >= 50) && (m < 20)) {	//Check if there are enough minerals and if m is valid and if so decrement all_minerals with 50.
					all_minerals -= 50;	//Decrement all_minerals with 50.
					soldiers = (pthread_t*) realloc(soldiers, (m + 2) * sizeof(pthread_t));
					if (!soldiers) {
						printf("Failed to perform realloc on a pointer\n");
						game_over();
						break;
					}
					if (pthread_create(&soldiers[m], NULL, soldier, NULL)) {	//Check if pthread_create fail and if so print error massage and force game_over().
						printf("Failed to create new sodler!\n");
						game_over();
						break;
					}
				} else printf("Not enough minerals.\n");	//Else indicate that that there are not enough minerals rigth now.
			}
			if (command == 's') {	//Check if written command is s and if so check is it possible to create new worker.
				if ((all_minerals >= 50) && (n < 100)) {	//Check if there are enough minerals and if n is valid and if so decrement all_minerals with 50.
					all_minerals -= 50;	//Decrement all_minerals with 50.
					workers = (pthread_t*) realloc(workers, (n + 2) * sizeof(pthread_t));
					if (!workers) {
						printf("Failed to perform realloc on a pointer\n");
						game_over();
						break;
					}
					if (pthread_create(&workers[n], NULL, worker, NULL)) {	//Check if pthread_create fail and if so print error massage and force game_over().
						printf("Failed to create new worker!\n");
						game_over();
						break;
					}
				} else printf("Not enough minerals.\n");	//Else indicate that that there are not enough minerals rigth now.
			}
			if (command == 'c') {	///Check if written command is c  and if so check is it possible to create new command center.
				if ((all_minerals >= 400) && (k < 12)) {	//Check if there are enough minerals and if c is valid and if so decrement all_minerals with 400.
					all_minerals -= 400;	//Decrement all_minerals with 400.
					centers = (pthread_t*) realloc(centers, (k + 2) * sizeof(pthread_t));
					if (!centers) {
						printf("Failed to perform realloc on a pointer\n");
						game_over();
						break;
					}
					if (pthread_create(&centers[k], NULL, command_center, NULL)) {	//Check if pthread_create fail and if so print error massage and force game_over().
						printf("Failed to create new command_center!\n");
						game_over();
						break;
					}
				} else printf("Not enough minerals.\n");	//Else indicate that that there are not enough minerals rigth now.
			}
			if (pthread_mutex_unlock(&mutex_all_minerals)) {	//Check if pthread_mutex_unlock fail and if so print error massage and force game_over().
				printf("Fail to unlock mutex: minerals!\n");
				game_over();
				break;
			}
		}
		if ((!all_minerals) && (!map_minerals)) {	//IF there is no way to create 20 soldiers assign ~ to command.
			command = '~';
			break;
		}
	}
	for(i = 0; i < n;i++) {	//Loop n times and join all workers with main.
		if (pthread_join(workers[i], NULL)) {	//Check if pthread_join fail and if so print error massage and force game_over().
			printf("Failed to join main and a worker!\n");
			game_over();
			break;
		}
	}
	for(i = 0; i < m;i++) {	//Loop m times and join all soldiers with main.
		if (pthread_join(soldiers[i], NULL)) {	//Check if pthread_join fail and if so print error massage and force game_over().
			printf("Failed to join main and a sodler!\n");
			game_over();
			break;
		}
	}
	for(i = 0; i < k;i++) {	//Loop k times and join all command_centers with main and destroy mutex_command_centers_minereals[].
		if (pthread_join(centers[i], NULL)) {	//Check if pthread_join fail and if so print error massage and force game_over().
			printf("Failed to join main and a center!\n");
			game_over();
			break;
		}
		if (pthread_mutex_destroy(mutex_command_centers_minerals + i)) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
			printf("Failed to destroy mutex: mutex_command_centers_minerals[%d]!\n", i);
			game_over();
			break;
		}
	}
	if (pthread_mutex_destroy(&mutex_workers)) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
		printf("Failed to destroy mutex: mutex_workers!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_destroy(&mutex_command_centers)) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
		printf("Failed to destroy mutex: mutex_command_center!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_destroy(&mutex_soldiers)) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
		printf("Failed to destroy mutex: mutex_soldiers!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_destroy(&mutex_map_minerals)) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
		printf("Failed to destroy mutex: mutex_map_minerals!\n");
		game_over();
		return 0;
	}
	if (pthread_mutex_destroy(&mutex_all_minerals)) {	//Check if pthread_mutex_destroy fail and if so print error massage and force game_over().
		printf("Failed to destroy mutex: mutex_all_minerals!\n");
		game_over();
		return 0;
	}
	free(workers);
	free(soldiers);
	free(centers);
	free(command_centers_minerals);
	if (command == '~') printf("You Failed to create 20 soldiers Game Over!\n");	//Message that game can't be won.
	else if (!error) {
		printf("Game Won !, You successfuly created 20 soldiers\n");	//Message that game has been won.
		printf("Game started with 5000 minerals on the map, %d minerals left on map and you successfuly collected %d minerals", map_minerals, collected_minerals);	//Print starting minerals, those left on map and all collected.
	}
	return 0;
}