#include <stdio.h>
#include <time.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>
#include <stdlib.h> 
#include <math.h>

const int up=4,down=26,left=2,right=14;
unsigned short pool[28];
unsigned short xx,yy;
bool dead,pause;
int typea,typeb;
int typea1,typeb1;
int score=0;
int level=1;
//各类方块 
const unsigned short maintable[7][4] = { 
		{ 0x00F0U, 0x2222U, 0x00F0U, 0x2222U },
		{ 0x0072U, 0x0262U, 0x0270U, 0x0232U },
		{ 0x0223U, 0x0074U, 0x0622U, 0x0170U },
		{ 0x0226U, 0x0470U, 0x0322U, 0x0071U },
		{ 0x0063U, 0x0264U, 0x0063U, 0x0264U },
		{ 0x006CU, 0x0462U, 0x006CU, 0x0462U },
		{ 0x0660U, 0x0660U, 0x0660U, 0x0660U }
};
//初始地图 
const unsigned short mainbox[28] = {
	 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 
	 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 
	 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 
	 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xC003U, 0xFFFFU, 0xFFFFU 
};

HANDLE g_hConsoleOutput;

//找到合适的点打印 
void gotopos(short x,short y)
{
	static COORD cd;
	cd.X=(short)(x<<1);
	cd.Y=y;
	SetConsoleCursorPosition(g_hConsoleOutput,cd);
 } 

//找到游戏方框中合适的点 
void gotobox(short x,short y)
{
	gotopos(x+9,y-3);
}

//改变颜色 
void changecolor(int x)
{
	SetConsoleTextAttribute(g_hConsoleOutput,x);
 }

void showscore()
{
	changecolor(10);
	gotopos(6,2);
	printf("%d",score);
}

void showlevel()
{
	changecolor(13);
	gotopos(6,4);
	printf("%d",level);
}
//碰撞检测
bool crash()
{
	unsigned short matrix=maintable[typea][typeb];
	unsigned short dest = 0;
	dest |= (((pool[yy + 0] >> xx) << 0x0) & 0x000F);
	dest |= (((pool[yy + 1] >> xx) << 0x4) & 0x00F0);
	dest |= (((pool[yy + 2] >> xx) << 0x8) & 0x0F00);
	dest |= (((pool[yy + 3] >> xx) << 0xC) & 0xF000);
	return ((dest & matrix) != 0);
 } 

//删除方块
void Remove()
{
	unsigned short matrix;
	matrix=maintable[typea][typeb];
	pool[yy + 0] &= ~(((matrix >> 0x0) & 0x000F) << xx);
	pool[yy + 1] &= ~(((matrix >> 0x4) & 0x000F) << xx);
	pool[yy + 2] &= ~(((matrix >> 0x8) & 0x000F) << xx);
	pool[yy + 3] &= ~(((matrix >> 0xC) & 0x000F) << xx); 
 } 
//插入方块 
void insert()
{
	unsigned short matrix;
	matrix=maintable[typea][typeb];
	pool[yy + 0] |= (((matrix >> 0x0) & 0x000F) << xx);
	pool[yy + 1] |= (((matrix >> 0x4) & 0x000F) << xx);
	pool[yy + 2] |= (((matrix >> 0x8) & 0x000F) << xx);
	pool[yy + 3] |= (((matrix >> 0xC) & 0x000F) << xx); 
}
//画出下一个方块
void drawnext()
{
	const int startx=3,starty=15;
	int i,j;
	changecolor(14);
	gotopos(startx-1,starty-2);
	printf("%s","下一个方块"); 
	unsigned short matrix=maintable[typea1][typeb1];
	for (i=0;i<4;i++)
		for (j=0;j<4;j++)
		{
			if ((matrix&(1<<(i*4+j)))!=0)
			{
				gotopos(startx+j,starty+i);
				changecolor(9);
				printf("■");
			}
			else
			{
				gotopos(startx+j,starty+i);
				changecolor(0);
				printf("%2s","");
			}
		}
 } 
//创建新方块 
void create()
{
	unsigned short matrix;
	typea=typea1;
	typeb=typeb1;
	typea1=rand()%7;
	typeb1=rand()%4;
	drawnext();
	matrix=maintable[typea][typeb];
	if (matrix&0xF000)
	{
		yy=0;
	}
	else
	{
		yy=(matrix&0xFF00)?1:2;
	}
	xx=6;
	if (crash())
	{
		dead=true;
	} 
	else
	{
		insert();
	}
}

//消行检测
bool erase()
{
	const unsigned scores[5]={0,1,3,9,15};
	unsigned short count=0;
	unsigned short k=0,y=yy+3; 
	do {
		if (y<down && pool[y]==0xFFFFU)
		{
			count++; 
			memmove(pool+1,pool,sizeof(unsigned short)*y);
		} 
		else 
		{
			y--;
			k++;
		}
	} while (y>=yy && k<4);
	score+=scores[count]*level;
	showscore();
	level+=count;
	showlevel();
	create();
	return (count > 0);
 } 
 
