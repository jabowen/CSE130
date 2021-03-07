/**
 * See scheduler.h for function details. All are callbacks; i.e. the simulator 
 * calls you when something interesting happens.
 */
#include <stdlib.h>
#include <stdio.h>
#include <sys/queue.h>

#include "simulator.h"
#include "scheduler.h"

//LIST_HEAD(ready_list,__thread_t) ready;
//LIST_HEAD(waitIO_list,__thread_t) waitIO;
int size = 1024;
int statsA[1025][8];
thread_t threads[1025];
int numThreads=0;

thread_t ready[1024];
int readyStart=0;
int readyEnd=0;
thread_t waitIO[1024];
int waitStart=0;
int waitEnd=0;

int isRunning=0;

void scheduler(enum algorithm algorithm, unsigned int quantum) {
  //LIST_INIT(&ready);
  //LIST_INIT(&waitIO);
}

void sim_tick() { }

void sys_exec(thread_t *t) {
  if(!statsA[t->tid][7]){
    numThreads++;
    threads[numThreads]=*t;
    statsA[t->tid][0]=sim_time();
  }
  if(isRunning){
    ready[readyEnd]=*t;
    readyEnd=(readyEnd+1)%size;
  }else{
    isRunning=1;
    statsA[t->tid][2]=sim_time()+statsA[t->tid][7];
    sim_dispatch(t);
  }
}

void sys_read(thread_t *t) {
  waitIO[waitEnd]=*t;
  waitEnd=(waitEnd+1)%size;
  statsA[t->tid][3]=sim_time()+1;
  statsA[t->tid][7]=1;
  statsA[t->tid][5]=statsA[t->tid][2];
  isRunning=0;
  if(readyStart!=readyEnd){
    thread_t *t2=&ready[readyStart];
    statsA[t2->tid][2]=sim_time()+1;
    sim_dispatch(t2);
    readyStart=(readyStart+1)%size;
    isRunning=1;
  }

  
}

void sys_write(thread_t *t) {
  waitIO[waitEnd]=*t;
  waitEnd=(waitEnd+1)%size;
  statsA[t->tid][3]=sim_time()+1;
  statsA[t->tid][7]=1;
  statsA[t->tid][5]=statsA[t->tid][2];
  isRunning=0;
  if(readyStart!=readyEnd){
    thread_t *t2=&ready[readyStart];
    statsA[t2->tid][2]=sim_time()+1;
    sim_dispatch(t2);
    readyStart=(readyStart+1)%size;
    isRunning=1;
  }
}

void sys_exit(thread_t *t) {
  statsA[t->tid][1]=sim_time();
  isRunning=0;
  if(readyStart!=readyEnd){
    thread_t *t2=&ready[readyStart];
    statsA[t2->tid][2]=sim_time()+1;
    sim_dispatch(t2);
    readyStart=(readyStart+1)%size;
    isRunning=1;
  }
}

void io_complete(thread_t *t) {
  waitStart=(waitStart+1)%size;
  statsA[t->tid][6]=sim_time()+1;
  sys_exec(t);
}

void io_starting(thread_t *t) { 
  statsA[t->tid][4]=sim_time();
}

stats_t *stats() {
  int turnaround;
  int wait;
  int avgWait=0;
  int avgTurn=0;
  stats_t *stats=malloc(sizeof(stats_t));
  stats->tstats = malloc(sizeof(stats_t)*numThreads);
  stats->thread_count=numThreads;
  stats_t *s=stats->tstats;
  for(int i=1; i<numThreads+1; i++){
    s->tid=i;
    int first=2+statsA[i][7]*3;
    wait=(statsA[i][first]-statsA[i][0])+((statsA[i][4]-statsA[i][3])+(statsA[i][2]-statsA[i][6]))*statsA[i][7];
    turnaround = statsA[i][1]-statsA[i][0]+1;
    s->turnaround_time=turnaround;
    s->waiting_time=wait;
    s=s+1;
    avgTurn+=turnaround;
    avgWait+=wait;
    //printf("\ntid=%d, start=%d, end=%d, dispatch[2]=%d, dispatch[5]=%d, join=%d, iostart=%d, ioend=%d, turnaround=%d, wait=%d, io=%d", i, statsA[i][0], statsA[i][1], statsA[i][2], statsA[i][5],statsA[i][3],statsA[i][4], statsA[i][6], turnaround, wait, statsA[i][7]);
  }
  stats->turnaround_time=avgTurn/numThreads;
  stats->waiting_time=avgWait/numThreads;
  //printf("\ntotT=%d, totW=%d, numThreads=%d, avgT=%d, avgW=%d", avgTurn, avgWait, numThreads, stats->turnaround_time, stats->waiting_time);
  return stats;  
}
