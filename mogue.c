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
void create_field(tile_t *field,int b,int m,int a,int s);
void create_dungeon(tile_t *dungeon,int b,int m);
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
	*pink="\e[1;35;38m",
    	*grass_colors[2]={"\e[0;32;38m","\e[1;32;38m"},
	*floor_colors[2]={"\e[1;37;38m","\e[0;38;38m"},
	*wall_colors[2]={"\e[0;31;38m","\e[1;31;38m"},
	*rock_colors[2]={"\e[0;37;38m","\e[1;30;38m"},
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
		,*dungeon=malloc(AREA*sizeof(tile_t))
		,*current_zone;
	int p_c;
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
	// Initialize field
	create_field(field,buildings,monsters,animals,soldiers);
	current_zone=field;
	// Initialize player
	spawn_player(current_zone,&p_c);
	// Draw board
	clear_screen();
	draw_board(current_zone);
	// Control loop
	char input;
	bool has_scepter=false;
	for (;;) {
		input=fgetc(stdin);
		if (input==27&&fgetc(stdin)==91)
			input=fgetc(stdin);
		fprintf(debug_log,"Input registered: \'%c\'\n",input);
		if (input=='q')
			break;
		if (input=='>'&&current_zone[p_c].bg=='>') {
			fprintf(debug_log,"Entering dungeon!\n");
			char *old_color=current_zone[p_c].fg_c;
			set_fg(&current_zone[p_c],'\0',NULL);
			create_dungeon(dungeon,buildings,monsters);
			current_zone=dungeon;
			for (p_c=0;current_zone[p_c].bg!='<';p_c++);
			set_fg(&current_zone[p_c],'@',old_color);
			draw_board(current_zone);
		} else if (input=='<'&&current_zone[p_c].bg=='<') {
			fprintf(debug_log,"Exiting dungeon!\n");
			char *old_color=current_zone[p_c].fg_c;
			for (p_c=0;field[p_c].bg!='>';p_c++);
			set_fg(&field[p_c],'@',old_color);
			draw_board(field);
			current_zone=field;
		} else if (input=='z'&&has_scepter) {
			fprintf(debug_log,"Summoning zombie!\n");
			int target=p_c+dir_offset(fgetc(stdin));
			update(current_zone);
			try_summon(&current_zone[target],'Z',teal);
			draw_pos(target,current_zone);
			continue;
		} else if (input=='O'&&has_scepter) {
			fprintf(debug_log,"Opening portal!\n");
			int target=p_c+dir_offset(fgetc(stdin));
			try_summon(&current_zone[target],'O',purple);
			draw_pos(target,current_zone);
		} else if (input=='R'&&has_scepter
				&&current_zone[p_c].fg!='@') {
			fprintf(debug_log,"Resurrecting player!\n");
			set_fg(&current_zone[p_c],'@',lblue);
			has_scepter=false;
			draw_pos(p_c,current_zone);
		}
		switch (move_player(input,&p_c,current_zone)) {
			case 'I':
				has_scepter=true;
				current_zone[p_c].fg_c=purple;
				draw_pos(p_c,current_zone);
				break;
			case 'O':
				fprintf(debug_log,"Entering portal!\n");
				char *old_color=current_zone[p_c].fg_c;
				create_field(field,buildings,monsters,
						animals,soldiers);
				fprintf(debug_log,"Spawning player...\n");
				spawn_player(field,&p_c);
				set_fg(&field[p_c],'@',old_color);
				draw_board(field);
				current_zone=field;
				fprintf(debug_log,"Done!\n");

		}
		update(current_zone);
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
	return n<0?-n:n;
}
char move_tile(int pos,char dir,tile_t *zone)
{
	int dest=pos+dir_offset(dir);
	if (abs((dest%WIDTH)-(pos%WIDTH))==WIDTH-1||0>dest||dest>AREA-1)
		return '\0';
	tile_t *from=&zone[pos],*to=&zone[dest];
	// Note: Describes only cases where movement is disallowed
	switch (from->fg) {
		case '%': // Wall
		case '+': // Door
		case 'I': // Scepter
		case 'O': // Portal
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
			// If it's a creature not on stairs, place a corpse
			if (char_in_string(to->fg,creatures)
					&&!char_in_string(to->bg,"<>"))
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
	char fgcopies[AREA],dir;
	for (int i=0;i<AREA;i++)
		fgcopies[i]=zone[i].fg;
	// For each occupied space, if it hasn't moved yet, move it
	for (int i=0;i<AREA;i++)
		if (fgcopies[i]&&fgcopies[i]==zone[i].fg&&zone[i].fg!='@')
			// Act based on collision
			switch (move_tile(i,dir=dirs[rand()%9],zone)) {
				case 'I':
					zone[i+dir_offset(dir)].fg_c=purple;
					draw_pos(i+dir_offset(dir),zone);
					break;
				case 'O':
					set_fg(&zone[i+dir_offset(dir)]
							,'\0',NULL);
					draw_pos(i+dir_offset(dir),zone);
			}
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
void create_field(tile_t *field,int b,int m,int a,int s)
{
	fprintf(debug_log,"Growing grass...\n");
	for (int i=0;i<AREA;i++)
		set_tile(&field[i],'\0',NULL,grass_chars[rand()%5]
				,grass_colors[rand()%2]);
	fprintf(debug_log,"Building buildings...\n");
	for (int i=0;i<b;i++)
		make_random_building(field);
	fprintf(debug_log,"Culling walls...\n");
	cull_walls(field);
	fprintf(debug_log,"Creating monsters...\n");
	for (int i=0;i<m;i++)
		place_on_floor('&',dgray,field);
	fprintf(debug_log,"Breeding animals...\n");
	for (int i=0;i<a;i++)
		place_on_grass('A',yellow,field);
	fprintf(debug_log,"Training soldiers...\n");
	for (int i=0;i<s;i++)
		place_on_floor('$',blue,field);
	fprintf(debug_log,"Digging stairwell...\n");
	set_bg(random_floor(field),'>',brown);
	fprintf(debug_log,"Opening portal...");
	place_on_grass('O',purple,field);
	fprintf(debug_log,"Done!\n");
}
void create_dungeon(tile_t *dungeon,int b,int m)
{
	fprintf(debug_log,"Placing rocks...\n");
	for (int i=0;i<AREA;i++)
		set_tile(&dungeon[i],'%',rock_colors[rand()%2],'%',dgray);
	fprintf(debug_log,"Carving rooms...\n");
	for (int i=0;i<b;i++)
		make_random_building(dungeon);
	fprintf(debug_log,"Culling walls...\n");
	cull_walls(dungeon);
	fprintf(debug_log,"Creating monsters...\n");
	for (int i=0;i<m;i++)
		place_on_floor('&',dgray,dungeon);
	fprintf(debug_log,"Digging stairwell...\n");
	set_bg(random_floor(dungeon),'<',brown);
	fprintf(debug_log,"Crafting scepter...\n");
	randomly_place('I',purple,dungeon);
	fprintf(debug_log,"Done!\n");
}
