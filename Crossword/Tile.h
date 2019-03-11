#pragma once
#include<windows.h>
class Tile
{
private:
	char value;
	RECT place;
	HDC _hdc;
	HWND hwnd;
public:
	void setValue(char _val);
	char getValue();
	RECT getRECT();
	void Draw(bool errHighlight,bool mode);
	void attachHDC(HDC hdc);
	void attachRECT(RECT rect);
	void attachHWND(HWND _hwnd);
	Tile();
	~Tile();
};

