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
void draw_pos(int ypos,int xpos,tile_t field[WIDTH][HEIGHT]);
void draw_board(tile_t field[WIDTH][HEIGHT]);
bool char_in_string(char c,char *string);
int dir_offset(char axis,char dir);
void set_fg (tile_t *tile,char fg,char *fg_c);
void set_bg (tile_t *tile,char bg,char *bg_c);
void set_tile (tile_t *tile,char fg,char *fg_c,char bg,char *bg_c);
char move_tile(int xpos,int ypos,char dir,tile_t field[WIDTH][HEIGHT]);
void update (tile_t field[WIDTH][HEIGHT]);
bool try_summon(tile_t *tile,char fg,char *fg_c);
void randomly_place(char fg,char *fg_c,tile_t field[WIDTH][HEIGHT]);
void spawn_player(tile_t field[WIDTH][HEIGHT],int *playerx,int *playery);
char move_player(char dir,int *playerx,int *playery
		,tile_t field[WIDTH][HEIGHT]);
void place_on_grass(char fg,char *fg_c,tile_t field[WIDTH][HEIGHT]);
void place_on_floor(char fg,char *fg_c,tile_t field[WIDTH][HEIGHT]);
void place_wall(tile_t *tile,int x,int y);
void place_floor(tile_t *tile,int x,int y);
void place_door(tile_t *tile);
void place_building(int xpos,int ypos,int h,int w
		,tile_t field[WIDTH][HEIGHT]);
