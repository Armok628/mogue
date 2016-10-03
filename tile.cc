#include <cstdlib>
#include "main.h"
#include "tile.h"
#include "display.h"
static char _RESET_COLOR[]="\033[0m";
static char _GREEN[]="\033[0;32m";
static char _L_GREEN[]="\033[1;32m";
static char _D_GRAY[]="\033[1;30m";
static char _GRAY[]="\033[0;39m";
static char _WHITE[]="\033[1;39m";
static char _BROWN[]="\033[0;33m";
static char _YELLOW[]="\033[1;33m";
static char _RED[]="\033[0;31m";
static char _L_RED[]="\033[1;31m";
static char _PURPLE[]="\033[0;35m";
bool m,dead,haswand;
Tile::Tile()
{
	grass();// By default, each tile should be grass.
}
void Tile::grass()
{
	if (fg=='\0'||fg=='%'||fg=='+')// If the tile is not initialized or is already a wall or door:
		fg=' ';// Grass should have a blank foreground.
	switch(rand()%2)// Pick randomly between two choices:
	{
		case 0:bc=_GREEN;break;// light green,
		case 1:bc=_L_GREEN;break;// green.
	}
	switch(rand()%5)// Pick randomly between five choices:
	{
		case 0:bg='"';break;// quotation mark,
		case 1:bg='\'';break;// apostrophe,
		case 2:bg=',';break;// comma,
		case 3:bg='.';break;// period,
		case 4:bg='`';break;// grave (?).
	}
}
void Tile::rmove(int x,int y)
{
	if (m==true)// If the tile is supposed to move:
		switch (rand()%9)// Pick randomly between nine choices:
		{
			case 0:break;// not moving at all,
			case 1:move(x,y,'h');break;// moving West,
			case 2:move(x,y,'j');break;// moving South,
			case 3:move(x,y,'k');break;// moving North,
			case 4:move(x,y,'l');break;// moving East,
			case 5:move(x,y,'y');break;// moving Northwest,
			case 6:move(x,y,'u');break;// moving Northeast,
			case 7:move(x,y,'b');break;// moving Southwest,
			case 8:move(x,y,'n');break;// moving Southeast.
		}
}
void Tile::wall(int x,int y)
{
	if (fg!='@'&&fg!='A'&&fg!='&')// If this tile is not the player, an animal, or a monster:
	{
		bg=fg='%';// Set the background and foreground to the percent sign.
		if (x%2!=y%2)// In a checkerboard pattern, alternate between...
			bc=fc=_RED;// ...red...
		else bc=fc=_L_RED;// ...and light red.
	}
	else floor(x,y);// If it is, just set this tile to a floor.
}
void Tile::floor(int x,int y)
{
	bg='#';// Set the background to a floor.
	if (fg!='@'&&fg!='A'&&fg!='&')// If the foreground is not a person, animal, or monster:
	{
		fg=' ';// Clear the foreground character
		fc=_RESET_COLOR;// Clear the foreground color
	}
	if (x%2!=y%2)// In a checkerboard pattern, aleternate between...
		bc=_GRAY;// ...gray...
	else bc=_WHITE;// ...and light gray.
}
void Tile::door()
{
	if (fg!='@'&&fg!='A'&&fg!='&')// If the foreground is not a person, animal, or monster:
	{
		fg='+';// Set the foreground to a plus to represent a door.
		fc=_BROWN;// Set the foreground to brown.
	}
	bg='-';// The background of a door is pre-set to a minus symbol to reduce operations needed for opening.
	bc=_BROWN;// Doors will always be brown.
}
void Tile::open()
{
	fg=' ';// Remove the foreground to 'open' the door. The background is already set, so nothing else needs to be done.
}
void Tile::wand()
{
	fg='/';
	fc=_PURPLE;
}
int Tile::animal()
{
	if (fg==' ')// If the tile is clear:
	{
		fg='A';// This tile's foreground should be D (for deer).
		fc=_BROWN;// Animals should be displayed as brown.
		return 0;// Report success.
	}
	return 1;// Otherwise, report failure.
}
int Tile::monster()
{
	if (fg==' ')// If the tile is clear:
	{
		fg='&';// Set the foreground to M for monster.
		fc=_D_GRAY;// Set the monster's color to dark gray.
		return 0;// Report success.
	}
	return 1;// Otherwise, report failure.
}
void Tile::set(char c)
{
	fg=c;// Set the foreground to the parameter.
}
void Tile::setc(char* c)
{
	if (fg==' ')// If there is no foreground on this tile:
		bc=c;// Set the background color to the parameter.
	else fc=c;// Otherwise, set the foreground color to the parameter.
}
char Tile::ret()
{
	if (fg==' ')// If there is no foreground character:
		return bg;// Return the background character.
	return fg;// Otherwise, return the foreground character.
}
char* Tile::retc()
{
	if (fg==' ')// If there is no foreground character:
		return bc;// Return the background color.
	return fc;// Otherwise, return the foreground color.
}
int Tile::move(int x,int y,char c)
{
	if (field[x][y].fg==' ')// If the tile supposed to be moved is empty:
		return 1;// Report failure.
	int nx=x+d.offset('x',c);int ny=y+d.offset('y',c);// Create and set new X and new Y variables.
	if (nx==x&&ny==y)// If the tile moving would move to its same space:
		return 1;// Report failure.
	if (!((c=='h'||c=='y'||c=='b')&&x==0// If the player is not moving West at X equals zero...
			||(c=='j'||c=='b'||c=='n')&&y==31// ...nor South at Y equals thirty-one...
			||(c=='k'||c=='y'||c=='u')&&y==0// ...nor North at Y equals zero...
			||(c=='l'||c=='u'||c=='n')&&x==63)// ...nor East at X equals sixty-three...
			&&field[nx][ny].fg!='%')// ...and the tile to be moved to is not a wall:
	{
		if (field[nx][ny].fg=='+')// If the tile walked to is a door:
		{
			if (field[x][y].fg=='@'||field[x][y].fg=='&')// If the tile moving is a monster or the player:
				field[nx][ny].open();// Open the door.
			return 1;// Report failure. (As in, do not move the player in the same turn as opening.
		}
		if (field[nx][ny].fg=='A')// If the tile moved to is an animal:
		{
			if (field[x][y].fg=='A')// If the tile moving is also an animal:
				return 1;// Report failure.
			field[nx][ny].kill();// Otherwise, kill the animal.
		}
		if (field[nx][ny].fg=='&')// If the tile moved to is a monster:
		{
			if (field[x][y].fg=='A')// If the tile moving is an animal:
			{
				field[x][y].kill();// Kill the animal.
				return 1;// Report failure (for the animal, anyway).
			}
			if (field[x][y].fg=='@')// If the tile moving is the player:
				field[nx][ny].kill();// Kill the monster.
			if (field[x][y].fg=='&')// If the tile moving is a monster:
				return 1;// Report failure.
		}
		if (field[nx][ny].fg=='@')// If the tile moved to is the player:
		{
			if (field[x][y].fg=='A')// If the tile moving is an animal:
				return 1;// Report failure.
			if (field[x][y].fg=='&')// If the tile moving is a monster:
			{
				field[nx][ny].kill();// Kill the player.
				dead=true;// Let the records show that the forces of good are vanquished.
			}
		}
		if (field[nx][ny].fg=='/')// If the tile moved to is the wand:
		{
			if (field[x][y].fg=='@')// If the tile moving is the player:
			{
				haswand=true;// Set the wand ownership variable to true.
				field[x][y].fc=_PURPLE;// Set the player's color to purple.
			}
			else
				return 1;// Report failure.
		}
		field[nx][ny].fg=field[x][y].fg;// Set the new tile's foreground...
		field[nx][ny].fc=field[x][y].fc;// ...and set the new tile's color.
		if (field[nx][ny].fg=='@')// If the tile moved is the player:
		{px=nx;py=ny;}// Update the player coordinate index.
		field[x][y].fg=' ';// Set the old tile to its new vacant state.
	}
	return 0;// Report success.
}
void Tile::kill()
{
	bg=fg;// Set the background to whatever creature was there.
	bc=_RED;// Set the background to a blood color.
	m=false;// Disallow random movement.
	fg=' ';// Remove the foreground character.
}
