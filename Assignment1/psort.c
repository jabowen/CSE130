/************************************************************************
 * 
 * CSE130 Winter 2021 Assignment 1
 *  
 * POSIX Shared Memory Multi-Process Merge Sort
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
#include "sys/wait.h"
#include "pthread.h"
#include "fcntl.h"   
#include "sys/mman.h"

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

/* 
 * This function stub needs to be completed
 */
void multiProcessMergeSort(int arr[], int left, int right) 
{
  // Your code goes here 
  int middle = (left+right)/2;
  int status = 0;
  int wpid;
  int size = (right+1)*sizeof(int);
  int shmid = shm_open("temp", O_CREAT|O_RDWR, 0666);
  ftruncate(shmid, size);
  int *array = (int *)mmap(0,size,PROT_WRITE|PROT_READ,MAP_SHARED,shmid,0);
  int j = 0;
  array[0] = 1;
  while(j<=right){
    array[j] = arr[j];
    j++;
  }
  int pid = fork();
  switch(pid){
    case -1:
      pthread_exit(0);
    case 0:
      singleProcessMergeSort(array, left, middle);
      munmap(array,size);
      close(shmid);
      pthread_exit(0);
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
  munmap(array,size);
  close(shmid);
  shm_unlink("temp");
}
