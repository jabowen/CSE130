
/*********************************************************************
 *
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/

#include "manpage.h"
#include "pthread.h"
#include "semaphore.h"

int current = 0;
sem_t mutex;


void *show(void* hello){
  int pid = getParagraphId();
  while(pid!=current);
  
  sem_wait(&mutex);
  showParagraph();
  current++;
  sem_post(&mutex);

  pthread_exit(NULL);
}

/*
 * See manpage.h for details.
 *
 * As supplied, shows random single messages.
 */

void manpage() 
{
  pthread_t threads[7];
  sem_init(&mutex, 0, 1);

  pthread_create(&threads[0], NULL, &show, NULL);
  pthread_create(&threads[1], NULL, &show, NULL);
  pthread_create(&threads[2], NULL, &show, NULL);
  pthread_create(&threads[3], NULL, &show, NULL);
  pthread_create(&threads[4], NULL, &show, NULL);
  pthread_create(&threads[5], NULL, &show, NULL);
  pthread_create(&threads[6], NULL, &show, NULL);

  pthread_join(threads[0],NULL);
  pthread_join(threads[1],NULL);
  pthread_join(threads[2],NULL);
  pthread_join(threads[3],NULL);
  pthread_join(threads[4],NULL);
  pthread_join(threads[5],NULL);
  pthread_join(threads[6],NULL);

}
