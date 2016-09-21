#ifndef _TILE_H
#define _TILE_H
class Tile
{
	public:
		Tile();
		Tile(char b,char f,bool g);
		void grass();
		void wall();
		void floor();
		void animal();
		int set(char c);
		int setc(char* c);
		char ret();
		char* retc();
		void move(int x,int y,char c);
		void pmove(char c);
		void rmove(int x,int y);
		bool m;
		char bg,fg,*bc,*fc;
};
#endif
