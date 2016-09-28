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
static char ic;
static bool quit=false;
static Display d;
int main()
{
	srand(time(0));// Seed the random number generator.
	int ix,iy,iw,ih,in,r,c;// Create local variables.
	char id[64];// Create local character array variable.
	for (int x=0;x<64;x++)// For every tile...
		for (int y=0;y<32;y++)// ...
			field[x][y].grass();// ...initialize each as grass.
	/*// DEPRECATED: Manual player placement
	cout<<"--Player parameters--"<<endl;
	cout<<"X (int) : ";cin>>ix;
	cout<<"Y (int) : ";cin>>iy;
	d.placeplayer(ix,iy);
	*/
	d.placeplayer(rand()%63,rand()%31);// Place the player randomly on the field.
	cout<<"--House parameters--"<<endl;// Prompt for input.
	cout<<"# (int) : ";cin>>in;// Get input for number of houses.
	for (int i=0;i<in;i++)// For every time that a house is supposed to be spawned:
	{
		ix=3+rand()%+60;iy=3+rand()%28;// Generate random coordinates.
		iw=3+rand()%16;ih=3+rand()%8;// Generate random dimensions.
		switch (rand()%4)// Choose randomly between 4 cases:
		{
			case 0:id[0]='h';break;// left,
			case 1:id[0]='j';break;// down,
			case 2:id[0]='k';break;// up,
			case 3:id[0]='l';break;// right.
		}
		if (ix+iw<=62&&iy+ih<=30)// If the dimensions and coordinates do not go to/past the edge...
			r=d.house(ix,iy,iw,ih,id[0]);// ...spawn the house...
		else i--;// ...otherwise try again.
	}
	/*// DEPRECATED: Manual house placement
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
	cout<<"--Animal parameters--"<<endl;// Prompt for input.
	cout<<"# (int) : ";cin>>in;// Get input for number of animals.
	for (int i=0;i<in;i++)// For every time that an animal is to be created:
	{
		ix=rand()%64;iy=rand()%32;// Generate random coordinates.
		if (field[ix][iy].animal()==1)// Create the animal. If unsuccessful...
			i--;// ...try again.
	}
	d.cullwalls();// Recursively destroy walls flanked by floors.
	initscr();// Begin ncurses screen output.
	do// At least once, but repeatedly:
	{
		clear();// Clear the whole screen.
		refresh();// Refresh the screen to make changes.
		d.draw();// Recursively draw every tile.
		ic=getch();// Get unbuffered user input as char.
		if (ic=='q')// If the input was a Q:
			quit=true;// Plan to quit.
		else field[px][py].move(px,py,ic);// Otherwise, try to move the player in that direction.
		for (int x=0;x<64;x++)// For every tile...
			for (int y=0;y<32;y++)// ...
				field[x][y].rmove(x,y,1);// ...try to move each randomly.
	}
	while (quit==false);// (...while the game is not told to quit.)
	endwin();// When it is told to quit, end ncurses output.
	return 0;// Return success code zero.
}
