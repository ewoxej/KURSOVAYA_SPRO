#pragma once
#include<windows.h>
#include<windowsx.h>
#include<string>
#include<fstream>
#include"Tile.h"
enum :char {
	STATE_MAINMENU,
	STATE_EDITOR,
	STATE_GAME
};
class TileMatrix
{
private:
	//variables:
	char state;
	bool mem_allocated;
	int dim_X, dim_Y;
	int dim_XDM, dim_YDM;
	RECT rect,topRect,leftRect;
	Tile** matr;
	int** matr_x;
	int** matr_y;
	HDC hdc;
	HWND hwnd;
	//functions:
	template<class T>
	T** memAlloc(int _rows,int _cols);
	template<class T>
	void memRelease(T** dest, int _size);
	void rectCalculate(LPRECT leftrect, LPRECT toprect);
public:
	TileMatrix();
	void createTables();
	void destroyTables();
	void fillMatrix(int**, int _r, int _c);
	TileMatrix(int x,int y);
	void Draw(bool highlightErrors=false);
	void InitializeMatrix();
	void attachHWND(HWND _hwnd);
	void attachRECT(RECT _rect);
	char getState();
	void setState(char);
	void setValueByPress(LPARAM lParam, char val);
	void save(LPWSTR filename);
	void restore(LPWSTR filename);
	void create(int, int);
	int returnx();
	int returny();
	void countInY();
	void countInX();
	void DrawMatrix(int**,int,int,RECT _rect);
	~TileMatrix();
};

