#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#define NEW(x) malloc(sizeof(x))
#define BETW(x, min, max) (min<x&&x<max)
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
int dir_offset(int axis,char dir);
void move_tile(int ypos,int xpos,char dir,tile_t field[HEIGHT][WIDTH]);
void update (tile_t field[HEIGHT][WIDTH]);
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
	field[10][10].fg='&';
	field[10][10].fg_c=monster_color;
	// Draw board
	clear_screen();
	draw_board(field);
	// Control loop
	char input;
	for (;;) {
		input=fgetc(stdin);
		if (input=='q')
			break;
		if (BETW(player_y+dir_offset(0,input),-1,HEIGHT)&&
				BETW(player_x+dir_offset(1,input),-1,WIDTH)) {
			move_tile(player_y,player_x,input,field);
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
int dir_offset(int axis,char dir)
{
	if (!axis) { // Y-axis (0)
		switch (dir) {
			// Up
			case 'k':
			case 'y':
			case 'u':
				return -1;
			// Down
			case 'j':
			case 'b':
			case 'n':
				return 1;
			// Nothing
			default:
				return 0;
		}
	} else { // X-axis (1)
		switch (dir) {
			// Left
			case 'h':
			case 'y':
			case 'b':
				return -1;
			// Right
			case 'l':
			case 'u':
			case 'n':
				return 1;
			// Nothing
			default:
				return 0;
		}
	}
}
void move_tile(int ypos,int xpos,char dir,tile_t field[HEIGHT][WIDTH])
{
	/////////////// To-do: ABSTRACT PATTERNS
	if (field[ypos][xpos].fg=='%')
		return;
	int ydest=ypos+dir_offset(0,dir),xdest=xpos+dir_offset(1,dir);
	tile_t *from=&field[ypos][xpos],*to=&field[ydest][xdest];
	// Note: Describes only cases where movement is disallowed
	switch (from->fg) {
		case '@': // Player
			switch (to->fg) {
				case '%': // Wall
				case '$': // Soldier
					return;
			}
			break;
		case '&': // Monster
			switch (to->fg) {
				case '%': // Wall
				case '&': // Monster
					return;
			}
			break;
		case '$': // Soldier
			switch (to->fg) {
				case '%': // Wall
				case '$': // Soldier
				case '@': // Player
				case 'A': // Animal
					return;
			}
			break;
		case 'A': // Animal
			switch (to->fg) {
				case '%': // Wall
				case '@': // Player
				case '$': // Soldier
				case '&': // Monster
					return;
			}
	}
	if ((BETW(ydest,-1,HEIGHT)&&BETW(xdest,-1,WIDTH))
			&&(ydest!=ypos||xdest!=xpos)) {
		fprintf(debug_log,"Moving %c to %d,%d\n",from->fg,ydest,xdest);
		if (to->fg) {
			to->bg=to->fg;
			to->bg_c=blood_color;
		}
		to->fg=from->fg;
		to->fg_c=from->fg_c;
		from->fg='\0';
		from->fg_c=NULL;
		draw_pos(ypos,xpos,field);
		draw_pos(ydest,xdest,field);
		reset_cursor();
	}
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
					// Exceptions
					field[y][x].fg!='@'&&
					field[y][x].fg!='%')
				move_tile(y,x,dirs[rand()%9],field);
}
/*
void place_building(int ypos,int xpos,int height,int width,
	tile_t field[HEIGHT][WIDTH])
{
	for (int x=xpos;x<xpos+width;x++) {
		field[ypos][x].fg='%';
		field[ypos+height][x].fg='%';
		// To-do: color
	}
	for (int y=ypos;y<ypos+height;y++) {
		for (int x=xpos+1;x<xpos+width-1;x++) {
			///////// To-do
		}
	}
	// To-do: add door
}
*/
