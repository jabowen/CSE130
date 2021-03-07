/*********************************************************************
 *
 * Copyright (C) 2020-2021 David C. Harrison. All right reserved.
 *
 * You may not use, distribute, publish, or modify this code without 
 * the express written permission of the copyright holder.
 *
 ***********************************************************************/

#include "fileman.h"
#include <fcntl.h>  
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void dir_rec(char *dname, int level, char *buf);
void tree_rec(char *dname, int level, char *buf);

/*
 * You need to implement this function, see fileman.h for details 
 */
int fileman_read(char *fname, size_t foffset, char *buf, size_t boffset, size_t size) {
  int file = open(fname, O_RDONLY);
  if(file==-1){
    return -1;
  }
  char tbuf[size+foffset];
  read(file,tbuf,size+foffset);
  for(int i=0; i<size-0.0; i++){
    buf[boffset+i]=tbuf[foffset+i];
  }

  close(file);
  return size;
}

/*
 * You need to implement this function, see fileman.h for details 
 */
int fileman_write(char *fname, size_t foffset, char *buf, size_t boffset, size_t size) {
  if(access(fname,F_OK)==0){
    return -1;
  }
  int file=creat(fname,S_IRWXU);
  if(file==-1){
    return -1;
  }
  char tbuf[size+foffset];
  for(int i=0; i<size+foffset+0.0; i++){
    if(i<(foffset+0.0)){
      tbuf[i]='\0';
    }else{
      tbuf[i]=buf[boffset+(i-foffset)];
    }
  }
  write(file,tbuf,size+foffset);
  close(file);
  return size+foffset;
}

/*
 * You need to implement this function, see fileman.h for details 
 */
int fileman_append(char *fname, char *buf, size_t size) {
  int file = open(fname, O_WRONLY | O_APPEND);
  if(file==-1){
    return -1;
  }
  
  write(file,buf,size);
  close(file);
  return size;
}

/*
 * You need to implement this function, see fileman.h for details 
 */
int fileman_copy(char *fsrc, char *fdest) {
  int fileS = open(fsrc, O_RDWR);
  if(fileS==-1){
    return -1;
  }
  int fileD = creat(fdest,S_IRWXU);
  if(fileD==-1){
    return -1;
  }
  struct stat st;
  int tmp = stat(fsrc, &st);
  if (tmp!=0){
    return -1;
  }
  long size=st.st_size;
  char buf[size];
  read(fileS,buf,size);
  
  write(fileD,buf,size);
  close(fileS);
  close(fileD);
  return size;

}

/*
 * You need to implement this function, see fileman.h for details 
 */
void fileman_dir(int fd, char *dname) {
  char buf[strlen(dname)+1];
  char *spaces="";
  sprintf(buf,"%s\n",dname);
  int file=creat("dir.out",S_IRWXU);
  write(file,buf,strlen(buf));
  dir_rec(dname,1,spaces);
  close(file);

  struct stat st;
  int tmp = stat("dir.out", &st);
  if (tmp!=0){
    return;
  }
  file = open("dir.out", O_RDONLY);
  long size=st.st_size;
  char buffer[size];
  read(file,buffer,size);

  write(fd,buffer,size);
  close(file);
  //remove("dir.out");
  //printf("remove");
}

//this fucntion is based on https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
void dir_rec(char *dname, int level, char *buf){
  DIR *d=opendir(dname);
  int file=open("dir.out", O_WRONLY | O_APPEND);
  if(d==NULL||file<0){
    //printf("fail(%d,%s)",level,dname);
    return;
  }
  char spaces[255];
  sprintf(spaces,"    %s",buf);
  int dirS=0;
  struct dirent *dirs[255];
  struct dirent *dir;
  while((dir=readdir(d))!=NULL){
    dirs[dirS]=dir;
    dirS++;
  }
  char b[255];
  for(int i=0; i<dirS;i++){
    dir=dirs[i];
    for(int j=i+1; j<dirS; j++){
      if(strcmp(dir->d_name,dirs[j]->d_name)>0){
        struct dirent *temp=dirs[j];
        dirs[j]=dir;
        dir=temp;
      }
    }
    if(dir->d_type!=DT_DIR){
      sprintf(b,"%s%s\n",spaces,dir->d_name);
      write(file,b,strlen(b));
    }else if(dir->d_type==DT_DIR && strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0 ){
      sprintf(b,"%s%s\n",spaces,dir->d_name);
      write(file,b,strlen(b));
      char d_path[255];
      sprintf(d_path,"%s/%s",dname,dir->d_name);
      dir_rec(d_path,level+1,spaces);
    }
  }
  closedir(d);
  close(file);
}

/*
 * You need to implement this function, see fileman.h for details 
 */
void fileman_tree(int fd, char *dname) {
  char buf[strlen(dname)+1];
  char *spaces="";
  sprintf(buf,"%s\n",dname);
  int file=creat("t.out",S_IRWXU);
  write(file,buf,strlen(buf));
  tree_rec(dname,1,spaces);
  close(file);

  struct stat st;
  int tmp = stat("t.out", &st);
  if (tmp!=0){
    return;
  }
  file = open("t.out", O_RDONLY);
  long size=st.st_size;
  char buffer[size];
  read(file,buffer,size);

  write(fd,buffer,size);
  close(file);
}

//this fucntion is based on https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
void tree_rec(char *dname, int level, char *buf){
  DIR *d=opendir(dname);
  int file=open("t.out", O_WRONLY | O_APPEND);
  if(d==NULL||file<0){
    //printf("fail(%d,%s)",level,dname);
    return;
  }
  char spaces[255];
  char spaces2[255];
  int dirS=0;
  struct dirent *dirs[255];
  struct dirent *dir;
  while((dir=readdir(d))!=NULL){
    dirs[dirS]=dir;
    dirS++;
  }
  char b[255];
  for(int i=0; i<dirS;i++){
    dir=dirs[i];
    for(int j=i+1; j<dirS; j++){
      if(strcmp(dir->d_name,dirs[j]->d_name)>0){
        struct dirent *temp=dirs[j];
        dirs[j]=dir;
        dir=temp;
      }
    }
    if(i==dirS-1){
      sprintf(spaces,"%s\u2514\u2500\u2500 ",buf);
      sprintf(spaces2,"%s    ",buf);
    }else{
      sprintf(spaces,"%s\u251c\u2500\u2500 ",buf);
      sprintf(spaces2,"%s\u2502   ",buf);
    }
    if(dir->d_type!=DT_DIR){
      sprintf(b,"%s%s\n",spaces,dir->d_name);
      write(file,b,strlen(b));
    }else if(dir->d_type==DT_DIR && strcmp(dir->d_name,".")!=0 && strcmp(dir->d_name,"..")!=0 ){
      sprintf(b,"%s%s\n",spaces,dir->d_name);
      write(file,b,strlen(b));
      char d_path[255];
      sprintf(d_path,"%s/%s",dname,dir->d_name);
      tree_rec(d_path,level+1,spaces2);
    }
  }
  closedir(d);
  close(file);
}

