#include <cstdlib>
#include "tile.h"
#include "main.h"
static char _RESET_COLOR[]="\033[0m";
static char _GREEN[]="\033[0;32m";
static char _L_GREEN[]="\033[1;32m";
static char _GRAY[]="\033[0;39m";
static char _WHITE[]="\033[1;39m";
static char _YELLOW[]="\033[1;33m";
static char _RED[]="\033[0;31m";
static char _L_RED[]="\033[1;31m";
bool m;
Tile::Tile()
{
	grass();
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
void Tile::rmove(int x,int y,int c)
{
	for (int i=0;i<c;i++)
		if (m==true)
			switch (rand()%9)
			{
				case 0:break;
				case 1:move(x,y,'h');break;
				case 2:move(x,y,'j');break;
				case 3:move(x,y,'k');break;
				case 4:move(x,y,'l');break;
				case 5:move(x,y,'y');break;
				case 6:move(x,y,'u');break;
				case 7:move(x,y,'b');break;
				case 8:move(x,y,'n');break;
			}
}
void Tile::wall(int x,int y)
{
	m=false;
	bg=fg='%';
	if (x%2!=y%2)
		bc=fc=_RED;
	else bc=fc=_L_RED;

}
void Tile::floor(int x,int y)
{
	m=false;
	bg='#';
	fg=' ';
	fc=_RESET_COLOR;
	if (x%2!=y%2)
		bc=fc=_GRAY;
	else bc=fc=_WHITE;
}
void Tile::animal()
{
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
	return 0;////
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
		case 'y':offset[0]=-1;offset[1]=-1;break;
		case 'u':offset[0]=1;offset[1]=-1;break;
		case 'b':offset[0]=-1;offset[1]=1;break;
		case 'n':offset[0]=1;offset[1]=1;break;
	}
	int nx=x+offset[0];int ny=y+offset[1];
	if (!((c=='h'||c=='y'||c=='b')&&x==0
			||(c=='j'||c=='b'||c=='n')&&y==31
			||(c=='k'||c=='y'||c=='u')&&y==0
			||(c=='l'||c=='u'||c=='n')&&x==63)
			&&field[nx][ny].fg==' ')
	{
		field[nx][ny].fg=field[x][y].fg;
		field[nx][ny].fc=field[x][y].fc;
		if (fg!='@')
		{field[nx][ny].m=true;field[x][y].m=false;}
		else {px=nx,py=ny;}
		field[x][y].fg=' ';
	}
}
