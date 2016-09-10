#include <cstdlib>
#include "tile.h"
#include "main.h"
static char _RESET_COLOR[]="\033[0m";
static char _GREEN[]="\033[0;32m";
static char _L_GREEN[]="\033[1;32m";
static char _GRAY[]="\033[0;39m";
static char _WHITE[]="\033[1;39m";
Tile field[64][32];
int px,py;
Tile::Tile()
{
	floor();
}
Tile::Tile(char b,char f)
{
	bg=b;
	fg=f;
}
void Tile::grass()
{
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
void Tile::wall()
{
	bg=fg='X';
	bc=fc=_GRAY;
}
void Tile::floor()
{
	bg='#';
	fg=' ';
	bc=_WHITE;
	fc=_RESET_COLOR;
}
int Tile::set(char c)
{
	//if ()
	//return ...
	fg=c;
	/**/return 0;
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
void Tile::move(char c)
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
