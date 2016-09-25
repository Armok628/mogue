#include <iostream>
#include <stdio.h>
#include <ctime>
#include <cstdlib>
#include <ncurses.h>
#include "main.h"
#include "display.h"
#include "tile.h"
using namespace std;
static char input[64];
static char ione;
static bool quit=false;
static Display d;
int main()
{
	srand(time(0));
	int ix,iy,iw,ih,in;
	char id[64];
	for (int y=0;y<32;y++)
		for (int x=0;x<64;x++)
			field[x][y].grass();
	cout<<"--Player parameters--"<<endl;
	cout<<"X (int) : ";cin>>ix;
	cout<<"Y (int) : ";cin>>iy;
	d.placeplayer(ix,iy);
	cout<<"--House parameters--"<<endl;
	cout<<"# (int) : ";cin>>in;
	for (int i=0;i<in;i++)
	{
		quit=false;
		ix=rand()%64;iy=rand()%32;
		iw=3+rand()%32;ih=3+rand()%16;
		switch (rand()%4)
		{
			case 0:id[0]='h';break;
			case 1:id[0]='j';break;
			case 2:id[0]='k';break;
			case 3:id[0]='l';break;
		}
		if (ix+iw>63||iy+ih>31)
			quit=true;
		else
			for (int i1=ix;i1<=ix+iw;i1++)
				for (int i2=iy;i2<=iy+ih;i2++)
					if (field[i1][i2].fg=='@')
						quit=true;
		if (!quit)
			d.house(ix,iy,iw,ih,id[0]);
		else i--;
	}
	/*//Manual house placement
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
	*/
	cout<<"--Animal parameters--"<<endl;
	cout<<"# (int) : ";cin>>in;
	for (int i=0;i<in;i++)
	{
		ix=rand()%64;iy=rand()%32;
		if (field[ix][iy].fg!='%')
			field[ix][iy].animal();
		else i--;
	}
	initscr();
	quit=false;
	do
	{
		clear();
		refresh();
		d.draw();
		ione=getch();
		if (ione=='q')
			quit=true;
		else field[px][py].move(px,py,ione);
		for (int y=0;y<32;y++)
			for (int x=0;x<64;x++)
				field[x][y].rmove(x,y,1);
	}
	while (quit==false);
	endwin();
	return 0;
}
