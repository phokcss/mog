// char_server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define BUF_SIZE 100       //메시지 버퍼 크기 설정//
#define MAX_CLNT 256       //최대 클라이언트 수 설정//

void * handle_clnt(void * arg);     //받은 메시지를 모든 클라이언트에게 보내고 클라이언트 연결 해지 하는 함수//
void send_msg(char * msg, int len);  //handle_clnt의 함수에서 메시지를 보내는 함수//
void error_handling(char * msg);       //소켓 에러 감지 함수//

int clnt_cnt=0;            // 클라이언트 수 변수//
int clnt_socks[MAX_CLNT];  //클라이언트 소켓 배열//
pthread_mutex_t mutx;      //스레드 오류 방지를 위한 뮤텍스 설정//

int main(int argc, char *argv[])
{
   int serv_sock, clnt_sock;                 //서버,클라이언트 소켓 설정//
   struct sockaddr_in serv_adr, clnt_adr;    //소켓 주소 설정 변수//
   int clnt_adr_sz;
   pthread_t t_id;
   if(argc!=2) {
      printf("Usage : %s <port>\n", argv[0]);
      exit(1);
   }
  
   pthread_mutex_init(&mutx, NULL);            //뮤택스 설정//
   serv_sock=socket(PF_INET, SOCK_STREAM, 0);         //TCP로 설정//

   memset(&serv_adr, 0, sizeof(serv_adr));            //SOCKET주소 초기화//
   serv_adr.sin_family=AF_INET;              
   serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
   serv_adr.sin_port=htons(atoi(argv[1]));            //SOCKET주소 설정//
   
   if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1) //SOCKET주소 설정//
      error_handling("bind() error");
   if(listen(serv_sock, 5)==-1)        //연결 요청 대기 큐 //
      error_handling("listen() error");
  while(1)
   {
      clnt_adr_sz=sizeof(clnt_adr);    //소켓 주소 스트럭 크기 저장//
      clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);     //클라이언트 접속 요청 수락 및 소켓 디스크립터 저장//
      
      pthread_mutex_lock(&mutx);       //뮤텍스 락을 통해 다른 스레드 움직임 정지//
      clnt_socks[clnt_cnt++]=clnt_sock;  //accept시 클라이언트 소켓 배열에 소캣 디스크랩터 넣기//
      pthread_mutex_unlock(&mutx);     //다른 스레드 락 해지//
   
      pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock); //각각 클라이언트 송수신하는 스레드 생성//
      pthread_detach(t_id);         //종료시 스레드 자원 반납//
      printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
   }
   close(serv_sock);
   return 0;
}
   
void * handle_clnt(void * arg)
{
   int clnt_sock=*((int*)arg);      //소켓 디스크립터 변수 저장//
   int str_len=0, i;    //글자 수 저장하는 변수, 반복문 변수//
   char msg[BUF_SIZE];
   
   while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)//읽을 값이 없을때까지 읽기//
   {
      send_msg(msg, str_len);  //읽은 값을 모든 클라이언트에게 전달//  
      fputs((msg), stdout);
      fputc('\n', stdout);
   }
   pthread_mutex_lock(&mutx);
   for(i=0; i<clnt_cnt; i++)   //연결 해지시//
   {
      if(clnt_sock==clnt_socks[i])  //clnt_socks배열에 현재 소켓 디스크립터와 같은 것 찾기//
      {
         while(i++<clnt_cnt-1)      //같은게 있다면 클라이언트 소켓 디스크립터 배열 정리//
            clnt_socks[i]=clnt_socks[i+1];   
         break;
      }
   }
   clnt_cnt--;       //클라이언트 수 한개 줄이기//
   pthread_mutex_unlock(&mutx);
   close(clnt_sock);
   return NULL;
}
void send_msg(char * msg, int len)   // send to all
{
   int i;
   pthread_mutex_lock(&mutx); //뮤텍스 락을 통해 다른 스레드 움직임 정지//
   for(i=0; i<clnt_cnt; i++)
      write(clnt_socks[i], msg, len); //모든 클라이언트에게 받은 메시지 송신//
   pthread_mutex_unlock(&mutx); //뮤텍스 언락을 통해 다른 스레드 움직임 정지 해지//
}
void error_handling(char * msg)
{
   fputs(msg, stderr);
   fputc('\n', stderr);
   exit(1);
}
