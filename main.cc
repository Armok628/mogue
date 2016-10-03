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
static char ic0,ic1,ic2;
static bool quit=false;
Display d;
int main(int argc,char *argv[])
{
	srand(time(0));// Seed the random number generator.
	int ix,iy,iw,ih,in,r;// Create local variables.
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
	dead=false;// The player is (obviously) not dead by default.
	d.placeplayer(rand()%64,rand()%32);// Place the player randomly on the field.
	if (argc>1)// If there are any arguments:
		sscanf(argv[1],"%d",&in);// The first argument should be the number of houses spawned.
	else// Otherwise:
	{
		cout<<"--House parameters--"<<endl;// Prompt for input.
		cout<<"# (int) : ";cin>>in;// Get input for number of houses.
	}
	for (int i=0;i<in;i++)// For every time that a house is supposed to be spawned:
	{
		ix=1+rand()%+62;iy=1+rand()%30;// Generate random coordinates.
		iw=3+rand()%16;ih=3+rand()%8;// Generate random dimensions.
		switch (rand()%4)// Choose randomly between four cases:
		{
			case 0:id[0]='h';break;// left,
			case 1:id[0]='j';break;// down,
			case 2:id[0]='k';break;// up,
			case 3:id[0]='l';break;// right.
		}
		if (ix+iw<=63&&iy+ih<=31)// If the dimensions and coordinates do not go to/past the edge...
			d.house(ix,iy,iw,ih,id[0]);// ...spawn the house...
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
	if (argc>2)// If there is more than one argument:
		sscanf(argv[2],"%d",&in);// The second argument should be the number of animals spawned.
	else// Otherwise:
	{
		cout<<"--Animal parameters--"<<endl;// Prompt for input.
		cout<<"# (int) : ";cin>>in;// Get input for number of animals.
	}
	for (int i=0;i<in;i++)// For every time that an animal is to be created:
	{
		ix=rand()%64;iy=rand()%32;// Generate random coordinates.
		if (field[ix][iy].bg=='#'||field[ix][iy].animal()==1)// Create the animal at a random location. If unsuccessful:
			i--;// ...try again.
	}
	if (argc>3)// If there are more than two arguments:
		sscanf(argv[3],"%d",&in);// The third argument should be the number of monsters spawned.
	else// Otherwise:
	{
		cout<<"--Monster parameters--"<<endl;// Prompt for input.
		cout<<"# (int) : ";cin>>in;// Get input for number of monsters.
	}
	for (int i=0;i<in;i++)// For every time that a monster is to be created:
	{
		ix=rand()%64;iy=rand()%32;// Generate random coordinates.
		if (field[ix][iy].bg!='#'||field[ix][iy].monster()==1)// Create the monster at those coordinates. If unsuccessful:
			i--;// ...try again
	}
	d.cullwalls();// Recursively destroy walls flanked by floors.
	d.makedoors();// Recursively create doors on suitable floors.
	int mc,ac;// Create new local variables to count creatures.
	mc=ac=0; // Initialize local counting variables.
	do {} while (field[rand()%64][rand()%32].wand()==1);// Keep trying to place a wand randomly on the field until success.
	haswand=false;// Initialize external wand ownership boolean.
	initscr();// Begin ncurses screen output.
	do// At least once, but repeatedly:
	{
		for (int ix=0;ix<64;ix++)// For every column...
			for (int iy=0;iy<32;iy++)// ...for every row...
				if (field[ix][iy].fg=='&')// ...if the tile is a monster:
					mc++;// Increase the monster count meter.
				else if (field[ix][iy].fg=='A')// If the tile is an animal:
					ac++;// Increase the animal count meter.
		clear();// Clear the whole screen.
		refresh();// Refresh the screen to make changes.
		cout<<"Monsters left: "<<mc<<endl<<'\r';mc=0;// Print the number of monsters left and reset the counter.
		cout<<"Animals left: "<<ac<<'\r';ac=0;// Print the number of animals left and reset the counter.
		d.draw();// Recursively draw every tile.
		for (int x=0;x<64;x++)// For every column:
			for (int y=0;y<32;y++)// For every row::
				if (field[x][y].fg=='A'||field[x][y].fg=='&')// If the tile is a monster or animal:
					field[x][y].m=true;// Allow it to move randomly.
				else// Otherwise:
					field[x][y].m=false;// Disallow random movement.
		ic0=getch();// Get unbuffered user input as char.
		if (ic0=='q')// If the input was a Q:
			break; // Quit immediately.
		else if (ic0=='m')// If the input was an M:
		{
			ic1=getch();// Get (second) unbuffered user input as char.
			ic2=getch();// Get (third) unbuffered user input as char.
			int nx=px+d.offset('x',ic2);// Make absolute x coordinate from player-relative x coordinate.
			int ny=py+d.offset('y',ic2);// Make absolute y coordinate from player-relative y coordinate.
			if (nx>-1&&nx<65&&ny>-1&&ny<33)// If the coordinates are within bounds:
			{
				if (ic1=='g')// If the second character is a g:
					field[nx][ny].grass();// Try to make the selected tile grass.
				if (ic1=='f')// If the second character is an f:
					field[nx][ny].floor(nx,ny);// Try to make the selected tile a floor.
				if (ic1=='w')// If the second character is a w:
					field[nx][ny].wall(nx,ny);// Try to make the selected tile a wall.
				if (ic1=='d')// If the second character is a d:
					field[nx][ny].door();// Try to make the selected tile a door.
				if (ic1=='&'&&haswand)// If the second character is & and the wand has been obtained:
					field[nx][ny].monster();// Try to make the selected tile a monster.
				if (ic1=='A'&&haswand)// If the second character is A and the wand has been obtained:
					field[nx][ny].animal();// Try to make the selected tile an animal.
			}
		}
		else if (!dead)// Otherwise, if the player is not dead:
			field[px][py].move(px,py,ic0);// Try to move the player in that direction.
		for (int x=0;x<64;x++)// For every tile...
			for (int y=0;y<32;y++)// ...
				field[x][y].rmove(x,y);// ...try to move each randomly.
	}
	while (true);// (...while the loop is not broken.)
	endwin();// When it is told to quit, end ncurses output.
	return 0;// Return success code zero.
}
