#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>

#define REGISTER_NUMBER 5
#define CUSTOMER_NUMBER 25
#define CUSTOMER_ARRIVAL_TIME_MIN 1
#define CUSTOMER_ARRIVAL_TIME_MAX 3
#define COFFEE_TIME_MIN 2
#define COFFEE_TIME_MAX 5
//Semaphores
sem_t semCustomer, semRegister;

//Order acts as a buffer which keeps the customer ids
int order[CUSTOMER_NUMBER];
int in = 0;
int out = 0;

//Customer Function act as a producer. It produces orders for the register.
void *customerFunc(){
	//Initial arrival time
	int arrival = 1;
	while(1){
		//Semaphore runs this loop until semCustomer's value equals 0
		sem_wait(&semCustomer);
		//in value is index of data which will be added to buffer
		//in value acts as counter to determine customer ID as well
		order[in] = in;
		//sleep between each customer creation
		sleep(arrival);
		printf("CUSTOMER %d IS CREATED AFTER %d SECONDS.\n",in,arrival);
		//randomly create arrival time
		arrival = (rand() %
        (CUSTOMER_ARRIVAL_TIME_MAX - CUSTOMER_ARRIVAL_TIME_MIN + 1)) + CUSTOMER_ARRIVAL_TIME_MIN;
        	//mark the next buffer index for the next customer
		in = (in+1)%CUSTOMER_NUMBER;
		//decrease semRegister by 1 
		sem_post(&semRegister);
	}
}
//register function as a consumer. It consumes data which means coffee order
void *registerFunc(void* arg){
	while(1){
		//this semaphore locks the register if it has a customer in process
		sem_wait(&semRegister);
		//customer id is determined from buffer
		int customerID = order[out];
		//out index has incremented for the next read
		out = (out+1)%CUSTOMER_NUMBER;
		//random order time value between two numbers
		int orderTime = (rand() %
        (COFFEE_TIME_MAX - COFFEE_TIME_MIN + 1)) + COFFEE_TIME_MIN;
        	//id of register comes from main
		long int id = (long int) arg;
		printf("CUSTOMER %d GOES TO REGISTER %d\n", customerID, *(int*)id);
		//sleep
		sleep(orderTime);
		printf("CUSTOMER %d FINISHED BUYING FROM REGISTER %d AFTER %d SECONDS\n", customerID, *(int*)id, orderTime);
	}
}
int main(int argc, char* argv[])
{
	//Initializing the threads
	//1 producer which is customerFunc and 5 consumer threads which is registerFunc
	pthread_t p, c[REGISTER_NUMBER];
	//semaphore initializes
	//customer semaphore starts with customer_number and has no shared data
	sem_init(&semCustomer,0,CUSTOMER_NUMBER);
	//register semaphore is starts with 0 and has no shared data
	sem_init(&semRegister,0,0);
	
	int j;
	//Create threads for consumer ,which is register here, with predefined size
	for (j = 0; j < REGISTER_NUMBER; j++)
	{
		//a value is the id of the register
		int* a = malloc(sizeof(int));
		*a = j;
		//thread creation 
		pthread_create(&c[j],NULL,(void *)registerFunc,a);
	}
	//Create thread for producer ,which is customer here.
	pthread_create(&p,NULL,(void *)customerFunc,NULL);
	//join threads
	for (int l = 0; l < REGISTER_NUMBER; l++)
	{
		pthread_join(c[l],NULL);
	}
	pthread_join(p,NULL);
	return 0;
}
