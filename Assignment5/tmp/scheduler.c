/**
 * See scheduler.h for function details. All are callbacks; i.e. the simulator 
 * calls you when something interesting happens.
 */
#include <stdlib.h>
#include <stdio.h>
//#include <sys/queue.h>

#include "simulator.h"
#include "scheduler.h"

typedef struct {
  thread_t *t;
  int start;
  int end;
  int wait;
  int waitS;
  int disp;
  int io;
  int sorter;
} ThreadData;

typedef struct node_t {
  ThreadData *data;
  struct node_t *next;
} Node;

typedef struct {
  Node *head;
  Node *tail;
  int length;
} List;

void rr(void);
void list_add(List *list, ThreadData *data, int ordered);
ThreadData *list_get(List *list, int remove, int tid);
void list_print(List *list);

List *ready;
List *all;
int isRunning=0;
int alg;
int ord;
ThreadData *curr;
int q=0;
int preempt=0;

void scheduler(enum algorithm algorithm, unsigned int quantum) {
  ready=malloc(sizeof(List));
  all=malloc(sizeof(List));
  ready->length=0;
  all->length=0;
  alg=algorithm;
  //printf("\nalg=%d\n",alg);
  q=quantum;
  if(alg>1){
    ord=1;
    preempt=0;
    if(alg%2==1){
      preempt=1;
    }
  }else{
    ord=0;
    preempt=2;
  }
}

void sim_tick() {
  if(curr&&isRunning&&alg>5){
    curr->sorter--;
    printf("\n(%d,%d)", curr->t->tid, curr->sorter);
  }
}

void sim_ready() {
  if(ready->length>=1){
    if(isRunning==0){
      //list_print(ready);
      isRunning=1;
      ThreadData *tmp=list_get(ready,1,0);
      int time = sim_time();
      tmp->wait+=time-tmp->waitS;
      //tmp->waitS=time;
      tmp->disp=time;
      curr=tmp;
      sim_dispatch(tmp->t);
    }else if(preempt==1&&curr->sorter>ready->head->data->sorter){
      ThreadData *tmp=list_get(ready,1,0);
      int time=sim_time();
      tmp->wait+=time-tmp->waitS;
      sim_dispatch(tmp->t);
      curr->waitS=time;
      list_add(ready,curr,ord);
      tmp->disp=time;
      curr=tmp;
      //isRunning=1;
    }else{
      rr();
    }
  }
}

void sys_exec(thread_t *t) {
  ThreadData *tmp=malloc(sizeof(ThreadData));
  tmp->start=sim_time();
  tmp->t=t;
  tmp->wait=0;
  tmp->io=0;
  list_add(all,tmp,0);
  tmp->waitS=tmp->start;
  tmp->sorter=0;
  if(alg==2||alg==3){
    tmp->sorter=t->priority;
  }else if(alg>3){
    tmp->sorter=t->length;
  }
  list_add(ready,tmp,ord);
}

void sys_read(thread_t *t) {
  ThreadData *tmp=list_get(all,0,t->tid);
  curr=NULL;
  tmp->waitS=sim_time()+1;
  if(alg>5){
    tmp->sorter--;
  }
  isRunning=0;
}

void sys_write(thread_t *t) {
  ThreadData *tmp=list_get(all,0,t->tid);
  curr=NULL;
  tmp->waitS=sim_time()+1;
  if(alg>5){
    tmp->sorter--;
  }
  isRunning=0;
}

void sys_exit(thread_t *t) {
  ThreadData *tmp=list_get(all,0,t->tid);
  curr=NULL;
  tmp->end=sim_time();
  isRunning=0;
}

void io_complete(thread_t *t) {
  //list_print(all);
  //list_print(ready);
  ThreadData *tmp=list_get(all,0,t->tid);
  tmp->waitS=sim_time();
  tmp->waitS+=1;
  tmp->io=1;
  list_add(ready,tmp,ord);
}

void io_starting(thread_t *t) {
  ThreadData *tmp=list_get(all,0,t->tid);
  tmp->wait+=sim_time()-tmp->waitS;
}

stats_t *stats() { 
  int turnaround;
  int wait;
  int avgWait=0;
  int avgTurn=0;
  stats_t *stats=malloc(sizeof(stats_t));
  stats->tstats = malloc(sizeof(stats_t)*all->length);
  stats->thread_count=all->length;
  ThreadData *tmp;
  stats_t *s=stats->tstats;
  for(int i=1; i<all->length+1; i++){
    s->tid=i;
    //int first=2+statsA[i][7]*3;
    //wait=(statsA[i][first]-statsA[i][0])+((statsA[i][4]-statsA[i][3])+(statsA[i][2]-statsA[i][6]))*statsA[i][7];
    tmp=list_get(all,0,i);
    wait=tmp->wait;
    turnaround = tmp->end-tmp->start+1;
    s->turnaround_time=turnaround;
    s->waiting_time=wait;
    s=s+1;
    avgTurn+=turnaround;
    avgWait+=wait;
  }
  stats->turnaround_time=avgTurn/all->length;
  stats->waiting_time=avgWait/all->length; 
  return stats;
}

void rr(void){
  int time=sim_time();
  if(alg==1&&isRunning==1&&((time-curr->disp)%q==0)&&ready->length>0){
    ThreadData *tmp=list_get(ready,1,0);
    tmp->wait+=time-tmp->waitS;
    sim_dispatch(tmp->t);
    curr->waitS=time;
    list_add(ready,curr,ord);
    tmp->disp=time;
    curr=tmp;
      //list_print(ready);
  }
}

void list_add(List *list, ThreadData *data, int ordered){
  Node *node=malloc(sizeof(Node));
  node->data=data;
  if(list->length==0){
    //printf("hello");
    list->head=node;
    list->tail=node;
  }else{
    if(ordered){
      Node *current = list->head;
      Node *prev = list->head;
      if(data->sorter<current->data->sorter){
        list->head=node;
        node->next=current;
      }else{
        int i=0;
        while(i<list->length&&(current->data->sorter<=data->sorter)){
          i++;
          prev=current;
          current=current->next;
        }
        if(i<list->length){
          prev->next = node;
          node->next=current;
        }else{
          list->tail->next=node;
          list->tail=node;
        }
      }
    }else{
      list->tail->next=node;
      list->tail=node;
    }
  }
  list->length++;
}


ThreadData *list_get(List *list, int remove, int tid){
  if(list->length == 0){
    return NULL;
  }
  Node *node=list->head;
  Node *prevNode = list->head;
  //void *vdata = node->data;
  ThreadData *data = node->data;
  if(tid){
    //printf("\n(%d,%d,%d)", numThreads, tid, data->t->tid);
    //printf("\n%d\n",tid);
    while(data->t->tid!=tid){
      prevNode=node;
      node=node->next;
      if(node==NULL){
        return NULL;
      }
      data=node->data;
      //printf("\n(%d, %d, %d)", tid, data->t->tid, ttid);
    }
  }
  if(remove){
    if(node->data->t->tid==list->head->data->t->tid){
      list->head=node->next;
    }
    prevNode->next = node->next;
    list->length--;
    if(node->next==NULL){
      list->tail=prevNode;
    }
    free(node);
  }
  return data;
}

void list_print(List *list){
  printf("\nList Stats:");
  printf("\nlength=%d\n",list->length);
  Node *node=list->head;
  for(int i=0; i<list->length; i++){
    printf("(%d,%d),",node->data->t->tid, node->data->sorter);
    node=node->next;
  }
  printf("\n");
}
