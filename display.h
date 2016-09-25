#ifndef _DISPLAY
#define _DISPLAY
class Display
{
	public:
		void draw();
		void placeplayer(int x,int y);
		void cullwalls();
		int house(int x,int y,int w,int h,char d);
};
#endif
