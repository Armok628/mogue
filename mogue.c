#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#define NEW(x) malloc(sizeof(x))
#define BETW(x,min,max) (min<x&&x<max)
#define CHECKER(x,y) ((x%2==0)^(y%2==0))
#define WIDTH 80
#define HEIGHT 23
// Tile type definition
typedef struct tile_t {
	char fg,bg,*fg_c,*bg_c;
} tile_t;
// Function prototypes
void clear_screen();
void move_cursor(int x,int y);
void reset_cursor();
void draw_tile(tile_t tile);
void draw_pos(int ypos,int xpos,tile_t field[HEIGHT][WIDTH]);
void draw_board(tile_t field[HEIGHT][WIDTH]);
int char_in_string(char c,char *string);
int dir_offset(int axis,char dir);
int move_tile(int ypos,int xpos,char dir,tile_t field[HEIGHT][WIDTH]);
void update (tile_t field[HEIGHT][WIDTH]);
void randomly_place(char fg,char *fg_c,tile_t field[HEIGHT][WIDTH]);
void place_building(int ypos,int xpos,int h,int w,
		tile_t field[HEIGHT][WIDTH]);
// Global definitions
static char
    	*reset_color="\e[0;38;38m",
	*blood_color="\e[0;31;38m",
    	*player_color="\e[1;34;38m",
    	*door_color="\e[0;33;38m",
	*animal_color="\e[1;33;38m",
	*monster_color="\e[0;37;38m",
    	*grass_colors[2]={"\e[0;32;38m","\e[1;32;38m"},
	*floor_colors[2]={"\e[0;37;38m","\e[1;37;38m"},
	*wall_colors[2]={"\e[0;31;38m","\e[1;31;38m"},
	grass_chars[5]="\"\',.`",
	dirs[9]="hjklyubn.";
