// To-do: Fix code formatting
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#define NEW(x) malloc(sizeof(x))
#define BETW(x,min,max) (min<x&&x<max)
#define WIDTH 80
#define HEIGHT 24
#define CHECKER(x) (x%2^(x/WIDTH%2))
#define AREA (WIDTH*HEIGHT)
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
void draw_pos(int pos,tile_t *zone);
void draw_board(tile_t *zone);
bool char_in_string(char c,char *string);
int dir_offset(char dir);
void set_fg (tile_t *tile,char fg,char *fg_c);
void set_bg (tile_t *tile,char bg,char *bg_c);
void set_tile (tile_t *tile,char fg,char *fg_c,char bg,char *bg_c);
tile_t *random_tile(tile_t *zone);
tile_t *random_grass(tile_t *zone);
tile_t *random_floor(tile_t *zone);
int abs(int n);
char move_tile(int pos,char dir,tile_t *zone);
void update (tile_t *zone);
bool try_summon(tile_t *tile,char fg,char *fg_c);
void randomly_place(char fg,char *fg_c,tile_t *zone);
void spawn_player(tile_t *zone,int *pc);
char move_player(char dir,int *pc,tile_t *zone);
void place_on_grass(char fg,char *fg_c,tile_t *zone);
void place_on_floor(char fg,char *fg_c,tile_t *zone);
void set_wall(tile_t *tile,int pos);
void set_floor(tile_t *tile,int pos);
void set_door(tile_t *tile);
void make_building(int pos,int h,int w,tile_t *zone);
void make_random_building(tile_t *zone);
void cull_walls(tile_t *zone);
void make_paths(tile_t *zone);
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
	tile_t *field=malloc(AREA*sizeof(tile_t))
		,*dungeon=malloc(AREA*sizeof(tile_t));
	int player_coords;
	// Initialize zone
	for (int i=0;i<AREA;i++) {
		set_bg(&field[i],grass_chars[rand()%5]
				,grass_colors[rand()%2]);
		set_tile(&dungeon[i],'%',dgray,'%',dgray);
	}
	// Parse command line arguments
	int buildings=20,monsters=20,animals=20,soldiers=20; // Defaults
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
	for (int i=0;i<buildings;i++) {
		make_random_building(field);
		make_random_building(dungeon);
	}
	cull_walls(field);
	cull_walls(dungeon);
	// Initialize test subjects
	for (int i=0;i<monsters;i++) {
		printf("Making monster %i\n",i);
		place_on_floor('&',dgray,field);
	}
	for (int i=0;i<animals;i++) {
		printf("Making animal %i\n",i);
		place_on_grass('A',yellow,field);
	}
	for (int i=0;i<soldiers;i++) {
		printf("Making soldier %i\n",i);
		place_on_floor('$',blue,field);
	}
	// Place special tiles (temporary)
	randomly_place('I',purple,field);
	set_bg(random_floor(field),'>',dgray);
	set_bg(random_floor(field),'<',dgray);
	// Initialize player
	spawn_player(field,&player_coords);
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
		if (input=='q')
			break;
		if (input=='z'&&has_scepter) {
			input=fgetc(stdin);
			int target=player_coords+dir_offset(input);
			update(field);
			try_summon(&field[target],'Z',teal);
			draw_pos(target,field);
			continue;
		}
		switch (move_player(input,&player_coords,field)) {
			case 'I':
				has_scepter=true;
				field[player_coords].fg_c=purple;
				draw_pos(player_coords,field);
				break;
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
void draw_pos(int pos,tile_t *zone)
{
	move_cursor(pos%WIDTH,pos/WIDTH);
	draw_tile(zone[pos]);
}
void draw_board(tile_t *zone)
{
	for (int i=0;i<AREA;i++)
		draw_pos(i,zone);
}
bool char_in_string(char c,char *string)
{
	for (;*string;string++)
		if (c==*string)
			return true;
	return false;
}
int dir_offset(char dir)
{
	int offset=0;
	if (char_in_string(dir,"kyuA789")) // North
		offset-=WIDTH;
	else if (char_in_string(dir,"jbnB123")) // South
		offset+=WIDTH;
	if (char_in_string(dir,"hybD147")) // West
		offset-=1;
	else if (char_in_string(dir,"lunC369")) // East
		offset+=1;
	return offset;
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
tile_t *random_tile(tile_t *zone)
{
	return &zone[rand()%AREA];
}
tile_t *random_grass(tile_t *zone)
{
	tile_t *tile;
	do 
		tile=random_tile(zone);
	while (tile->fg||!char_in_string(tile->bg,grass_chars));
	return tile;
}
tile_t *random_floor(tile_t *zone)
{
	tile_t *tile;
	do
		tile=random_tile(zone);
	while (tile->fg||tile->bg!='#');
	return tile;
}
int abs(int n)
{
	return n>0?n:-n;
}
char move_tile(int pos,char dir,tile_t *zone)
{
	int dest=pos+dir_offset(dir);
	if (abs((dest%WIDTH)-(pos%WIDTH))==WIDTH-1||0>dest||dest>AREA)
		return '\0';
	tile_t *from=&zone[pos],*to=&zone[dest];
	// Note: Describes only cases where movement is disallowed
	switch (from->fg) {
		case '%': // Wall
		case '+': // Door
		case 'I': // Scepter
		case '>': // Staircase (down)
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
	if (dest!=pos) {
		char killed='\''; // (Represents grass)
		// If the destination is a door
		if (to->fg=='+') {
			// Open it, not moving the creature
			set_fg(to,'\0',NULL);
			draw_pos(dest,zone);
			// Report failure to move
			return '\0';
		}
		// If there's something else at the destination:
		if (to->fg) {
			killed=to->fg; // Remember what was captured
			fprintf(debug_log,"%c moved into %c at [%i]\n"
					,from->fg,to->fg,dest);
			// If it's a creature, place a corpse
			if (char_in_string(to->fg,creatures))
				set_bg(to,to->fg,red);
		}
		// Move the symbol and color
		set_fg(to,from->fg,from->fg_c);
		set_fg(from,'\0',NULL);
		// Redraw the changed positions
		draw_pos(pos,zone);
		draw_pos(dest,zone);
		// Return the value of what was captured
		return killed;
	} else
		return '\0';
}
void update (tile_t *zone)
{
	// Make a matrix of the foreground characters
	char fgcopies[AREA];
	for (int i=0;i<AREA;i++)
		fgcopies[i]=zone[i].fg;
	// For each occupied space, if it hasn't moved yet, move it
	for (int i=0;i<AREA;i++)
		if (fgcopies[i]&&fgcopies[i]==zone[i].fg&&zone[i].fg!='@')
				move_tile(i,dirs[rand()%9],zone);
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
void randomly_place(char fg,char *fg_c,tile_t *zone)
{
	if (!try_summon(random_tile(zone),fg,fg_c))
		randomly_place(fg,fg_c,zone);
}
void spawn_player(tile_t *zone,int *pc)
{
	*pc=rand()%AREA;
	if (!try_summon(&zone[*pc],'@',lblue))
		spawn_player(zone,pc);
}
char move_player(char dir,int *pc,tile_t *zone)
{
	char result='\0';
	if (zone[*pc].fg!='@')
		return '\0';
	if (result=move_tile(*pc,dir,zone))
		*pc+=dir_offset(dir);
	return result;
}
void place_on_grass(char fg,char *fg_c,tile_t *zone)
{
	set_fg(random_grass(zone),fg,fg_c);
}
void place_on_floor(char fg,char *fg_c,tile_t *zone)
{
	set_fg(random_floor(zone),fg,fg_c);
}
void set_wall(tile_t *tile,int pos)
{
	set_tile(tile,'%',wall_colors[CHECKER(pos)]
			,'%',wall_colors[CHECKER(pos)]);
}
void set_floor(tile_t *tile,int pos)
{
	set_tile(tile,'\0',NULL,'#',floor_colors[CHECKER(pos)]);
}
void set_door(tile_t *tile)
{
	set_tile(tile,'+',brown,'-',brown);
}
void make_building(int pos,int w,int h,tile_t *zone)
{
	// Floors
	for (int y=pos;y<=pos+h*WIDTH;y+=WIDTH)
		for (int x=y;x<=y+w;x++)
			set_floor(&zone[x],x);
	// Horizontal walls
	for (int i=pos;i<=pos+w;i++) {
		set_wall(&zone[i],i);
		set_wall(&zone[i+h*WIDTH],i+h*WIDTH);
	}
	// Vertical walls
	for (int i=pos;i<=pos+h*WIDTH;i+=WIDTH) {
		set_wall(&zone[i],i);
		set_wall(&zone[i+w],i+w);
	}
	// Door
	switch (rand()%4) {
		case 0: // North
			set_door(&zone[pos+w/2]);
			break;
		case 1: // South
			set_door(&zone[pos+h*WIDTH+w/2]);
			break;
		case 2: // West
			set_door(&zone[pos+h/2*WIDTH]);
			break;
		case 3: // East
			set_door(&zone[pos+w+h/2*WIDTH]);
	}
}
void make_random_building(tile_t *zone)
{
	int w=3+rand()%(WIDTH/4),h=3+rand()%(HEIGHT/4);
	int pos=(1+rand()%(WIDTH-w-2))+(1+rand()%(HEIGHT-h-2))*WIDTH;
	make_building(pos,w,h,zone);
}
void cull_walls(tile_t *zone)
{
	// Beware: ugly formatting ahead
	int walls_removed;
	do {
		walls_removed=0;
		for (int i=0;i<AREA;i++)
			if (char_in_string(zone[i].fg,"%+")
					&&((zone[i+1].bg=='#'
					&&zone[i-1].bg=='#')
					||(zone[i+WIDTH].bg=='#'
					&&zone[i-WIDTH].bg=='#'))) {
				set_floor(&zone[i],i);
				walls_removed++;
			}
	} while (walls_removed>0);
}
void make_paths(tile_t *zone)
{
}
