/**
*  @author: Travis Mayer
*     ID: 002273275
*     Email: mayer129@mail.chapman.edu
*     Course: CPSC 353-01
*     Sources:    
*       Provided course material
*       Matt Raymond 
*
*/
#include <unistd.h>     
#include <sys/types.h>  
#include <errno.h>      
#include <stdio.h>      
#include <stdlib.h>     
#include <pthread.h>    
#include <string.h>     
#include <semaphore.h> 

void *producer(void *param);
void *consumer(void *param);
char* mem;
int memsize;
int numBlocks;
int ntimes;

pthread_mutex_t work_mutex;
sem_t sem[2];

#define SIZE 32

int main(int argc, char** argv)
{
    // checking basic args like greater than 0, divisible by 32, less than 64k.
    if (argc != 3) {
	    fprintf(stderr,"usage: ./prodcon <memsize> <ntimes>\n");
	    return -1;
    }
    
    if (atoi(argv[1]) < 0) {
	    fprintf(stderr,"Argument %d must be non-negative\n",atoi(argv[1]));
	    return -1;
    }
    else if (atoi(argv[2]) < 0) {
	    fprintf(stderr,"Argument %d must be non-negative\n",atoi(argv[2]));
	    return -1;
    }
    else if (atoi(argv[1]) > 64000) {
        fprintf(stderr,"Argument %d must be less than 64k\n",atoi(argv[1]));
	    return -1;
    }
    else if (atoi(argv[1]) % SIZE != 0) {
        fprintf(stderr,"Argument %d must be divisible by 32\n",atoi(argv[1]));
	    return -1;
    }
    // initialize variables
    memsize = atoi(argv[1]);
    numBlocks = memsize / SIZE;
    ntimes = atoi(argv[2]);
    mem = malloc(memsize);
    // initialize sem
    if ((sem_init(&sem[0], 0, 1) != 0) || (sem_init(&sem[1], 0, 0) != 0)) {
        perror("Semaphore initialization failed");
        exit(EXIT_FAILURE);
    }
    // create mutex
    pthread_mutex_init(&work_mutex, NULL);
    pthread_t tid[2];
    // create threads
    pthread_create(&tid[0], NULL, producer, NULL);
    pthread_create(&tid[1], NULL, consumer, NULL);
    // join threads
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    // destroy sem
    sem_destroy(&sem[0]);
    sem_destroy(&sem[1]);
    free(mem);
    pthread_mutex_destroy(&work_mutex);
    
    return 0;
    
}


void *producer(void *param) {
    int in;
    for (int i = 0; i < ntimes; i++) { // iterate ntimes
        sem_wait(&sem[0]);
        for (int j = 0; j < numBlocks; j++) { // iterate numBlocks
            pthread_mutex_lock(&work_mutex);
            
            unsigned short int cksum = 0;
            for (in = (j * SIZE); in < (((j + 1) * SIZE) - 2); in++) { // produce data
                mem[in] = rand() % 255;
                cksum += mem[in];
            }
            ((unsigned short int *) mem)[(in + 1) / 2] = cksum; // add cksum
            pthread_mutex_unlock(&work_mutex);
            sem_post(&sem[1]);
            
        }
    }
    pthread_exit(0);
}

void *consumer(void *param) {
    int in;
    for (int i = 0; i < ntimes; i++) { // iterate ntimes
        for (int j = 0; j < numBlocks; j++) { // iterate numBlocks
            sem_wait(&sem[1]);
            
            pthread_mutex_lock(&work_mutex);
            
            unsigned short int cksum = 0;
            for (in = (j * SIZE); in < (((j + 1) * SIZE) - 2); in++) { // consume data
                cksum += mem[in];
            }
            int pull_cksum = ((unsigned short int *) mem)[(in + 1) / 2]; // compare cksums
            if (pull_cksum != cksum) {
                fprintf(stderr,"Checksums don't match.\n%d\n%d\n", cksum, pull_cksum);
	            return -1;
            }
            pthread_mutex_unlock(&work_mutex);
        }
        sem_post(&sem[0]);
    }
    pthread_exit(0);
}


