#pragma once
#include<windows.h>
#include<windowsx.h>
#include<string>
#include<fstream>
#include<vector>
#include"Tile.h"
class TileMatrix
{
private:
	char state;
	bool hlErr;
	Tile** matr;
	int** matr_x;
	int** matr_y;
	HDC hdc;
	RECT rect;
	int dim_X, dim_Y;
	int dim_XDM, dim_YDM;
	int matr_x_mgr, matr_y_mgr;
	bool mem_allocated;
	template<class T>
	T** memAlloc(int _rows,int _cols);
	void createTables();
	void destroyTables();
	template<class T>
	void memRelease(T** dest, int _size);
	void fillMatrix(int**, int _r, int _c);
	int deleteZeros(int**,int,int,bool);
public:
	TileMatrix();
	TileMatrix(int x,int y);
	void setValue(int x, int y, int val);
	int getValue(int x, int y);
	void highlightErrors();
	void Draw();
	void attachHDC(HDC _hdc);
	void attachRECT(RECT _rect);
	char getState();
	void setState(char);
	void setValueByPress(LPARAM lParam, int val);
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

