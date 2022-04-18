#include "monitor.h"
#include <cstdlib>
#include <time.h>
#include <vector>
#include <pthread.h>

using namespace std;

vector<int> buffer;

int nrEven=0;
int nrOdd=0;

int time_to_produce = 5;
int time_to_consume = 7;

Semaphore mutex(1), prodEvenSem(0), prodOddSem(0), consEvenSem(0), consOddSem(0);
unsigned int numOfProdEvenWaiting =0, numOfProdOddWaiting =0, numOfConsEvenWaiting =0, numOfConsOddWaiting =0;

void printBuffer(vector<int> buffer){
	printf("Bufor with size %lu:   ", buffer.size());
	for (int i=0; i< buffer.size(); i++){
			printf("   %d ",buffer[i]);
	}
	printf("\nEven: %d, 	Odd: %d", nrEven, nrOdd);
	printf("\n");
}

void update_vector(vector<int> & vtr, int value){
	vtr.push_back(value);
}

/**********************************************************************************************************************************
==============================CHECK=========================================================
**********************************************************************************************************************************/

bool canPutEven(){return nrEven < 10;} 
bool canPopEven(){return (buffer.size() >= 3) && (buffer[0]%2 == 0);} 
bool canPutOdd(){return nrEven > nrOdd;}
bool canPopOdd(){return (buffer.size() >= 7) && (buffer[0]%2 == 1);}



/**********************************************************************************************************************************
==============================PUT&POP==========================================================
**********************************************************************************************************************************/


void putEven(int value){
	mutex.p();
	
	if(!canPutEven()){
		numOfProdEvenWaiting++;
		mutex.v();
		prodEvenSem.p();
	}
	
	//buffer.push_back(value);
	update_vector(buffer, value);
	nrEven++;
	printf("Adding even number: %d  \n", value);
	printBuffer(buffer);
	
	
	if(numOfProdOddWaiting >0 && canPutOdd()){
			numOfProdOddWaiting--;
			prodOddSem.v();
	} else if (numOfConsOddWaiting >0 && canPopOdd()){
			numOfConsOddWaiting--;
			consOddSem.v();
	} else if (numOfConsEvenWaiting >0 && canPopEven()){
			numOfConsEvenWaiting--;
			consEvenSem.v();
	} else {
			mutex.v();
	}
}

void popEven(){
	mutex.p();
	
	if(!canPopEven()){
		numOfConsEvenWaiting++;
		mutex.v();
		consEvenSem.p();
	}
	
	
	printf("Deleting even number  %d  \n", buffer[0]);
	buffer.erase(buffer.begin());
	nrEven--;
	printBuffer(buffer);
	
	
	if(numOfProdOddWaiting >0 && canPutOdd()){
			numOfProdOddWaiting--;
			prodOddSem.v();
	} else if (numOfConsOddWaiting >0 && canPopOdd()){
			numOfConsOddWaiting--;
			consOddSem.v();
	} else if (numOfProdEvenWaiting >0 && canPutEven()){
			numOfProdEvenWaiting--;
			prodEvenSem.v();
	} else {
			mutex.v();
	}
}

void putOdd(int value){
	mutex.p();
	
	if(!canPutOdd()){
		numOfProdOddWaiting++;
		mutex.v();
		prodOddSem.p();
	}
	
	//buffer.push_back(value);
	update_vector(buffer, value);
	nrOdd++;
	printf("Adding odd number  %d \n", value);
	printBuffer(buffer);
	
	
	if(numOfProdEvenWaiting >0 && canPutEven()){
			numOfProdEvenWaiting--;
			prodEvenSem.v();
	} else if (numOfConsOddWaiting >0 && canPopOdd()){
			numOfConsOddWaiting--;
			consOddSem.v();
	} else if (numOfConsEvenWaiting >0 && canPopEven()){
			numOfConsEvenWaiting--;
			consEvenSem.v();
	} else {
			mutex.v();
	}
}

void popOdd(){
	mutex.p();
	
	if(!canPopOdd()){
		numOfConsOddWaiting++;
		mutex.v();
		consOddSem.p();
	}
	
	printf("Deleting odd number  %d  \n", buffer[0]);
	buffer.erase(buffer.begin());
	nrOdd--;
	printBuffer(buffer);
	
	
	if(numOfProdEvenWaiting >0 && canPutEven()){
			numOfProdEvenWaiting--;
			prodEvenSem.v();
	} else if (numOfProdOddWaiting >0 && canPutOdd()){
			numOfProdOddWaiting--;
			prodOddSem.v();
	} else if (numOfConsEvenWaiting >0 && canPopEven()){
			numOfConsEvenWaiting--;
			consEvenSem.v();
	} else {
			mutex.v();
	}
}

/**********************************************************************************************************************************
===============================PRODUCER&CONSUMER=====================================================
**********************************************************************************************************************************/

void* evenProducer(void *threadid){
	while(1){
		int evenNum = (rand()%25)*2;
		putEven(evenNum);
		sleep(time_to_produce);
	}
}
void* evenConsumer(void *threadid){
	while(1){
		popEven();
		sleep(time_to_consume);
	}
}

void* oddProducer(void *threadid){	
	while(1){
		int oddNum = ((rand()%25)*2)+1;
		putOdd(oddNum);
		sleep(time_to_produce);
	}
}
void* oddConsumer(void *threadid){
	while(1){
		popOdd();
		sleep(time_to_consume);
	}
}


/**********************************************************************************************************************************
=====================================MAIN=====================================================
**********************************************************************************************************************************/

#define NUM_THREADS 4

int main(){
	
	srand(time(NULL));
	
	pthread_t threads[NUM_THREADS];
	int rc;
	
	/*
	printf("Stworzono wątek even Producer\n");
	rc = pthread_create(&threads[0], NULL, evenProducer, (void *)0);
	/**/
	/*
	printf("Stworzono wątek odd Producer\n");
	rc = pthread_create(&threads[1], NULL, oddProducer, (void *)1);
	/**/
	/*
	printf("Stworzono wątek even Consumer\n");
	rc = pthread_create(&threads[2], NULL, evenConsumer, (void *)2);
	/**/
	/*
	printf("Stworzono wątek odd Consumer\n");
	rc = pthread_create(&threads[3], NULL, oddConsumer, (void *)3);
	/**/
	
	pthread_exit(NULL);
}
