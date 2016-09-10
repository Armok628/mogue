#include <iostream>
#include <ctime>
#include <cstdlib>
#include "main.h"
#include "tile.h"
using namespace std;
static char _RESET_COLOR[]="\033[0m";
static char _RED[]="\033[1;31m";
static char _WHITE[]="\033[1;39m";
static bool quit = false;
static char input[64];
static char ione;
void cls();
void draw();
char in();
int main()
{
	srand(time(0));
	for (int y=0;y<32;y++)
		for (int x=0;x<64;x++)
			field[x][y].grass();
	/*house. to-do: make display class*/
	for (int y=15;y<20;y++)
		for (int x=15;x<25;x++)
			field[x][y].wall();
	for (int y=16;y<19;y++)
		for (int x=16;x<24;x++)
			field[x][y].floor();
	field[15][17].floor();
	/**/
	field[10][10].set('@');field[10][10].setc(_WHITE);
	px=10;py=10;
	do
	{
		cls();
		draw();
		int ione=in();
		if (ione=='q')
			quit=true;
		else field[px][py].move(ione);
	}
	while (quit==false);
	return 0;
}
void cls()
{
	system("clear");
	//for (int i=0;i<64;i++)
	//	cout<<endl;
}
void draw()
{
	for (int y=0;y<32;y++)
	{
		for (int x=0;x<64;x++)
			cout<<field[x][y].retc()<<field[x][y].ret();
		cout<<_RESET_COLOR<<endl;
	}
}
char in()
{
	cin>>input;
	return input[0];
}
