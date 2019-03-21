#pragma once
#include<windows.h>
enum :char
{
	TILE_EMPTY,
	TILE_MARKED,//invisible in game mode
	TILE_CROSSED,
	TILE_FILLED,//looks like TILE_MARKED in edit mode
	TILE_CROSSED_WRONG,//X
	TILE_MARKED_WRONG//[]
};
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
	void Draw(bool errHighlight,bool mode);
	void attachHDC(HDC hdc);
	void attachRECT(RECT rect);
	Tile();
	~Tile();
};

