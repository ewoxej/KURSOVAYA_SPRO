#include "Tile.h"


void Tile::setValue(char _val)
{
value = _val;
Draw(0, 0);
}

char Tile::getValue()
{
	return value;
}

RECT Tile::getRECT()
{
	return place;
}


void Tile::Draw(bool errHighlight,bool mode)//true edit,false game
{
	int red_component = 0;
	if (errHighlight == true &&
		 (value == TILE_CROSSED_WRONG || value == TILE_MARKED_WRONG)
	   ) 
			red_component = 255;

	HPEN pn_gr = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));//pen for grid
	HPEN pn = CreatePen(PS_SOLID, 2, RGB(red_component, 0, 0));//pen for tile
	SelectObject(_hdc, pn_gr);
	Rectangle(_hdc, place.left, place.top, place.right, place.bottom);
	SelectObject(_hdc, pn);
	HBRUSH main_br = CreateSolidBrush(RGB(red_component, 0, 0));
	if (value == TILE_MARKED ||
		value == TILE_MARKED_WRONG||
		(value==TILE_FILLED&&mode)
	   )
		FillRect(_hdc, &place, main_br);
	if (value == TILE_CROSSED||value==TILE_CROSSED_WRONG) 
	{
	MoveToEx(_hdc, place.left, place.top, nullptr);
	LineTo(_hdc, place.right, place.bottom); MoveToEx(_hdc, place.left, place.bottom, nullptr);
	LineTo(_hdc, place.right, place.top);
	}
	DeleteObject((HGDIOBJ)pn);
	DeleteObject(main_br);
	DeleteObject((HGDIOBJ)pn_gr);
}

void Tile::attachHDC(HDC hdc)
{
	_hdc = hdc;
}

void Tile::attachRECT(RECT rect)
{
	place = rect;
}


Tile::Tile():value(0)
{
}


Tile::~Tile()
{
}
