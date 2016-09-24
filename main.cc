#include <iostream>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <ncurses.h>
#include "main.h"
#include "display.h"
#include "tile.h"
using namespace std;
static bool quit = false;
static char input[64];
static char ione;
static Display d;
int main()
{
	int ix,iy,iw,ih;
	char id[64];
	for (int y=0;y<32;y++)
		for (int x=0;x<64;x++)
			field[x][y].grass();

	cout<<"--House parameters--"<<endl;
	do
	{
		cout<<"X (int) : ";cin>>ix;
		cout<<"Y (int) : ";cin>>iy;
		cout<<"W (int) : ";cin>>iw;
		cout<<"H (int) : ";cin>>ih;
		cout<<"D (h/j/k/l) : ";cin>>id;
		d.house(ix,iy,iw,ih,id[0]);
		cout<<endl<<"Another house? (y/n) : ";
		cin>>id;
	} while (id[0]=='y');

	cout<<"--Animal parameters--"<<endl;
	do
	{
		cout<<"X (int) : ";cin>>ix;
		cout<<"Y (int) : ";cin>>iy;
		field[ix][iy].animal();
		cout<<"Another animal? (y/n) ";cin>>id;
	} while (id[0]=='y');

	srand(time(0));
	d.placeplayer(10,10);
	px=10;py=10;
	initscr();
	do
	{
		clear();
		refresh();
		d.draw();
		ione=getch();
		if (ione=='q')
			quit=true;
		else field[px][py].pmove(ione);
		for (int y=0;y<32;y++)
			for (int x=0;x<64;x++)
				field[x][y].rmove(x,y,1);
	}
	while (quit==false);
	endwin();
	return 0;
}
