// char_server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define _CRT_SECURE_NOWARNINGS
#define BUF_SIZE 100    //메시지 버퍼 크기 설정//
#define MAX_CLNT 256    //최대 클라이언트 수 설정//
#define BOARD_SIZE 23   //19x19바둑판 에러 방지를 위해 4를 더한 값//
void * handle_clnt(void * arg);
void board_set(char (*arr_board)[23]);
void board_mornitor(char (*arr_board)[23]);
void error_handling(char * msg);
int victory_detect(char (*arr_board)[23],char color);
int double_three_detect(char (*arr_board)[23],int x, int y,char color);
char arr_board[23][23]; //보드 배열 선언//
static char next_turn='b';  //첫번째 턴 black설정//
int clnt_cnt=0;            // 클라이언트 수//
int clnt_socks[MAX_CLNT];  //클라이언트 소켓 배열//
pthread_mutex_t mutx;       //스레드 오류 방지를 위한 뮤텍스 설정//

int main(int argc, char *argv[])
{
   int serv_sock, clnt_sock;                    //서버,클라이언트 소켓 설정//
   struct sockaddr_in serv_adr, clnt_adr;       //소켓 주소 설정 변수//
   int clnt_adr_sz;
   pthread_t t_id;                       //스레드 설정 변수//
   if(argc!=2) {
      printf("Usage : %s <port>\n", argv[0]);
      exit(1);
   }
   pthread_mutex_init(&mutx, NULL);         //뮤택스 설정//
   serv_sock=socket(PF_INET, SOCK_STREAM, 0);       //TCP로 설정//

   memset(&serv_adr, 0, sizeof(serv_adr));       //SOCKET주소 초기화//
   serv_adr.sin_family=AF_INET; 
   serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
   serv_adr.sin_port=htons(atoi(argv[1]));      //SOCKET주소 설정//
   
   if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr))==-1)//SOCKET주소 할당//
      error_handling("bind() error");
   if(listen(serv_sock, 5)==-1)
      error_handling("listen() error");
  while(1)
   {
      clnt_adr_sz=sizeof(clnt_adr);     //소켓 주소 스트럭 크기 저장//
      clnt_sock=accept(serv_sock, (struct sockaddr*)&clnt_adr,&clnt_adr_sz);    //클라이언트 접속 요청 수락 및 소켓 디스크립터 저장//
      
      pthread_mutex_lock(&mutx);        //뮤텍스 락을 통해 다른 스레드 움직임 정지//
      clnt_socks[clnt_cnt++]=clnt_sock; //accept시 클라이언트 소켓 배열에 소캣 디스크랩터 넣기//
      pthread_mutex_unlock(&mutx);      //다른 스레드 락 해지//
   
      pthread_create(&t_id, NULL, handle_clnt, (void*)&clnt_sock);  //각각 클라이언트 송수신하는 스레드 생성//
      pthread_detach(t_id);         //종료시 스레드 자원 반납//
      printf("Connected client IP: %s \n", inet_ntoa(clnt_adr.sin_addr));
      
   }
   close(serv_sock);
   return 0;
}
   
