#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#define SIZE 1024


struct arg_struct{
    FILE* stream;
    char line[SIZE];
};

const char* getEmails(char* line)
{
    int num=3; // because emails are the 3rd field in the csv file
    const char* tok;
    for (tok = strtok(line, ","); // strtok splits the string by the given delimiter ","
            tok && *tok;
            tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}

const char* getFirstName(char* line)
{
    int num=1; // because first names are the 1st field in the csv file
    const char* tok;
    for (tok = strtok(line, ","); // strtok splits the string by the given delimiter ","
            tok && *tok;
            tok = strtok(NULL, ",\n"))
    {
        if (!--num)
            return tok;
    }
    return NULL;
}



  void* printAllEmails(void *arguments){

    struct arg_struct *args = (struct arg_struct *)arguments;
      while(fgets(args->line,1024,args->stream))
      {
        char *tmp = strdup(args->line);
        printf("%s\n",getEmails(tmp));

        free(tmp);
      }
      return NULL;

  }

  void* printAllFirstNames(void *arguments){
    struct arg_struct *args = (struct arg_struct *)arguments;
      while(fgets(args->line,1024,args->stream))
      {
        char *tmp = strdup(args->line);
        printf("%s",getFirstName(tmp));
                        printf("                  [second thread]\n");

        free(tmp);
      }
      return NULL;
  }
 

void write_file(int sockfd){
  int n;
  FILE *fp;
  char *filename = "recievedfile.csv";
  char buffer[SIZE];

  fp = fopen(filename, "w");
  while (1) {
    n = recv(sockfd, buffer, SIZE, 0);
    if (n <= 0){
      break;
      return;
    }
    fprintf(fp, "%s", buffer);
    bzero(buffer, SIZE);
  }
  fclose(fp);
  return;
}


int main(){
  char *ip = "127.0.0.1";
  int port = 8080;
  int e;
  struct arg_struct arguments,arguments2;
  int sockfd, new_sock;
  struct sockaddr_in server_addr, new_addr;
  socklen_t addr_size;
  char buffer[SIZE];
  pthread_t newthread; 

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("Error in socket");
    exit(1);
  }
  printf("Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e < 0) {
    perror("Error in bind");
    exit(1);
  }
  printf("Binding successfull.\n");

  if(listen(sockfd, 10) == 0){
 printf("Listening....\n");
 }else{
 perror("Error in listening");
    exit(1);
 }

  addr_size = sizeof(new_addr);
  new_sock = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
  write_file(new_sock);
  printf("Data retrieved successfully.\n\n\n");

  // multithreaded parsing
 arguments.stream=fopen("recievedfile.csv","r");
  arguments2.stream=fopen("recievedfile.csv","r");
  pthread_create(&newthread,NULL,&printAllFirstNames,(void *)&arguments);
  printAllEmails((void *)&arguments2);


  return 0;
}