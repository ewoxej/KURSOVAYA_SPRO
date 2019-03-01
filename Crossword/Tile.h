#pragma once
#include<windows.h>
class Tile
{
private:
	char value;
	RECT place;
	HDC _hdc;
public:
	void setValue(char _val);
	char getValue();
	RECT getRECT();
	void Draw();
	void attachHDC(HDC hdc);
	void attachRECT(RECT rect);
	Tile();
	~Tile();
};

