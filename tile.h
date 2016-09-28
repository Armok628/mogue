#ifndef _TILE_H
#define _TILE_H
class Tile
{
	public:
		Tile();
		void grass();
		void wall(int x,int y);
		void floor(int x,int y);
		int animal();
		int monster();
		void set(char c);
		void setc(char* c);
		char ret();
		char* retc();
		int move(int x,int y,char c);
		void rmove(int x,int y,int c);
		void kill();
		bool m;
		char bg,fg,*bc,*fc;
};
#endif