//画出游戏区域的方块 
void drawbox()
{
	changecolor(0xF0);
	int i;
	for (i=4;i<26;++i)
	{
		gotopos(10,i-3);
		printf("%2s","");
		gotopos(23,i-3);
		printf("%2s","");
	}
	gotopos(10,i-3);
	printf("%28s", "");
 } 

//展示游戏区域的所有内容 
void showgame()
{
	int x,y;
	for (y=up;y<down;++y)
	{
		gotobox(2,y);
		for (x=left;x<right;++x)
		{
			if ((pool[y]>>x)&1)
			{
				changecolor(12);
				printf("■");
			}
			else
			{
				changecolor(0);
				printf("%2s", "");
			}
		}
	}
}

// 显示当前方块
void showmatrix() 
{
	unsigned short x,y;
	y=(yy>up)?(yy-1):up;
	for (;y<down && y<yy+4;++y)
	{
		x=(xx>left)?(xx-1):left;
		for (;x<right && x<xx+5;++x)
		{
			gotobox(x,y);
			if ((pool[y]>>x) & 1)
			{
				changecolor(12);
				printf("■");
			} else
			{
				changecolor(0);
				printf("%2s", "");
			}
		}
	}
}
//移动方块 
void movedown()
{
	unsigned short y=yy;
	Remove();
	yy++;
	if (crash())
	{
		yy=y;
		insert();
		if (erase())
		{
			showgame();
		}
	} 
	else 
	{
		insert();
		showmatrix();
	}
}

//左右移动
void move(int demo)
{
	int x =xx;
	Remove();
	demo==0?(--xx):(++xx);
	if (crash())
	{
		xx=x;
		insert();
	} else {
		insert();
		showmatrix();
	}
} 
//旋转方块 
void rotate()
{
	Remove();
	int dede=typeb;
	typeb=(typeb+1)%4;
	if (crash())
	{
		typeb=dede;
		insert();
	} 
	else 
	{
		insert();
		showmatrix();
	}
}

//下一步内容 
void Down(int key)
{
	if (key==13)
		pause=!pause;

	if (pause)
		return;
	
	switch (key) 
	{
	case 72:  // 上 
		rotate();
		break;
	case 75:  // 左
		move(0);
		break;
	case 77:  // 右
		move(1);
		break;
	case 80:  // 下
		movedown();
		break;
	default:
		break;
	}
}

//执行游戏 
void Run()
{
	clock_t cL,cN;
	cL=clock();
	showgame();
	while (!dead)
	{
		while (_kbhit())
		{
			Down(_getch());
		} 
		if (!pause)
		{
			cN=clock();
			if (cN-cL>0.45F*pow(0.98,level)*CLOCKS_PER_SEC) 
			{
				cL=cN;
				Down(80);
			}
		}
	}
 }

//界面构造 
void init()
{
	int i;
	CONSOLE_CURSOR_INFO cursorInfo={1,FALSE};
	g_hConsoleOutput=GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorInfo(g_hConsoleOutput,&cursorInfo);
	SetConsoleTitleA("俄罗斯方块");
	for (i=0;i<28;i++) 
		pool[i]=mainbox[i];
	srand((unsigned) time(NULL));
	typea1=rand()%7;
	typeb1=rand()%4;
	create();
	gotopos(3,2);
	changecolor(10);
	printf("%s","得分:"); 
	showscore();
	gotopos(3,4);
	changecolor(13);
	printf("%s","等级:");
	showlevel();
}

void showreadme()
{
	changecolor(14);
	gotopos(26,3);
	printf("%s","现在开始俄罗斯方块游戏");
	gotopos(26,5);
	printf("%s","↑代表旋转");
	gotopos(26,7);
	printf("%s","←→代表移动方块");
	gotopos(26,9);
	printf("%s","↓代表快速下落");
	gotopos(26,11);
	printf("%s","Enter 代表暂停");
	gotopos(26,13);
	printf("%s","等级越高下落速度越快，但得分越高");
	gotopos(26,15);
	printf("%s","如果你不幸死亡，游戏将直接结束");
	gotopos(26,17);
	printf("%s","玩得愉快！");
	gotopos(26,19);
	printf("%s","by HarborYuan"); 
}
//死亡
void godie()
{
	system("cls");
	gotopos(0,0);
	printf("%s","游戏结束");
	system("pause");
 } 
int main()
{
	init();
	drawbox();
	showreadme();
	Run();
	changecolor(0);
	godie(); 
	return 0;
}
