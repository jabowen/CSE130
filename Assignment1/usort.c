/************************************************************************
 * 
 * CSE130 Winter 2021 Assignment 1
 * 
 * UNIX Shared Memory Multi-Process Merge Sort
 * 
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ************************************************************************/

#include "merge.h"
#include "unistd.h"
#include "stdlib.h"
#include "sys/shm.h"
#include "sys/wait.h"

/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleProcessMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleProcessMergeSort(arr, left, middle); 
    singleProcessMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

void multiProcessMergeSort(int arr[], int left, int right) 
{
  int middle = (left+right)/2;
  int status = 0;
  int wpid;
  key_t key = ftok("shmseg", 65);
  int shmid = shmget(key, 1024,0666|IPC_CREAT);
  int *array = (int *)shmat(shmid,0,0);
  int j = 0;
  while(j<=right){
    array[j] = arr[j];
    j++;
  }
  int pid = fork();
  switch(pid){
    case -1:
      exit(0);
    case 0:
      singleProcessMergeSort(array, left, middle);
      exit(0);
    default:
      singleProcessMergeSort(array, middle+1, right);
      while((wpid = wait(&status))>0);
      merge(array, left, middle, right);
      j = 0;
      while(j<=right){
        arr[j] = array[j];
        j++;
      }
}
  
  shmdt((void *) array);
  shmctl(shmid, IPC_RMID, NULL);

}
