#include "stdafx.h"
#include "Tile.h"


void Tile::setValue(char _val)
{
	if (value == _val) value = 0;
	else value = _val;
	Draw();
}

char Tile::getValue()
{
	return value;
}

RECT Tile::getRECT()
{
	return place;
}


void Tile::Draw()
{
	Rectangle(_hdc, place.left, place.top, place.right, place.bottom);
	if(value==1)FillRect(_hdc, &place, (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)));
	if (value == 2) { MoveToEx(_hdc, place.left, place.top, nullptr); 
	LineTo(_hdc, place.right, place.bottom); MoveToEx(_hdc, place.left, place.bottom, nullptr);
	LineTo(_hdc, place.right, place.top);
	}
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