static FILE *debug_log;
// Main function
int main(int argc,char **argv)
{
	// Open the debug log
	debug_log=fopen("debug.log","w+");
	// Seed the RNG
	srand(time(NULL));
	// Set terminal attributes
	struct termios old_term,new_term;
	tcgetattr(0,&old_term);
	new_term=old_term;
	new_term.c_lflag&=(~ICANON&~ECHO);
	tcsetattr(0,TCSANOW,&new_term);
	// Variable definitions
	static int player_x,player_y;
	static tile_t field[HEIGHT][WIDTH];
	player_y=rand()%HEIGHT;
	player_x=rand()%WIDTH;
	// Initialize field
	for (int y=0;y<HEIGHT;y++)
		for (int x=0;x<WIDTH;x++) {
			field[y][x].bg=grass_chars[rand()%5];
			field[y][x].bg_c=grass_colors[rand()%2];
		}
	// Initialize player
	field[player_y][player_x].fg='@';
	field[player_y][player_x].fg_c=player_color;
	// Initialize a test subject
	randomly_place('&',monster_color,field);
	// Create a test house
	place_building(5,5,10,10,field);
	// Draw board
	clear_screen();
	draw_board(field);
	// Control loop
	char input;
	for (;;) {
		input=fgetc(stdin);
		if (input=='q')
			break;
		if (move_tile(player_y,player_x,input,field))
		{
			player_y+=dir_offset(0,input);
			player_x+=dir_offset(1,input);
		}
		update(field);
	}
	// Clean up terminal
	tcsetattr(0,TCSANOW,&old_term);
	printf("%s",reset_color);
	// Exit (success)
	return 0;
}
// Function definitions
void clear_screen()
{
	printf("\e[2J");
}
void move_cursor(int y,int x)
{
	// To-do: Change the coordinate system to being [x][y]
	// Let this function alone deal with the weird conventions
	printf("\e[%d;%dH",y+1,x+1);
}
void reset_cursor()
{
	move_cursor(HEIGHT,0);
}
void draw_tile(tile_t tile)
{
	printf("%s%c",tile.fg?tile.fg_c:tile.bg_c,
			tile.fg?tile.fg:tile.bg);
}
void draw_pos(int ypos,int xpos,tile_t field[HEIGHT][WIDTH])
{
	move_cursor(ypos,xpos);
	draw_tile(field[ypos][xpos]);
}
void draw_board(tile_t field[HEIGHT][WIDTH])
{
	for (int y=0;y<HEIGHT;y++)
		for (int x=0;x<WIDTH;x++)
			draw_pos(y,x,field);
	reset_cursor();
}
int char_in_string(char c,char *string)
{
	for (;*string;string++)
		if (c==*string)
			return 1;
	return 0;
}
int dir_offset(int axis,char dir)
{
	if (!axis) { // Y-axis (0)
		if (char_in_string(dir,"kyu")) // Up
			return -1;
		else if (char_in_string(dir,"jbn")) // Down
			return 1;
		else // Neither
			return 0;
	} else { // X-axis (1)
		if (char_in_string(dir,"hyb")) // Left
			return -1;
		else if (char_in_string(dir,"lun")) // Right
			return 1;
		else // Neither
			return 0;
	}	
	return 0;
}
int move_tile(int ypos,int xpos,char dir,tile_t field[HEIGHT][WIDTH])
{
	/////////////// To-do: ABSTRACT PATTERNS
	if (field[ypos][xpos].fg=='%')
		return 0;
	int ydest=ypos+dir_offset(0,dir),xdest=xpos+dir_offset(1,dir);
	if (0>ydest||ydest>HEIGHT-1||0>xdest||xdest>WIDTH-1)
		return 0;
	tile_t *from=&field[ypos][xpos],*to=&field[ydest][xdest];
	// Note: Describes only cases where movement is disallowed
	switch (from->fg) {
		case '%': // Wall
		case '+': // Door
			return 0; // Always disallowed
		case '@': // Player
			if (char_in_string(to->fg,"%$"))
				return 0;
			break;
		case '&': // Monster
			if (char_in_string(to->fg,"%&"))
				return 0;
			break;
		case '$': // Soldier
			if (char_in_string(to->fg,"%$@A"))
				return 0;
			break;
		case 'A': // Animal
			if (char_in_string(to->fg,"%@$&"))
				return 0;
	}
	// If the destination is not the source
	if (ydest!=ypos||xdest!=xpos) {
		// If the destination is a door
		if (to->fg=='+') {
			// Open it, not moving the creature
			to->fg='\0';
			to->fg_c=NULL;
			draw_pos(ydest,xdest,field);
			reset_cursor();
			// Report failure to move
			return 0;
		}
		// If there's something else at the destination
		if (to->fg) {
			// Kill it
			to->bg=to->fg;
			to->bg_c=blood_color;
		}
		// Move the symbol and color
		to->fg=from->fg;
		to->fg_c=from->fg_c;
		from->fg='\0';
		from->fg_c=NULL;
		// Redraw the changed positions
		draw_pos(ypos,xpos,field);
		draw_pos(ydest,xdest,field);
		reset_cursor();
		// Report success
		return 1;
	} else
		return 0;
}
void update (tile_t field[HEIGHT][WIDTH])
{
	// Make a matrix of the foreground characters
	char fgcopies[HEIGHT][WIDTH];
	for (int y=0;y<HEIGHT;y++)
		for (int x=0;x<WIDTH;x++)
			fgcopies[y][x]=field[y][x].fg;
	// For each occupied space, if it hasn't moved yet, move it
	for (int y=0;y<HEIGHT;y++)
		for (int x=0;x<WIDTH;x++)
			if (fgcopies[y][x]&&fgcopies[y][x]==field[y][x].fg&&
					field[y][x].fg!='@') // Not player
				move_tile(y,x,dirs[rand()%9],field);
}
void randomly_place(char fg,char *fg_c,tile_t field[HEIGHT][WIDTH])
{
	int ypos=rand()%HEIGHT,xpos=rand()%WIDTH;
	field[ypos][xpos].fg=fg;
	field[ypos][xpos].fg_c=fg_c;
}
void place_building(int ypos,int xpos,int h,int w,
		tile_t field[HEIGHT][WIDTH])
{
	/////////////// To-do: ABSTRACT PATTERNS
	for (int x=xpos;x<=xpos+w;x++) {
		field[ypos][x].fg='%';
		field[ypos][x].fg_c=wall_colors[CHECKER(ypos,x)];
		field[ypos+h][x].fg='%';
		field[ypos+h][x].fg_c=wall_colors[CHECKER(ypos+h,x)];
	}
	for (int y=ypos;y<=ypos+h;y++) {
		field[y][xpos].fg='%';
		field[y][xpos].fg_c=wall_colors[CHECKER(y,xpos)];
		field[y][xpos+w].fg='%';
		field[y][xpos+w].fg_c=wall_colors[CHECKER(y,xpos+w)];
		for (int x=xpos;x<=xpos+w;x++) {
			field[y][x].bg='#';
			field[y][x].bg_c=floor_colors[CHECKER(y,x)];
		}
	}
	switch (rand()%4) {
		case 0: // Up
			field[ypos][xpos+w/2].fg='+';
			field[ypos][xpos+w/2].bg='-';
			field[ypos][xpos+w/2].fg_c=door_color;
			field[ypos][xpos+w/2].bg_c=door_color;
			break;
		case 1: // Down
			field[ypos+h][xpos+w/2].fg='+';
			field[ypos+h][xpos+w/2].bg='-';
			field[ypos+h][xpos+w/2].fg_c=door_color;
			field[ypos+h][xpos+w/2].bg_c=door_color;
			break;
		case 2: // Left
			field[ypos+h/2][xpos].fg='+';
			field[ypos+h/2][xpos].bg='-';
			field[ypos+h/2][xpos].fg_c=door_color;
			field[ypos+h/2][xpos].bg_c=door_color;
			break;
		case 3: // Right
			field[ypos+h/2][xpos+w].fg='+';
			field[ypos+h/2][xpos+w].bg='-';
			field[ypos+h/2][xpos+w].fg_c=door_color;
			field[ypos+h/2][xpos+w].bg_c=door_color;
			break;
	}
}