void * handle_clnt(void * arg)
{
   int x,y;  
   int clnt_sock=*((int*)arg);      //소켓 디스크립터 변수 저장//
   int str_len=0, i;                //글자 수 저장하는 변수, 반복문 변수//
   char msg[BUF_SIZE];

   int x_y_count=0;           //x,y중 어디에 넣어야 하는지 카운팅 하는 변수//
   char user;
   board_set(arr_board);    //보드 초기화//
   board_mornitor(arr_board);//보드 보여주는 함수//
   printf("_____________________________________black_____________________________________\n");
   if(clnt_sock==clnt_socks[0])
      {
         pthread_mutex_lock(&mutx);             //뮤텍스 락을 통해 다른 스레드 움직임 정지//
         write(clnt_socks[0], "your_black", 11);//첫번째로 연결된 클라이언트 블랙 역할 배정//
         user='b';
         pthread_mutex_unlock(&mutx);     
         
      }
     else if(clnt_sock==clnt_socks[1])
      {
         pthread_mutex_lock(&mutx);             //뮤텍스 락을 통해 다른 스레드 움직임 정지//
         write(clnt_socks[1], "your_white", 11);//두번째로 연결된 클라이언트 화이트 역할 배정//
         user='w';
         pthread_mutex_unlock(&mutx);     
      }
   while((str_len=read(clnt_sock, msg, sizeof(msg)))!=0)        //client 메시지 읽기//
   {
        char read_char[100];
        strcpy(read_char,msg);
        int int_char=atoi(read_char);    
        
        if (user=='b')  
            printf("black_");
        else if (user=='w')  
            printf("white_");
        else
            printf("o");
        if(x_y_count==0)            //순서가x좌표일때//
        {
            printf("x:%d",int_char);
            x_y_count=1;
            x=int_char;
        }
        else if(x_y_count==1)       //순서가y좌표일때//
        {
         printf("y:%d",int_char);
         
         x_y_count=0;
         y=int_char;
         printf("\n");
         if((arr_board[x+1][y+1]==('x'))||(arr_board[x+1][y+1]==('o'))) //돌을 놓을려는 곳이 이미 보드에 값이 있을시//
        {
            printf("exist\n");
            if(next_turn=='b')
               write(clnt_socks[0], "your_turn", 10);       //블랙 client에 "your_turn" write//
            else
               write(clnt_socks[1], "your_turn", 10);       //화이트 client에 "your_turn" write//
            continue;
        }
        else if (((x>0)&&(x<20))&&((y>0)&&(y<20)))          //19x19범위에 맞는지 확인//
        {
            printf("right range\n");
        }
        else 
        {
            printf("not right range or type error\n");      //범위를 벗어났을시//
            if(next_turn=='b')
               write(clnt_socks[0], "your_turn", 10);       //블랙 client에 "your_turn" write//
            else
               write(clnt_socks[1], "your_turn", 10);       //화이트 client에 "your_turn" write//
            continue;
        }
         if(next_turn=='b')
         {
            if((double_three_detect(arr_board, x+1,y+1,'x')==1)||(double_three_detect(arr_board, x+1,y+1,'o')==1))      //33감지//
            {
                printf("double Three\n");
                write(clnt_socks[0], "your_turn", 10);      //블랙 client에 "your_turn" write//
                continue;
            }
            pthread_mutex_lock(&mutx);                  //뮤텍스 락을 통해 다른 스레드 움직임 정지//
            arr_board[x+1][y+1]='x';        //보드에 값 저장//
            if(victory_detect(arr_board,'x')==1)
            {
               printf("black_win\n");
               break;
            }
            board_mornitor(arr_board);
            printf("_____________________________________white_____________________________________\n");
            write(clnt_socks[1], "your_turn", 10);
            next_turn='w';
            pthread_mutex_unlock(&mutx);                //뮤텍스 언락을 통해 다른 스레드 움직임 정지 해지//

         }
         else if(next_turn=='w')
         {
            if((double_three_detect(arr_board, x+1,y+1,'x')==1)||(double_three_detect(arr_board, x+1,y+1,'o')==1))
            {
                printf("double Three\n");
                write(clnt_socks[1], "your_turn", 10);      //화이트 client에 "your_turn" write//
                continue;
            }
            pthread_mutex_lock(&mutx);
            arr_board[x+1][y+1]='o';            //보드에 값 저장//
            if(victory_detect(arr_board,'o')==1)
            {
               printf("white_win\n");
               break;
            }
            board_mornitor(arr_board);
            printf("_____________________________________black_____________________________________\n");

            write(clnt_socks[0], "your_turn", 10);      //블랙 client에 "your_turn" write//
            next_turn='b';
            pthread_mutex_unlock(&mutx);
         }
         else
         {
            printf("error");
            break;
         }
         printf("\n");
      }
      else
         printf("count_error");

   
      fputc('\n', stdout);
   }
   
   pthread_mutex_lock(&mutx);
   for(i=0; i<clnt_cnt; i++)   // remove disconnected client
   {
      if(clnt_sock==clnt_socks[i])
      {
         while(i++<clnt_cnt-1)
            clnt_socks[i]=clnt_socks[i+1];
         break;
      }
   }
   clnt_cnt--;
   pthread_mutex_unlock(&mutx);
   close(clnt_sock);
   return NULL;
}

