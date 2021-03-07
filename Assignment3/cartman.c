
#include "cartman.h"
#include "semaphore.h"
#include "pthread.h"

sem_t semasJ[5];
int tracks[5];
int carts[40][2];
int size = 40;
int indexs[40];

void *helper(void *args){
  int cart = *((int *)args);
  int track=carts[cart%size][0];
  int junction=carts[cart%size][1];

  while(tracks[track]>0){
    sched_yield();
  }
  tracks[(track+1)%5]++;
  tracks[(track+4)%5]++;

  
  sem_wait(&semasJ[track]);
  sem_wait(&semasJ[(track+1)%5]);

  reserve(cart, track);
  reserve(cart, (track+1)%5);

  cross(cart, track, junction);
  pthread_exit(NULL);
}

/*
 * You need to implement this function, see cartman.h for details 
 */
void arrive(unsigned int cart, enum track track, enum junction junction) 
{
  pthread_t cartman;
  carts[cart%size][0]=track;
  carts[cart%size][1]=junction;
  indexs[cart%size]=cart;
  pthread_create(&cartman, NULL, &helper, (void *)&indexs[cart%size]);
}

/*
 * You need to implement this function, see cartman.h for details 
 */
void depart(unsigned int cart, enum track track, enum junction junction) 
{
 

  tracks[(track+1)%5]--;
  tracks[(track+4)%5]--;


  //printf("\nrelease(%d,%d,%d,%d)\n", cart, track, (track+4)%5, (track+1)%5);
  release(cart, track);
  release(cart,(track+1)%5);

  sem_post(&semasJ[track]);
  sem_post(&semasJ[(track+1)%5]);
}

/*
 * You need to implement this function, see cartman.h for details 
 */
void cartman() 
{
  for(int i = 0; i<5; i++){
    sem_init(&semasJ[i],0,1);
  }
}

