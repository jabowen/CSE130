#include "merge.h"
#include "pthread.h"
#include "stdio.h"

int *array;
int ends[5];
pthread_t threads[2];


void *helper(void *place){
  int i = *((int *)place);
  int notFirst = 1;
  if(i==0){
    notFirst=0;
  }
    singleThreadedMergeSort(array, ends[i]+notFirst, ends[i+1]);
  if(i%2==0){
    printf("i=%d",i+1);
    pthread_join(threads[notFirst],NULL);
    merge(array, ends[i]+notFirst, ends[i+1], ends[i+2]);
  }
  pthread_exit(NULL);
  return NULL;
}

/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleThreadedMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleThreadedMergeSort(arr, left, middle); 
    singleThreadedMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

/* 
 * This function stub needs to be completed
 */
void multiThreadedMergeSort(int arr[], int left, int right) 
{
  // Your code goes here 
  ends[0]=left;
  for(int i = 1; i<4; i++){
    ends[i] = i*(left+right)/4; 
  }
  ends[4] = right;
  array = arr;
  pthread_t thread1;
  pthread_t thread3;
  int t1=0;
  int t2=1;
  int t3=2;
  int t4=3;
  pthread_create(&thread1, NULL, &helper, (void*)&t1);
  pthread_create(&threads[0], NULL, &helper, (void*)&t2);
  pthread_create(&thread3, NULL, &helper, (void*)&t3);
  pthread_create(&threads[1], NULL, &helper, (void*)&t4);
  pthread_join(thread1,NULL);
  pthread_join(thread3,NULL);
  merge(arr, left, ends[2], right);
}

