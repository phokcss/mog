#include <stdio.h>
#define SIZE 23     //19x19바둑판 에러 방지를 위해 4를 더한 값//
void board_set(char (*arr_board)[23])
{

for (int i = 0; i < SIZE; i++)
        {
                {
                       for (int j = 0; j < SIZE; j++)
                        {   if((i>1)&&(j>1)&&(i<21)&&(j<21))
                                arr_board[i][j]=' ';        //공백//
                            else
                                arr_board[i][j]='n';        //벽//
                        }
                }       
        }
}

void board_mornitor(char (*arr_board)[23])
{
    printf("     1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19\n");
	printf("   ----------------------------------------------------------------------------\n");
	for (int i = 2; i < SIZE-2; i++)
	{
		if (i < 11)//Y좌표 위치를 보여줌
			printf("%d  ", i - 1);      //1의 자리 y(왼쪽)//
		else
			printf("%d ", i - 1);       //10의 자리 y(왼쪽)//

		for (int n = 2; n < SIZE-2; n++)//현재 오목의 상태를 보여줌
		{
			printf("| %c ", arr_board[i][n]);   //오목판에 O혹은X 넣기//
		}
		printf("|");                    //내부 바둑판 그리기//
        printf("  %d\n", i - 1);        //Y좌표 오른쪽//
		printf("   ----------------------------------------------------------------------------\n");
	}
    printf("     1   2   3   4   5   6   7   8   9  10  11  12  13  14  15  16  17  18  19\n");
}
int victory_detect(char (*arr_board)[23],char color)    //오목 발생 탐지 함수//
{
    for(int i=2; i<21;i++) //인덱스 에러 방지를 위해 2+19+2임으로 2~21로 범위//
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
    char reverse_color; //현재 반대 순서 저장 변수//
    if(color=='x')
        reverse_color='o';
    else
        reverse_color='x';    
    int count=0;        //연속된 3이 몇개 발생하는지 세는 변수//
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
        if(((arr_board[x+1][y]!=color)&&(arr_board[x-1][y]==color)&&(arr_board[x-2][y]==color)&&(arr_board[x-3][y]==color)//_○○○●X//
        ||(arr_board[x-1][y]!=color)&&(arr_board[x+1][y]==color)&&(arr_board[x+2][y]==color)&&(arr_board[x+3][y]==color)//X●○○○_//

        ||(arr_board[x+1][y]==color)&&(arr_board[x+2][y]==color)&&(arr_board[x-1][y]==color)        //○●○○//
        ||(arr_board[x-1][y]==color)&&(arr_board[x-2][y]==color)&&(arr_board[x+1][y]==color)        //○○●○//  
    
        ||(arr_board[x-1][y]==color)&&(arr_board[x-2][y]==color)&&(arr_board[x-4][y]==color)        //○_○○●//
        ||(arr_board[x+1][y]==color)&&(arr_board[x+2][y]==color)&&(arr_board[x+4][y]==color)        //●○○_○//
        ))
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
        if((arr_board[x][y-1]!=color)&&(arr_board[x][y+1]==color)&&(arr_board[x][y+2]==color)&&(arr_board[x][y+3]==color)
            ||((arr_board[x][y+1]!=color)&&(arr_board[x][y-1]==color)&&(arr_board[x][y-2]==color)&&(arr_board[x][y-3]==color)
            
            ||(arr_board[x][y+1]==color)&&(arr_board[x][y+2]==color)&&(arr_board[x][y-1]==color)
            ||(arr_board[x][y-1]==color)&&(arr_board[x][y-2]==color)&&(arr_board[x][y+1]==color)

            ||(arr_board[x][y-1]==color)&&(arr_board[x][y-2]==color)&&(arr_board[x][y-4]==color)
            ||(arr_board[x][y+1]==color)&&(arr_board[x][y+2]==color)&&(arr_board[x][y+4]==color)            
            ))
            
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
        if(((arr_board[x+1][y+1]!=color)&&(arr_board[x-1][y-1]==color)&&(arr_board[x-2][y-2]==color)&&(arr_board[x-3][y-3]==color)
        ||(arr_board[x-1][y-1]!=color)&&(arr_board[x+1][y+1]==color)&&(arr_board[x+2][y+2]==color)&&(arr_board[x+3][y+3]==color)
        
        ||(arr_board[x+1][y+1]==color)&&(arr_board[x+2][y+2]==color)&&(arr_board[x-1][y-1]==color)
        ||(arr_board[x+1][y+1]==color)&&(arr_board[x-1][y-1]==color)&&(arr_board[x-2][y-2]==color)
        
        ||(arr_board[x+1][y+1]==color)&&(arr_board[x+2][y+2]==color)&&(arr_board[x+4][y+4]==color)
        ||(arr_board[x-1][y-1]==color)&&(arr_board[x-2][y-2]==color)&&(arr_board[x-4][y-4]==color)
        ))
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
        if(((arr_board[x+1][y-1]!=color)&&(arr_board[x-1][y+1]==color)&&(arr_board[x-2][y+2]==color)&&(arr_board[x-3][y+3]==color)
        ||(arr_board[x-1][y+1]!=color)&&(arr_board[x+1][y-1]==color)&&(arr_board[x+2][y-2]==color)&&(arr_board[x+3][y-3]==color)
        
        ||(arr_board[x+1][y-1]==color)&&(arr_board[x-1][y+1]==color)&&(arr_board[x-2][y+2]==color)
        ||(arr_board[x-1][y+1]==color)&&(arr_board[x+1][y-1]==color)&&(arr_board[x+2][y-2]==color)
        
        ||(arr_board[x+1][y-1]==color)&&(arr_board[x+2][y-2]==color)&&(arr_board[x+4][y-4]==color)
        ||(arr_board[x-1][y+1]==color)&&(arr_board[x-2][y+2]==color)&&(arr_board[x-4][y+4]==color)
        ))

            printf("4x3");
        else
            count++;
    }


    if((arr_board[x-1][y+1]==color)&&(arr_board[x-3][y+3]==color))
{
    printf("43\n");
}    
    if(count>1)
    {
        printf("%d",count); //연속된 값이 3이되는 게 2개 이상일 시 1 리턴//
        return 1;
    }
    else
        return 0;
}
int main(int argc,char *argv[])
{   
    char arr_board[23][23];     //보드 생성//
    board_set((arr_board));     //보드 리셋//
    
    board_mornitor(arr_board);  //보드 보여주는 함수//
    char color= 'x';            //현재 순서를 블랙으로 설정//
    printf("_____________________________________black_____________________________________\n");
    int x,y;
    while(1)
    {  
        if(color=='x') 
            printf("_____________________________________black_____________________________________\n");//블랙 턴//
        else
            printf("_____________________________________white_____________________________________\n");//화이트 턴//
        printf("x:");
        int numx, numy;  //X좌표,Y좌표 선언//
        numx=scanf("%d",&x);
        printf("y:");
        numy=scanf("%d",&y);
        if((arr_board[x+1][y+1]==('x'))||(arr_board[x+1][y+1]==('o')))  //들어갈려는 좌표에 이미 값이 있다면//
        {
            printf("exist\n");
            while(getchar()!='\n');         //버퍼에 있는 값을 제거해 오류 방지//
            continue;                       //반복문 처음으로 돌아감//
        }
        else if (((x>0)&&(x<20))&&((y>0)&&(y<20)))      //범위가1~19인지 확인//
        {
            printf("right range\n");
        }
        else 
        {
            while(getchar()!='\n');
            printf("not right range or type error\n");
            continue;
        }
        if((double_three_detect(arr_board, x+1,y+1,color)==1))//33감지 함수//
            {
                printf("double Three\n");
                continue;
            }
        
        arr_board[x+1][y+1]=color;  //배열은 0부터 시작하고 보드는 1부터 시작하기 때문에 +1//
    
        board_mornitor(arr_board);
        if(victory_detect(arr_board,color)==1) //승리 감지//
        {
            printf("yhhhhhyyyyyyyyyyyyyyy\n");
            break;
        }
        if(color=='x')      //블랙 턴이라면//
            color='o';      //화이트 턴으로 변경//
        else
            color='x';      //화이트 턴이라면 블랙턴으로 변경//
    }
}
