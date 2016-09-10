#ifndef _TILE_H
#define _TILE_H
class Tile
{
	public:
		Tile();
		Tile(char b,char f);
		void grass();
		void wall();
		void floor();
		int set(char c);
		int setc(char* c);
		char ret();
		char* retc();
		void move(char c);
		char bg,fg,*bc,*fc;
};
#endif
