#ifndef _DISPLAY
#define _DISPLAY
class Display
{
	public:
		void draw();
		void placeplayer(int x,int y);
		void cullwalls();
		void makedoors();
		int house(int x,int y,int w,int h,char d);
		int dungeon(int x,int y,int w,int h,char d);
		int offset(char a,char c);
};
#endif