void place_random_building(tile_t field[WIDTH][HEIGHT]);
void cull_walls(tile_t field[WIDTH][HEIGHT]);
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
	*creatures="@&A$Z";
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
	static tile_t field[WIDTH][HEIGHT];
	int playerx,playery;
	playerx=rand()%WIDTH;
	playery=rand()%HEIGHT;
	// Initialize field
	for (int x=0;x<WIDTH;x++)
		for (int y=0;y<HEIGHT;y++) {
			field[x][y].bg=grass_chars[rand()%5];
			field[x][y].bg_c=grass_colors[rand()%2];
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
	spawn_player(field,&playerx,&playery);
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
		fprintf(debug_log,"Input registered: %i\n",input);
		fprintf(debug_log,"Player is at (%i, %i)\n",playerx,playery);
		if (input=='q')
			break;
		if (input=='z'&&has_scepter) {
			input=fgetc(stdin);
			int xtarget=playerx+dir_offset('x',input);
			int ytarget=playery+dir_offset('y',input);
			update(field);
			try_summon(&field[xtarget][ytarget],'Z',teal);
			draw_pos(xtarget,ytarget,field);
			continue;
		}
		switch (move_player(input,&playerx,&playery,field)) {
			case 'I':
				has_scepter=true;
				field[playerx][playery].fg_c=purple;
				draw_pos(playerx,playery,field);
		}
		update(field);
	}
	// Clean up terminal
	tcsetattr(0,TCSANOW,&old_term);
	printf("%s",reset_color);
	set_cursor_visibility(1);
	move_cursor(0,HEIGHT);
	// Exit (success)
	return 0;
}
// Function definitions
void clear_screen()
{
	printf("\e[2J");
}
void move_cursor(int x,int y)
{
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
void draw_pos(int xpos,int ypos,tile_t field[WIDTH][HEIGHT])
{
	move_cursor(xpos,ypos);
	draw_tile(field[xpos][ypos]);
}
void draw_board(tile_t field[WIDTH][HEIGHT])
{
	for (int x=0;x<WIDTH;x++)
		for (int y=0;y<HEIGHT;y++)
			draw_pos(x,y,field);
}
bool char_in_string(char c,char *string)
{
	for (;*string;string++)
		if (c==*string)
			return true;
	return false;
}
int dir_offset(char axis,char dir)
{
	switch (axis) {
		case 'x':
			if (char_in_string(dir,"hybD147"))
				return -1;
			if (char_in_string(dir,"lunC369"))
				return 1;
			return 0;
		case 'y':
			if (char_in_string(dir,"kyuA789"))
				return -1;
			if (char_in_string(dir,"jbnB123"))
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
char move_tile(int xpos,int ypos,char dir,tile_t field[WIDTH][HEIGHT])
{
	int xdest=xpos+dir_offset('x',dir),ydest=ypos+dir_offset('y',dir);
	if (0>xdest||xdest>WIDTH-1||0>ydest||ydest>HEIGHT-1)
		return '\0';
	tile_t *from=&field[xpos][ypos],*to=&field[xdest][ydest];
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
			break;
		case 'Z': // Zombie
			if (char_in_string(to->fg,"%Z@"))
				return '\0';
	}
	// If the destination is not the source
	if (xdest!=xpos||ydest!=ypos) {
		char killed='\''; // (Represents grass)
		// If the destination is a door
		if (to->fg=='+') {
			// Open it, not moving the creature
			set_fg(to,'\0',NULL);
			draw_pos(xdest,ydest,field);
			// Report failure to move
			return '\0';
		}
		// If there's something else at the destination:
		if (to->fg) {
			killed=to->fg; // Remember what was captured
			fprintf(debug_log,"%c moved into %c at [%i][%i]\n"
					,from->fg,to->fg,xdest,ydest);
			// If it's a creature, place a corpse
			if (char_in_string(to->fg,creatures))
				set_bg(to,to->fg,red);
		}
		// Move the symbol and color
		set_fg(to,from->fg,from->fg_c);
		set_fg(from,'\0',NULL);
		// Redraw the changed positions
		draw_pos(xpos,ypos,field);
		draw_pos(xdest,ydest,field);
		// Return the value of what was captured
		return killed;
	} else
		return '\0';
}
void update (tile_t field[WIDTH][HEIGHT])
{
	// Make a matrix of the foreground characters
	char fgcopies[WIDTH][HEIGHT];
	for (int x=0;x<WIDTH;x++)
		for (int y=0;y<HEIGHT;y++)
			fgcopies[x][y]=field[x][y].fg;
	// For each occupied space, if it hasn't moved yet, move it
	for (int x=0;x<WIDTH;x++)
		for (int y=0;y<HEIGHT;y++)
			if (fgcopies[x][y]&&fgcopies[x][y]==field[x][y].fg
					&&field[x][y].fg!='@') // Not player
				move_tile(x,y,dirs[rand()%9],field);
}
bool try_summon(tile_t *tile,char fg,char *fg_c)
{
	if (!tile->fg) {
		tile->fg=fg;
		tile->fg_c=fg_c;
		return true;
	}
	return false;
}
void randomly_place(char fg,char *fg_c,tile_t field[WIDTH][HEIGHT])
{
	int xpos=rand()%WIDTH,ypos=rand()%HEIGHT;
	if (!try_summon(&field[xpos][ypos],fg,fg_c))
		randomly_place(fg,fg_c,field);
}
void spawn_player(tile_t field[WIDTH][HEIGHT],int *px,int *py)
{
	*px=rand()%WIDTH;
	*py=rand()%HEIGHT;
	if (!try_summon(&field[*px][*py],'@',lblue))
		spawn_player(field,px,py);
}
char move_player(char dir,int *playerx,int *playery,tile_t field[WIDTH][HEIGHT])
{
	char result='\0';
	if (field[*playerx][*playery].fg!='@')
		return '\0';
	if (result=move_tile(*playerx,*playery,dir,field)) {
		*playerx+=dir_offset('x',dir);
		*playery+=dir_offset('y',dir);
	}
	return result;
}
void place_on_grass(char fg,char *fg_c,tile_t field[WIDTH][HEIGHT])
{
	int xpos=rand()%WIDTH,ypos=rand()%HEIGHT;
	if (!field[xpos][ypos].fg
			&&char_in_string(field[xpos][ypos].bg,grass_chars))
		set_fg(&field[xpos][ypos],fg,fg_c);
	else
		place_on_grass(fg,fg_c,field);
}
void place_on_floor(char fg,char *fg_c,tile_t field[WIDTH][HEIGHT])
{
	int xpos=rand()%WIDTH,ypos=rand()%HEIGHT;
	if (!field[xpos][ypos].fg
			&&field[xpos][ypos].bg=='#')
		set_fg(&field[xpos][ypos],fg,fg_c);
	else
		place_on_floor(fg,fg_c,field);
}
void place_wall(tile_t *tile,int x,int y)
{
	set_tile(tile,'%',wall_colors[CHECKER(x,y)]
			,'%',wall_colors[CHECKER(x,y)]);
}
void place_floor(tile_t *tile,int x,int y)
{
	set_tile(tile,'\0',NULL,'#',floor_colors[CHECKER(x,y)]);
}
void place_door(tile_t *tile)
{
	set_tile(tile,'+',brown,'-',brown);
}
void place_building(int xpos,int ypos,int w,int h,
		tile_t field[WIDTH][HEIGHT])
{
	for (int x=xpos;x<=xpos+w;x++) {
		for (int y=ypos;y<=ypos+h;y++)
			// Floors
			place_floor(&field[x][y],x,y);
		// West/East walls
		place_wall(&field[x][ypos],x,ypos);
		place_wall(&field[x][ypos+h],x,ypos+h);
	}
	for (int y=ypos;y<=ypos+h;y++) {
		// North/South walls
			place_wall(&field[xpos][y],xpos,y);
		place_wall(&field[xpos+w][y],xpos+w,y);
	}
	switch (rand()%4) {
		case 0: // North
			place_door(&field[xpos][ypos+h/2]);
			break;
		case 1: // South
			place_door(&field[xpos+w][ypos+h/2]);
			break;
		case 2: // West
			place_door(&field[xpos+w/2][ypos]);
			break;
		case 3: // East
			place_door(&field[xpos+w/2][ypos+h]);
	}
}
void place_random_building(tile_t field[WIDTH][HEIGHT])
{
	int width=3+rand()%(WIDTH/4),height=3+rand()%(HEIGHT/4);
	int x=1+rand()%(WIDTH-2-width),y=1+rand()%(HEIGHT-2-height);
	place_building(x,y,width,height,field);
}
void cull_walls(tile_t field[WIDTH][HEIGHT])
{
	// Beware: ugly formatting ahead
	int walls_removed;
	do { 
		walls_removed=0;
		for (int x=0;x<WIDTH;x++)
			for (int y=0;y<HEIGHT;y++)
				if (char_in_string(field[x][y].fg,"%+")
						&&((field[x+1][y].bg=='#'
						&&field[x-1][y].bg=='#')
						||(field[x][y-1].bg=='#'
						&&field[x][y+1].bg=='#'))) {
					place_floor(&field[x][y],x,y);
					walls_removed++;
				}
	} while (walls_removed>0);
}