void error_handling(char * msg)
{
   fputs(msg, stderr);
   fputc('\n', stderr);
   exit(1);
}

void board_mornitor(char (*arr_board)[23])
{
    printf("     1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19\n");
	printf("   ----------------------------------------------------------------------------\n");
	for (int i = 2; i < BOARD_SIZE-2; i++)
	{
		if (i < 11)//Y좌표 위치를 보여줌
			printf("%d  ", i - 1);
		else
			printf("%d ", i - 1);

		for (int n = 2; n < BOARD_SIZE-2; n++)//현제 오목의 상태를 보여줌
		{
			printf("| %c ", arr_board[i][n]);
		}
		printf("|");
        printf("  %d\n", i - 1);
		printf("   ----------------------------------------------------------------------------\n");
	}
    printf("     1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19\n");
}

void board_set(char (*arr_board)[23])
{

for (int i = 0; i < BOARD_SIZE; i++)
        {
                {
                       for (int j = 0; j < BOARD_SIZE; j++)
                        {   if((i>1)&&(j>1)&&(i<21)&&(j<21))
                                arr_board[i][j]=' ';    //공백//
                            else
                                arr_board[i][j]='n';    //벽//
                        }
                }       
        }
}

int victory_detect(char (*arr_board)[23],char color)    //오목 발생 탐지 함수//
{
    for(int i=2; i<21;i++)      //인덱스 에러 방지를 위해 2+19+2임으로 2~21로 범위//
    {
        for(int j=2; j<21;j++)
        {   
            if(arr_board[i][j]==color)//지금 들어간 값이 순서가 맞는지 확인하는 에러방지//
            {
                if((arr_board[i][j]==color)&&(arr_board[i][j+1]==color)&&(arr_board[i][j+2]==color)&&(arr_board[i][j+3]==color)&&(arr_board[i][j+4]==color))//연속된 값이5개 찾기//
                {   
                    if((arr_board[i][j-1]!=color)&&(arr_board[i][j+5]!=color))  //6 detect//
                        {
                            printf("w\n");
                            return 1;
                        }
                }
                if((((arr_board[i][j])==color)&&(arr_board[i+1][j]==color)&&(arr_board[i+2][j]==color)&&(arr_board[i+3][j]==color)&&(arr_board[i+4][j]==color)))//연속된 값이5개 찾기//
                {    
                    if((arr_board[i-1][j]!=color)&&(arr_board[i+5][j]!=color))  //6 detect//
                        {
                            printf("h\n");
                            return 1;
                        }
                }
            
                
                if(((arr_board[i][j]==color)&&(arr_board[i+1][j+1]==color)&&(arr_board[i+2][j+2]==color)&&(arr_board[i+3][j+3]==color)&&(arr_board[i+4][j+4]==color)))//연속된 값이5개 찾기//
                {
                    
                    if((arr_board[i-1][j-1]!=color)&&(arr_board[i+5][j+5]!=color))  //6 detect//
                        {
                            printf("do\n");
                            return 1;
                        }
                    return 1;
                }
                if((arr_board[i][j]==color)&&(arr_board[i+1][j-1]==color)&&(arr_board[i+2][j-2]==color)&&(arr_board[i+3][j-3]==color)&&(arr_board[i+4][j-4]==color))//연속된 값이5개 찾기//
                {
                    if((arr_board[i-1][j+1]!=color)&&(arr_board[i+5][j-5]!=color))  //6 detect//
                        {
                            printf("up\n");
                            return 1;
                        }
                }
            }
        }
    }
return 0;   
}
int double_three_detect(char (*arr_board)[23],int x, int y,char color)
{
    char reverse_color;//현재 반대 순서 저장 변수//
    if(color=='x')
        reverse_color='o';
    else
        reverse_color='x';    
    int count=0;    //연속된 3이 몇개 발생하는지 세는 변수//

//height                                                        //_=공백   ○=들어가있는 값   ●=들어갈 값   X=벽 혹은 반대 순서 값//
    if(((arr_board[x-1][y]==color)&&(arr_board[x+1][y]==color)  //_○●○_//
    &&(arr_board[x-2][y]!='n')&&(arr_board[x+2][y]!='n')                    //_에 들어간 값이 벽인지 확인//
    &&(arr_board[x-2][y]!=reverse_color)&&(arr_board[x+2][y]!=reverse_color))//_에 들어간 값이 반대 순서 값인지 확인//

    ||((arr_board[x+2][y]==color)&&(arr_board[x+1][y]==color)       //_●○○_//
    &&(arr_board[x+3][y]!='n')&&(arr_board[x-1][y]!='n')
    &&(arr_board[x+3][y]!=reverse_color)&&(arr_board[x-1][y]!=reverse_color))

    ||((arr_board[x-1][y]==color)&&(arr_board[x-2][y]==color)       //_○○●_//
    &&(arr_board[x+1][y]!='n')&&(arr_board[x-3][y]!='n')
    &&(arr_board[x+1][y]!=reverse_color)&&(arr_board[x-3][y]!=reverse_color))


    ||((arr_board[x+3][y]==color)&&(arr_board[x+2][y]==color)       //_●_○○_//
    &&(arr_board[x-4][y]!='n')&&(arr_board[x-1][y]!='n')
    &&(arr_board[x-4][y]!=reverse_color)&&(arr_board[x-1][y]!=reverse_color)
    &&(arr_board[x+1][y]!='n')&&(arr_board[x+1][y]!=reverse_color))

    ||((arr_board[x-1][y]==color)&&(arr_board[x-3][y]==color)       //_○_○●_//
    &&(arr_board[x+1][y]!='n')&&(arr_board[x-4][y]!='n')
    &&(arr_board[x+1][y]!=reverse_color)&&(arr_board[x-4][y]!=reverse_color)
    &&(arr_board[x-2][y]!='n')&&(arr_board[x-2][y]!=reverse_color))

    ||((arr_board[x+1][y]==color)&&(arr_board[x-2][y]==color)       //_○_●○_//
    &&(arr_board[x+2][y]!='n')&&(arr_board[x-3][y]!='n')
    &&(arr_board[x+2][y]!=reverse_color)&&(arr_board[x-3][y]!=reverse_color)
    &&(arr_board[x-1][y]!='n')&&(arr_board[x-1][y]!=reverse_color))


    ||((arr_board[x+1][y]==color)&&(arr_board[x+3][y]==color)       //_●○_○_//    
    &&(arr_board[x+4][y]!='n')&&(arr_board[x+2][y]!='n')
    &&(arr_board[x+4][y]!=reverse_color)&&(arr_board[x+2][y]!=reverse_color)
    &&(arr_board[x+2][y]!='n')&&(arr_board[x+2][y]!=reverse_color))

    ||((arr_board[x-1][y]==color)&&(arr_board[x+2][y]==color)       //_○●_○_//
    &&(arr_board[x-2][y]!='n')&&(arr_board[x+3][y]!='n')
    &&(arr_board[x-2][y]!=reverse_color)&&(arr_board[x+3][y]!=reverse_color)
    &&(arr_board[x+1][y]!='n')&&(arr_board[x+1][y]!=reverse_color))

    ||((arr_board[x-3][y]==color)&&(arr_board[x-2][y]==color)       //_○○_●_//
    &&(arr_board[x-4][y]!='n')&&(arr_board[x+1][y]!='n')
    &&(arr_board[x-4][y]!=reverse_color)&&(arr_board[x+1][y]!=reverse_color)
    &&(arr_board[x-1][y]!='n')&&(arr_board[x-1][y]!=reverse_color)))
    {
        if(((arr_board[x+1][y]!=color)&&(arr_board[x-1][y]==color)&&(arr_board[x-2][y]==color)&&(arr_board[x-3][y]==color))//_○○○●X//
        ||((arr_board[x-1][y]!=color)&&(arr_board[x+1][y]==color)&&(arr_board[x+2][y]==color)&&(arr_board[x+3][y]==color))//X●○○○_//

        ||((arr_board[x+1][y]==color)&&(arr_board[x+2][y]==color)&&(arr_board[x-1][y]==color))          //○●○○//
        ||((arr_board[x-1][y]==color)&&(arr_board[x-2][y]==color)&&(arr_board[x+1][y]==color))          //○○●○//
    
        ||((arr_board[x-1][y]==color)&&(arr_board[x-2][y]==color)&&(arr_board[x-4][y]==color))          //○_○○●//
        ||((arr_board[x+1][y]==color)&&(arr_board[x+2][y]==color)&&(arr_board[x+4][y]==color))          //●○○_○//
        )
            printf("4x3");
        else
            count++;//연속된 값이3개 되는걸 세는 변수//
    }
    //weight
    if(((arr_board[x][y-1]==color)&&(arr_board[x][y+1]==color)
    &&(arr_board[x][y+2]!='n')&&(arr_board[x][y-2]=='n')
    &&(arr_board[x][y+2]!=reverse_color)&&(arr_board[x][y-2]!=reverse_color))

    ||((arr_board[x][y+1]==color)&&(arr_board[x][y+2]==color)
    &&(arr_board[x][y+3]!='n')&&(arr_board[x][y-1]!='n')
    &&(arr_board[x][y+3]!=reverse_color)&&(arr_board[x][y-1]!=reverse_color))
 
    ||((arr_board[x][y-1]==color)&&(arr_board[x][y-2]==color)
    &&(arr_board[x][y-3]!='n')&&(arr_board[x][y+1]!='n')
    &&(arr_board[x][y-3]!=reverse_color)&&(arr_board[x][y+1]!=reverse_color))


    ||((arr_board[x][y-1]==color)&&(arr_board[x][y-3]==color)
    &&(arr_board[x][y-4]!='n')&&(arr_board[x][y+1]!='n')
    &&(arr_board[x][y-4]!=reverse_color)&&(arr_board[x][y+1]!=reverse_color)
    &&(arr_board[x][y-2]!='n')&&(arr_board[x][y-2]!=reverse_color))
    
    ||((arr_board[x][y+2]==color)&&(arr_board[x][y-1]==color)
    &&(arr_board[x][y+3]!='n')&&(arr_board[x][y-2]!='n')
    &&(arr_board[x][y+3]!=reverse_color)&&(arr_board[x][y-2]!=reverse_color)
    &&(arr_board[x][y+1]!='n')&&(arr_board[x+1][y]!=reverse_color))

    ||((arr_board[x][y+2]==color)&&(arr_board[x][y+3]==color)
    &&(arr_board[x][y+4]!='n')&&(arr_board[x][y-1]!='n')
    &&(arr_board[x][y+4]!=reverse_color)&&(arr_board[x][y-1]!=reverse_color)
    &&(arr_board[x][y+1]!='n')&&(arr_board[x][y+1]!=reverse_color))


    ||((arr_board[x][y+1]==color)&&(arr_board[x][y-2]==color)
    &&(arr_board[x][y+2]!='n')&&(arr_board[x][y-3]!='n')
    &&(arr_board[x][y+2]!=reverse_color)&&(arr_board[x][y-3]!=reverse_color)
    &&(arr_board[x][y-1]!='n')&&(arr_board[x][y-1]!=reverse_color))

    ||((arr_board[x][y-2]==color)&&(arr_board[x][y-3]==color)
    &&(arr_board[x][y-4]!='n')&&(arr_board[x][y+1]!='n')
    &&(arr_board[x][y-4]!=reverse_color)&&(arr_board[x][y+1]!=reverse_color)
    &&(arr_board[x][y-1]!=reverse_color)&&(arr_board[x][y-1]!=reverse_color))

    ||((arr_board[x][y+1]==color)&&(arr_board[x][y+3]==color)
    &&(arr_board[x][y-1]!='n')&&(arr_board[x][y+4]!='n')
    &&(arr_board[x][y-1]!=reverse_color)&&(arr_board[x][y+4]!=reverse_color)
    &&(arr_board[x][y+2]!=reverse_color)&&(arr_board[x][y+2]!=reverse_color)))
    {
        printf("_1_");
        if(((arr_board[x][y-1]!=color)&&(arr_board[x][y+1]==color)&&(arr_board[x][y+2]==color)&&(arr_board[x][y+3]==color))
            ||((arr_board[x][y+1]!=color)&&(arr_board[x][y-1]==color)&&(arr_board[x][y-2]==color)&&(arr_board[x][y-3]==color))
            
            ||((arr_board[x][y+1]==color)&&(arr_board[x][y+2]==color)&&(arr_board[x][y-1]==color))
            ||((arr_board[x][y-1]==color)&&(arr_board[x][y-2]==color)&&(arr_board[x][y+1]==color))

            ||((arr_board[x][y-1]==color)&&(arr_board[x][y-2]==color)&&(arr_board[x][y-4]==color))
            ||((arr_board[x][y+1]==color)&&(arr_board[x][y+2]==color)&&(arr_board[x][y+4]==color))     
            )
                {
                    printf("4x3");
                }
        else
            count++;
    }
    //down
    if(((arr_board[x-1][y-1]==color)&&(arr_board[x+1][y+1]==color)
    &&(arr_board[x-2][y-2]!='n')&&(arr_board[x+2][y+2]!='n')
    &&(arr_board[x-2][y-2]!=reverse_color)&&(arr_board[x+2][y+2]!=reverse_color))
    
    ||((arr_board[x+2][y+2]==color)&&(arr_board[x+1][y+1]==color)
    &&(arr_board[x+3][y+3]!='n')&&(arr_board[x-1][y-1]!='n')
    &&(arr_board[x+3][y+3]!=reverse_color)&&(arr_board[x-1][y-1]!=reverse_color))

    ||((arr_board[x-1][y-1]==color)&&(arr_board[x-2][y-2]==color)
    &&(arr_board[x-3][y-3]!='n')&&(arr_board[x+1][y+1]!='n')
    &&(arr_board[x-3][y-3]!=reverse_color)&&(arr_board[x+1][y+1]!=reverse_color))


    ||((arr_board[x-1][y-1]==color)&&(arr_board[x-3][y-3]==color)
    &&(arr_board[x+1][y+1]!='n')&&(arr_board[x-4][y-4]!='n')
    &&(arr_board[x+1][y+1]!=reverse_color)&&(arr_board[x-4][y-4]!=reverse_color)
    &&(arr_board[x-2][y-2]!='n')&&(arr_board[x-2][y-2]!=reverse_color))
    
    ||((arr_board[x-2][y-2]==color)&&(arr_board[x+1][y+1]==color)
    &&(arr_board[x-3][y-3]!='n')&&(arr_board[x+2][y+2]!='n')
    &&(arr_board[x-3][y-3]!=reverse_color)&&(arr_board[x+2][y+2]!=reverse_color)
    &&(arr_board[x-1][y-1]!='n')&&(arr_board[x-1][y-1]!=reverse_color))

    ||((arr_board[x+2][y+2]==color)&&(arr_board[x+3][y+3]==color)
    &&(arr_board[x+4][y+4]!='n')&&(arr_board[x-1][y-1]!='n')
    &&(arr_board[x+4][y+4]!=reverse_color)&&(arr_board[x-1][y-1]!=reverse_color)
    &&(arr_board[x+1][y+1]!='n')&&(arr_board[x+1][y+1]!=reverse_color))


    ||((arr_board[x-2][y-2]==color)&&(arr_board[x-3][y-3]==color)
    &&(arr_board[x-4][y-4]!='n')&&(arr_board[x+1][y+1]!='n')
    &&(arr_board[x-4][y-4]!=reverse_color)&&(arr_board[x+1][y+1]!=reverse_color)
    &&(arr_board[x-1][y-1]!='n')&&(arr_board[x-1][y-1]!=reverse_color))

    ||((arr_board[x-1][y-1]==color)&&(arr_board[x+2][y+2]==color)
    &&(arr_board[x-2][y-2]!='n')&&(arr_board[x+3][y+3]!='n')
    &&(arr_board[x-2][y-2]!=reverse_color)&&(arr_board[x+3][y+3]!=reverse_color)
    &&(arr_board[x+1][y+1]!='n')&&(arr_board[x+1][y+1]!=reverse_color))

    ||((arr_board[x+1][y+1]==color)&&(arr_board[x+3][y+3]==color)
    &&(arr_board[x+4][y+4]!='n')&&(arr_board[x-1][y-1]!='n')
    &&(arr_board[x+4][y+4]!=reverse_color)&&(arr_board[x-1][y-1]!=reverse_color)
    &&(arr_board[x+2][y+2]!='n')&&(arr_board[x+2][y+2]!=reverse_color)))
    {
        printf("_2_");
        if(((arr_board[x+1][y+1]!=color)&&(arr_board[x-1][y-1]==color)&&(arr_board[x-2][y-2]==color)&&(arr_board[x-3][y-3]==color))
        ||((arr_board[x-1][y-1]!=color)&&(arr_board[x+1][y+1]==color)&&(arr_board[x+2][y+2]==color)&&(arr_board[x+3][y+3]==color))
        
        ||((arr_board[x+1][y+1]==color)&&(arr_board[x+2][y+2]==color)&&(arr_board[x-1][y-1]==color))
        ||((arr_board[x+1][y+1]==color)&&(arr_board[x-1][y-1]==color)&&(arr_board[x-2][y-2]==color))
        
        ||((arr_board[x+1][y+1]==color)&&(arr_board[x+2][y+2]==color)&&(arr_board[x+4][y+4]==color))
        ||((arr_board[x-1][y-1]==color)&&(arr_board[x-2][y-2]==color)&&(arr_board[x-4][y-4]==color))
        )
            printf("4x3");
        else
            count++;
    }

    //up
    if(((arr_board[x+1][y-1]==color)&&(arr_board[x-1][y+1]==color)
    &&(arr_board[x+2][y-2]!='n')&&(arr_board[x-2][y+2]!='n')
    &&(arr_board[x+2][y-2]!=reverse_color)&&(arr_board[x-2][y+2]!=reverse_color))
    
    ||((arr_board[x-1][y+1]==color)&&(arr_board[x-2][y+2]==color)
    &&(arr_board[x-3][y+3]!='n')&&(arr_board[x+1][y-1]!='n')
    &&(arr_board[x-3][y+3]!=reverse_color)&&(arr_board[x+1][y-1]!=reverse_color))

    ||((arr_board[x+1][y-1]==color)&&(arr_board[x+2][y-2]==color)
    &&(arr_board[x+3][y-3]!='n')&&(arr_board[x-1][y+1]!='n')
    &&(arr_board[x+3][y-3]!=reverse_color)&&(arr_board[x-1][y+1]!=reverse_color))


    ||((arr_board[x-1][y+1]==color)&&(arr_board[x+2][y-2]==color)
    &&(arr_board[x-2][y+2]!='n')&&(arr_board[x+3][y-3]!='n')
    &&(arr_board[x-2][y+2]!=reverse_color)&&(arr_board[x+3][y-3]!=reverse_color)
    &&(arr_board[x+1][y-1]!='n')&&(arr_board[x+1][y-1]!=reverse_color))

    ||((arr_board[x-2][y+2]==color)&&(arr_board[x-3][y+3]==color)
    &&(arr_board[x-4][y+4]!='n')&&(arr_board[x+1][y-1]!='n')
    &&(arr_board[x-4][y+4]!=reverse_color)&&(arr_board[x+1][y-1]!=reverse_color)
    &&(arr_board[x-1][y+1]!='n')&&(arr_board[x-1][y+1]!=reverse_color))

    ||((arr_board[x+2][y-2]==color)&&(arr_board[x+3][y-3]==color)
    &&(arr_board[x+4][y-4]!='n')&&(arr_board[x-1][y+1]!='n')
    &&(arr_board[x+4][y-4]!=reverse_color)&&(arr_board[x-1][y+1]!=reverse_color)
    &&(arr_board[x+1][y-1]!='n')&&(arr_board[x+1][y-1]!=reverse_color))


    ||((arr_board[x+1][y-1]==color)&&(arr_board[x+3][y-3]==color)
    &&(arr_board[x+4][y-4]!='n')&&(arr_board[x-1][y+1]!='n')
    &&(arr_board[x+4][y-4]!=reverse_color)&&(arr_board[x-1][y+1]!=reverse_color)
    &&(arr_board[x+2][y-2]!='n')&&(arr_board[x+2][y-2]!=reverse_color))

    ||((arr_board[x-2][y+2]==color)&&(arr_board[x+1][y-1]==color)
    &&(arr_board[x+2][y-2]!='n')&&(arr_board[x-3][y+3]!='n')
    &&(arr_board[x+2][y-2]!=reverse_color)&&(arr_board[x-3][y+3]!=reverse_color)
    &&(arr_board[x-1][y+1]!='n')&&(arr_board[x-1][y+1]!=reverse_color))

    ||((arr_board[x-1][y+1]==color)&&(arr_board[x-3][y+3]==color)
    &&(arr_board[x-4][y+4]!='n')&&(arr_board[x+1][y-1]!='n')
    &&(arr_board[x-4][y+4]!=reverse_color)&&(arr_board[x+1][y-1]!=reverse_color)
    &&(arr_board[x-2][y+2]!='n')&&(arr_board[x-2][y+2]!=reverse_color)))
    {
        printf("_3_");
        if(((arr_board[x+1][y-1]!=color)&&(arr_board[x-1][y+1]==color)&&(arr_board[x-2][y+2]==color)&&(arr_board[x-3][y+3]==color))
        ||((arr_board[x-1][y+1]!=color)&&(arr_board[x+1][y-1]==color)&&(arr_board[x+2][y-2]==color)&&(arr_board[x+3][y-3]==color))
        
        ||((arr_board[x+1][y-1]==color)&&(arr_board[x-1][y+1]==color)&&(arr_board[x-2][y+2]==color))
        ||((arr_board[x-1][y+1]==color)&&(arr_board[x+1][y-1]==color)&&(arr_board[x+2][y-2]==color))
        
        ||((arr_board[x+1][y-1]==color)&&(arr_board[x+2][y-2]==color)&&(arr_board[x+4][y-4]==color))
        ||((arr_board[x-1][y+1]==color)&&(arr_board[x-2][y+2]==color)&&(arr_board[x-4][y+4]==color))
        )

            printf("4x3");
        else
            count++;
    }
    if(count>1)
    {
        printf("%d",count); //연속된 값이 3이되는 게 2개 이상일 시 1 리턴//
        return 1;
    }
    else
        return 0;
}