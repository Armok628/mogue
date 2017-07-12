// To-do: Fix code formatting
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#define NEW(x) malloc(sizeof(x))
#define BETW(x,min,max) (min<x&&x<max)
#define CHECKER(x,y) ((x%2==0)^(y%2==0))
#define WIDTH 80
#define HEIGHT 24
// bool type definition
typedef enum {false,true} bool;
// Tile type definition
typedef struct tile_t {
	char fg,bg,*fg_c,*bg_c;
} tile_t;
// Function prototypes
void clear_screen();
void move_cursor(int x,int y);
void set_cursor_visibility(int visible);
void draw_tile(tile_t tile);
void draw_pos(int ypos,int xpos,tile_t field[HEIGHT][WIDTH]);
void draw_board(tile_t field[HEIGHT][WIDTH]);
int char_in_string(char c,char *string);
int dir_offset(char axis,char dir);
void set_fg (tile_t *tile,char fg,char *fg_c);
void set_bg (tile_t *tile,char bg,char *bg_c);
void set_tile (tile_t *tile,char fg,char *fg_c,char bg,char *bg_c);
char move_tile(int ypos,int xpos,char dir,tile_t field[HEIGHT][WIDTH]);
void update (tile_t field[HEIGHT][WIDTH]);
void randomly_place(char fg,char *fg_c,tile_t field[HEIGHT][WIDTH]);
void spawn_player(tile_t field[HEIGHT][WIDTH],int *playery,int *playerx);
char move_player(char dir,int *playery,int *playerx
		,tile_t field[HEIGHT][WIDTH]);
void place_on_grass(char fg,char *fg_c,tile_t field[HEIGHT][WIDTH]);
void place_on_floor(char fg,char *fg_c,tile_t field[HEIGHT][WIDTH]);
void place_wall(tile_t *tile,int y,int x);
void place_floor(tile_t *tile,int y,int x);
void place_door(tile_t *tile);
void place_building(int ypos,int xpos,int h,int w
		,tile_t field[HEIGHT][WIDTH]);
