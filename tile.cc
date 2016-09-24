#include <cstdlib>
#include "tile.h"
#include "main.h"
static char _RESET_COLOR[]="\033[0m";
static char _GREEN[]="\033[0;32m";
static char _L_GREEN[]="\033[1;32m";
static char _GRAY[]="\033[0;39m";
static char _WHITE[]="\033[1;39m";
static char _YELLOW[]="\033[1;33m";
bool m;
Tile::Tile()
{
	floor();
}
Tile::Tile(char b,char f,bool g)
{
	m=g;
	bg=b;
	fg=f;
}
void Tile::grass()
{
	m=false;
	fg=' ';
	switch(rand()%2)
	{
		case 0:bc=_GREEN;break;
		case 1:bc=_L_GREEN;break;
	}
	switch(rand()%5)
	{
		case 0:bg='"';break;
		case 1:bg='\'';break;
		case 2:bg=',';break;
		case 3:bg='.';break;
		case 4:bg='`';break;
	}
}
void Tile::rmove(int x,int y)
{
	if (m==true)
		switch (rand()%5)
		{
			case 0:break;
			case 1:move(x,y,'h');break;
			case 2:move(x,y,'j');break;
			case 3:move(x,y,'k');break;
			case 4:move(x,y,'l');break;
		}
}
void Tile::wall()
{
	m=false;
	bg=fg='X';
	bc=fc=_GRAY;
}
void Tile::floor()
{
	m=false;
	bg='#';
	fg=' ';
	bc=_WHITE;
	fc=_RESET_COLOR;
}
void Tile::animal()
{
	grass();
	m=true;
	fg='D';
	fc=_YELLOW;
}
int Tile::set(char c)
{
	fg=c;
	return 0;////
}
int Tile::setc(char* c)
{
	if (fg==' ')
		bc=c;
	else fc=c;
}
char Tile::ret()
{
	if (fg==' ')
		return bg;
	return fg;
}
char* Tile::retc()
{
	if (fg==' ')
		return bc;
	return fc;
}
void Tile::move(int x,int y,char c)
{
	int offset[2];offset[0]=0;offset[1]=0;
	switch (c)
	{
		case 'h':offset[0]=-1;break;
		case 'j':offset[1]=1;break;
		case 'k':offset[1]=-1;break;
		case 'l':offset[0]=1;break;
	}
	int nx=x+offset[0];int ny=y+offset[1];
	if (field[nx][ny].fg==' ')
	{
		field[nx][ny].fg=field[x][y].fg;
		field[nx][ny].fc=field[x][y].fc;
		field[nx][ny].m=true;field[x][y].m=false;
		field[x][y].fg=' ';
		x=nx;y=ny;
	}
}
void Tile::pmove(char c)
{
	int offset[2];offset[0]=0;offset[1]=0;
	switch (c)
	{
		case 'h':offset[0]=-1;break;
		case 'j':offset[1]=1;break;
		case 'k':offset[1]=-1;break;
		case 'l':offset[0]=1;break;
	}
	int nx=px+offset[0];int ny=py+offset[1];
	if (field[nx][ny].fg==' ')
	{
		field[nx][ny].fg=field[px][py].fg;
		field[nx][ny].fc=field[px][py].fc;
		field[px][py].fg=' ';
		px=nx;py=ny;
	}
}
