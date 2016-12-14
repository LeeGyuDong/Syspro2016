#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>
#include <termios.h>
#include <stdlib.h>
#include <string.h>

#define BAR 	"-------------"
#define BLANK	"             "    


void frame(int sigNum );
void ball_frame();
void block_frame();
void bar_frame();

int set_tick(int time);
void initialize();
void shutdown();
void endFrame();
void crmode2();
void readStage(char* fileName, int array[50][100], int* stageStartHeight, int* stageStartWidth, int* stageHeight, int* stageWidth);

struct ball
{
	int xPos,yPos; //공의 위치
	int xDirection,yDirection; //공이 이동할 방향
};

struct bar
{
	int x_pos;
	char *symbol;	
};

int stageStartHeight;
int stageStartWidth;
int stageHeight;
int stageWidth;
struct ball ball;
struct bar bar;
int command;

int map[50][100];


int barStart = 50;


int main()
{
	initialize();
	
	set_tick(50);
	signal(SIGALRM,frame);	
	
	while((command = getchar()))
	{
		if( command == 'q' )
		{
			endwin();
			exit(1);
		}
		else if( command == 'z' && bar.x_pos > 8)
			bar.x_pos--;
		else if( command == 'm' && bar.x_pos < 2 + stageWidth - strlen(bar.symbol)/2 )
			bar.x_pos++;
	}
	return 0;
}

void initialize()
{
	initscr();
	crmode2();

	ball.xPos = stageStartHeight + 3;
	ball.yPos = stageStartWidth + 20;
	ball.xDirection = 1;
	ball.yDirection = 1;

	bar.symbol = BAR;
	bar.x_pos = barStart;
	
	if(has_colors())
	{
		start_color();

		init_pair(1,COLOR_RED,COLOR_RED);
		init_pair(2,COLOR_YELLOW,COLOR_YELLOW);
		init_pair(3,COLOR_RED,COLOR_GREEN);
		init_pair(4,COLOR_WHITE,COLOR_BLACK);
		init_pair(5,COLOR_BLUE,COLOR_WHITE);
	}
	
	readStage("./1.stage", map, &stageStartHeight, &stageStartWidth, &stageHeight, &stageWidth);	

}

void frame(int sigNum)
{
	int i, j;
	clear();
	
	block_frame();
	ball_frame();
	bar_frame();

	endFrame();
}

int set_tick(int time)
{
	struct itimerval timeSet;
	long sec,min;
	
	sec = time / 1000;
	min = ( time % 1000 ) * 1000L;
	
	timeSet.it_interval.tv_sec = sec;
	timeSet.it_interval.tv_usec = min;
	
	timeSet.it_value.tv_sec = sec;
	timeSet.it_value.tv_usec = min;

	return setitimer(ITIMER_REAL, &timeSet,NULL);
}


void block_frame()
{
	int i,j, x,y;
		

	for(i = stageStartHeight,x=0 ; i < stageStartHeight + stageHeight ; i++, x++)
	{
		for(j = stageStartWidth,y=0 ; j <stageStartWidth + stageWidth ; j++,y++)
		{
			move(i,j);
			if(map[x][y] == 1)
			{	
				attron(COLOR_PAIR(5));
				printw(" ");
		
			}

			else if(map[x][y] == 2)
			{
				attron(COLOR_PAIR(3));
				printw(" ");
			}

		}
	}

}

void ball_frame()
{
	int i, j;
	attron(COLOR_PAIR(4));
	if(ball.xPos + ball.xDirection >=  stageWidth + stageStartWidth)
		ball.xDirection = -1;
	else if(ball.xPos + ball.xDirection < stageStartWidth)
		ball.xDirection = 1;

	if(ball.yPos + ball.yDirection >= stageHeight + stageStartHeight)
		ball.yDirection = -1;
	else if ( ball.yPos + ball.yDirection < stageStartHeight)
		ball.yDirection = 1;

	ball.xPos = ball.xPos + ball.xDirection;
	ball.yPos = ball.yPos + ball.yDirection;

	move(ball.yPos,ball.xPos);
	addstr("o");

	
}

void bar_frame()
{
	if(command == 'z')
		move(barStart, bar.x_pos+1);
	else if(command == 'm')
		move(barStart,bar.x_pos - strlen(bar.symbol));
	addstr(BLANK);
	move(barStart,bar.x_pos);
	addstr(bar.symbol);
}

void crmode2()
{
	struct termios tty;
	tcgetattr(0, &tty);
	tty.c_lflag &= ~ICANON;
	tty.c_lflag &= ~ECHO;

	tcsetattr(0,TCSANOW,&tty);
}

void endFrame()
{
	move(LINES-1,0);
	refresh();
}


void shutdown()
{
	endwin();
}