void place_random_building(tile_t field[HEIGHT][WIDTH]);
void cull_walls(tile_t field[HEIGHT][WIDTH]);
// Global definitions
static char
    	*reset_color="\e[0;38;38m",
	*red="\e[0;31;38m",
    	*lblue="\e[1;34;38m",
    	*blue="\e[0;34;38m",
    	*brown="\e[0;33;38m",
	*yellow="\e[1;33;38m",
	*dgray="\e[1;30;38m",
	*white="\e[1;37;38m",
	*teal="\e[0;36;38m",
	*purple="\e[0;35;38m",
    	*grass_colors[2]={"\e[0;32;38m","\e[1;32;38m"},
	*floor_colors[2]={"\e[0;37;38m","\e[1;37;38m"},
	*wall_colors[2]={"\e[0;31;38m","\e[1;31;38m"},
	*grass_chars="\"\',.`",
	*dirs="hjklyubn.",
	*creatures="@&A$";
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
	set_cursor_visibility(0);
	// Variable definitions
	static tile_t field[HEIGHT][WIDTH];
	int playerx,playery;
	playery=rand()%HEIGHT;
	playerx=rand()%WIDTH;
	// Initialize field
	for (int y=0;y<HEIGHT;y++)
		for (int x=0;x<WIDTH;x++) {
			field[y][x].bg=grass_chars[rand()%5];
			field[y][x].bg_c=grass_colors[rand()%2];
		}
	// Parse command line arguments
	int buildings=20,monsters=20,animals=20,soldiers=20;
	switch (argc)
	{
		case 5:
			sscanf(argv[4],"%i",&soldiers);
		case 4:
			sscanf(argv[3],"%i",&animals);
		case 3:
			sscanf(argv[2],"%i",&monsters);
		case 2:
			sscanf(argv[1],"%i",&buildings);
	}
	// Create a test house
	for (int i=0;i<buildings;i++)
		place_random_building(field);
	cull_walls(field);
	// Initialize test subjects
	for (int i=0;i<monsters;i++)
		place_on_floor('&',dgray,field);
	for (int i=0;i<animals;i++)
		place_on_grass('A',yellow,field);
	for (int i=0;i<soldiers;i++)
		place_on_floor('$',blue,field);
	// Initialize player
	spawn_player(field,&playery,&playerx);
	// Place item (temporary)
	randomly_place('I',purple,field);
	// Draw board
	clear_screen();
	draw_board(field);
	// Control loop
	char input;
	bool has_scepter=false;
	for (;;) {
		input=fgetc(stdin);
		if (input==27&&fgetc(stdin)==91)
			input=fgetc(stdin);
		fprintf(debug_log,"Movement registered: %i\n",input);
		if (input=='q')
			break;
		switch (move_player(input,&playery,&playerx,field)) {
			case 'I':
				has_scepter=true;
				field[playery][playerx].fg_c=purple;
				draw_pos(playery,playerx,field);
		}
		update(field);
	}
	// Clean up terminal
	tcsetattr(0,TCSANOW,&old_term);
	printf("%s",reset_color);
	set_cursor_visibility(1);
	move_cursor(HEIGHT,0);
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
void set_cursor_visibility(int visible)
{
	if (visible)
		printf("\e[?25h");
	else
		printf("\e[?25l");
}
void draw_tile(tile_t tile)
{
	printf("%s%c",tile.fg?tile.fg_c:tile.bg_c
			,tile.fg?tile.fg:tile.bg);
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
}
int char_in_string(char c,char *string)
{
	for (;*string;string++)
		if (c==*string)
			return 1;
	return 0;
}
int dir_offset(char axis,char dir)
{
	switch (axis) {
		case 'y':
			if (char_in_string(dir,"kyuA789"))
				return -1;
			if (char_in_string(dir,"jbnB123"))
				return 1;
			return 0;
		case 'x':
			if (char_in_string(dir,"hybD147"))
				return -1;
			if (char_in_string(dir,"lunC369"))
				return 1;
		default:
			return 0;
	}
}
void set_fg (tile_t *tile,char fg,char *fg_c)
{
	tile->fg=fg;
	tile->fg_c=fg_c;
}
void set_bg (tile_t *tile,char bg,char *bg_c)
{
	tile->bg=bg;
	tile->bg_c=bg_c;
}
void set_tile (tile_t *tile,char fg,char *fg_c,char bg,char *bg_c)
{
	set_fg(tile,fg,fg_c);
	set_bg(tile,bg,bg_c);
}
char move_tile(int ypos,int xpos,char dir,tile_t field[HEIGHT][WIDTH])
{
	if (field[ypos][xpos].fg=='%')
		return '\0';
	int ydest=ypos+dir_offset('y',dir),xdest=xpos+dir_offset('x',dir);
	if (0>ydest||ydest>HEIGHT-1||0>xdest||xdest>WIDTH-1)
		return '\0';
	tile_t *from=&field[ypos][xpos],*to=&field[ydest][xdest];
	// Note: Describes only cases where movement is disallowed
	switch (from->fg) {
		case '%': // Wall
		case '+': // Door
		case 'I': // Scepter
			return '\0'; // Always disallowed
		case '@': // Player
			if (char_in_string(to->fg,"%$"))
				return '\0';
			break;
		case '&': // Monster
			if (char_in_string(to->fg,"%&"))
				return '\0';
			break;
		case '$': // Soldier
			if (char_in_string(to->fg,"%$@A"))
				return '\0';
			break;
		case 'A': // Animal
			if (char_in_string(to->fg,"%@$&A"))
				return '\0';
	}
	// If the destination is not the source
	if (ydest!=ypos||xdest!=xpos) {
		char killed='\''; // (Represents grass)
		// If the destination is a door
		if (to->fg=='+') {
			// Open it, not moving the creature
			set_fg(to,'\0',NULL);
			draw_pos(ydest,xdest,field);
			// Report failure to move
			return '\0';
		}
		// If there's something else at the destination:
		if (to->fg) {
			killed=to->fg; // Remember what was captured
			fprintf(debug_log,"%c moved to %c at [%i][%i]\n"
					,from->fg,to->fg,ydest,xdest);
			// If it's a creature, place a corpse
			if (char_in_string(to->fg,creatures))
				set_bg(to,to->fg,red);
		}
		// Move the symbol and color
		set_fg(to,from->fg,from->fg_c);
		set_fg(from,'\0',NULL);
		// Redraw the changed positions
		draw_pos(ypos,xpos,field);
		draw_pos(ydest,xdest,field);
		// Return the value of what was captured
		return killed;
	} else
		return '\0';
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
			if (fgcopies[y][x]&&fgcopies[y][x]==field[y][x].fg
					&&field[y][x].fg!='@') // Not player
				move_tile(y,x,dirs[rand()%9],field);
}
void randomly_place(char fg,char *fg_c,tile_t field[HEIGHT][WIDTH])
{
	int ypos=rand()%HEIGHT,xpos=rand()%WIDTH;
	if (!field[ypos][xpos].fg)
		set_fg(&field[ypos][xpos],fg,fg_c);
	else
		randomly_place(fg,fg_c,field);
}
void spawn_player(tile_t field[HEIGHT][WIDTH],int *py,int *px)
{
	*py=rand()%HEIGHT;
	*px=rand()%WIDTH;
	if (!field[*py][*px].fg)
		set_fg(&field[*py][*px],'@',lblue);
	else
		spawn_player(field,py,px);
}
char move_player(char dir,int *playery,int *playerx,tile_t field[HEIGHT][WIDTH])
{
	char result='\0';
	if (field[*playery][*playerx].fg!='@')
		return '\0';
	if (result=move_tile(*playery,*playerx,dir,field)) {
		*playery+=dir_offset('y',dir);
		*playerx+=dir_offset('x',dir);
	}
	return result;
}
void place_on_grass(char fg,char *fg_c,tile_t field[HEIGHT][WIDTH])
{
	int ypos=rand()%HEIGHT,xpos=rand()%WIDTH;
	if (!field[ypos][xpos].fg
			&&char_in_string(field[ypos][xpos].bg,grass_chars))
		set_fg(&field[ypos][xpos],fg,fg_c);
	else
		place_on_grass(fg,fg_c,field);
}
void place_on_floor(char fg,char *fg_c,tile_t field[HEIGHT][WIDTH])
{
	int ypos=rand()%HEIGHT,xpos=rand()%WIDTH;
	if (!field[ypos][xpos].fg
			&&field[ypos][xpos].bg=='#')
		set_fg(&field[ypos][xpos],fg,fg_c);
	else
		place_on_floor(fg,fg_c,field);
}
void place_wall(tile_t *tile,int y,int x)
{
	set_tile(tile,'%',wall_colors[CHECKER(y,x)]
			,'%',wall_colors[CHECKER(y,x)]);
}
void place_floor(tile_t *tile,int y,int x)
{
	set_tile(tile,'\0',NULL,'#',floor_colors[CHECKER(y,x)]);
}
void place_door(tile_t *tile)
{
	set_tile(tile,'+',brown,'-',brown);
}
void place_building(int ypos,int xpos,int h,int w,
		tile_t field[HEIGHT][WIDTH])
{
	for (int y=ypos;y<=ypos+h;y++) {
		for (int x=xpos;x<=xpos+w;x++)
			// Floors
			place_floor(&field[y][x],y,x);
		// West/East walls
		place_wall(&field[y][xpos],y,xpos);
		place_wall(&field[y][xpos+w],y,xpos+w);
	}
	for (int x=xpos;x<=xpos+w;x++) {
		// North/South walls
			place_wall(&field[ypos][x],ypos,x);
		place_wall(&field[ypos+h][x],ypos+h,x);
	}
	switch (rand()%4) {
		case 0: // North
			place_door(&field[ypos][xpos+w/2]);
			break;
		case 1: // South
			place_door(&field[ypos+h][xpos+w/2]);
			break;
		case 2: // West
			place_door(&field[ypos+h/2][xpos]);
			break;
		case 3: // East
			place_door(&field[ypos+h/2][xpos+w]);
	}
}
void place_random_building(tile_t field[HEIGHT][WIDTH])
{
	int height=3+rand()%(HEIGHT/4),width=3+rand()%(WIDTH/4);
	int y=1+rand()%(HEIGHT-2-height),x=1+rand()%(WIDTH-2-width);
	place_building(y,x,height,width,field);
}
void cull_walls(tile_t field[HEIGHT][WIDTH])
{
	// Beware: ugly formatting ahead
	int walls_removed;
	do { 
		walls_removed=0;
		for (int y=0;y<HEIGHT;y++)
			for (int x=0;x<WIDTH;x++)
				if (char_in_string(field[y][x].fg,"%+")
						&&((field[y+1][x].bg=='#'
						&&field[y-1][x].bg=='#')
						||(field[y][x-1].bg=='#'
						&&field[y][x+1].bg=='#'))) {
					place_floor(&field[y][x],y,x);
					walls_removed++;
				}
	} while (walls_removed>0);
}
