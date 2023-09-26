//chat_clnt.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <semaphore.h>
#define BUF_SIZE 1024
void * send_msg(void * arg);
void * recv_msg(void * arg);
void error_handling(char * msg);
static sem_t sem_one;
static sem_t sem_two;
static int str_len=20;     //메시지 저장 변수//
static char user;          //역할 저장 변수//
char msg[BUF_SIZE];        //fget함수 사용 변수//
int main(int argc, char *argv[])
{
   int sock;                     
   struct sockaddr_in serv_addr;       //소켓 주소 설정 구조체//
   pthread_t snd_thread, rcv_thread;   //스레드 설정 변수//
   void * thread_return;
   char name_msg[300];
   int recv_len,recv_cnt;
   if(argc!=3) {
      printf("Usage : %s <IP> <port>\n", argv[0]);
      exit(1);
    }   
   sock=socket(PF_INET, SOCK_STREAM, 0);      //tcp로 설정//
   
   memset(&serv_addr, 0, sizeof(serv_addr));       //소켓 주소 초기화//
   serv_addr.sin_family=AF_INET;                   //소켓 주소 설정//
   serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
   serv_addr.sin_port=htons(atoi(argv[2]));        //소켓 주소 설정//
     
   if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1)
      error_handling("connect() error");
   while((str_len=read(sock, &name_msg[recv_len], 11))!=0)        //역할 배정 메시지 수신//
   while(1)
   {
      printf("receive:%s\n",name_msg);
      if(name_msg[5]=='b')    //블랙//
      {
         user='b';
         sem_init(&sem_one,0,0);    //sem_one 0, sem_two 1로 초기화//
         sem_init(&sem_two,0,1);
         pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);    //메시지 송신 스레드 생성//
         pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);    //메시지 수신 스레드 생성//
         printf("receive:%s\n","your_turn");
         pthread_join(snd_thread, &thread_return);    //send스레드 종료 기다림//
         pthread_join(rcv_thread, &thread_return);    //recv스레드 종료 기다림//
         
         break;
      }
      else if(name_msg[5]=='w')  //화이트//
      {
         user='w';
         sem_init(&sem_one,0,1);       //sem_one 1, sem_two 0로 초기화//
         sem_init(&sem_two,0,0);
         pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);
         pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);
         pthread_join(snd_thread, &thread_return);
         pthread_join(rcv_thread, &thread_return); 
         
         break;
      }
      else 
         continue;
   }
   close(sock);  
   sem_destroy(&sem_one);
   sem_destroy(&sem_two);
   return 0;

}
   
void * send_msg(void * arg)   // send thread main
{
   int sock=*((int*)arg);              //소켓 디스크립터 변수 저장//
   char name_msg[BUF_SIZE];         //보내는 매시지 배열 선언//
   while(1) 
   {
      sem_wait(&sem_two);     //sem_two-1하고 sem_two가 음수면 block//
      for (int i=0;i<2; i++)
      {
         if(i==0)          //x좌표 받기//
         {
            printf("x:");
            fgets(msg, BUF_SIZE, stdin);
            if(atoi(msg)==0)
               continue;      
         }
         else
         {                 //y좌표 받기//
            printf("y:");
            fgets(msg, BUF_SIZE, stdin);
            if(atoi(msg)==0)
               continue;
         }
         if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n")) 
         {
            close(sock);
            exit(0);
         }
         str_len=write(sock, msg, strlen(msg));
         
         
      }
      sem_post(&sem_one);        //sem_one+1//
      
   }
   return NULL;
}
   
void * recv_msg(void * arg)   // read thread main
{
   
   int recv_len,recv_cnt;        //총 메시지 길이,한번에 받는 메시지 길이 재는 변수//
   int sock=*((int*)arg);           //소켓 디스크립터 변수 저장//
   char name_msg[BUF_SIZE];
   
   
   char s[10]="your_turn";
   while(1)
   {
      sem_wait(&sem_one);  //sem_one-1하고 sem_one가 음수면 block//
      for (int i=0;i<1; i++)
      {
         recv_len=0;
         while(recv_len<str_len)
         {
            recv_cnt=read(sock, &name_msg[recv_len], 10);
            if(strcmp(s,name_msg)==0)
            {  
               sem_post(&sem_two);  //sem_two+1//
               printf("receive:%s\n",name_msg);
               
            }
         if(recv_cnt==-1) 
            return (void*)-1;
         recv_len+=recv_cnt;
         }
         name_msg[recv_len]=0;
      }   
   }
   return NULL;
}
   
void error_handling(char *msg)
{
   fputs(msg, stderr);
   fputc('\n', stderr);
   exit(1);
}
