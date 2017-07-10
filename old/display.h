#ifndef _DISPLAY
#define _DISPLAY
class Display
{
	public:
		void draw();
		void placeplayer(int x,int y);
		void cullwalls();
		void makedoors();
		void floorline(int x1,int y1,int x2,int y2,bool inc);
		void makepaths();
		int house(int x,int y,int w,int h,char d);
		int dungeon(int x,int y,int w,int h,char d);
		int offset(char a,char c);
};
#endif
