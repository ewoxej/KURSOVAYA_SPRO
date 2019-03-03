#include "Tile.h"


void Tile::setValue(char _val)
{
value = _val;
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


void Tile::Draw(bool errHighlight)
{
	int rc = 0;
	if (errHighlight == true && (value == 4 || value == 5)) rc = 255;
	HPEN pn_gr = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
	SelectObject(_hdc, pn_gr);
	Rectangle(_hdc, place.left, place.top, place.right, place.bottom);
	HPEN pn = CreatePen(PS_SOLID, 2, RGB(rc, 0, 0));
	SelectObject(_hdc, pn);
	if (value == 1 || value == 5)FillRect(_hdc, &place, (HBRUSH)CreateSolidBrush(RGB(rc, 0, 0)));
	if (value == 2||value==4) {
		MoveToEx(_hdc, place.left, place.top, nullptr);
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
