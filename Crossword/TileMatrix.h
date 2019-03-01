#pragma once
#include<windowsx.h>
#include<string>
#include<fstream>
#include<vector>
#include"Tile.h"
class TileMatrix
{
private:
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
	void Draw();
	void attachHDC(HDC _hdc);
	void attachRECT(RECT _rect);
	void setValueByPress(LPARAM lParam, int val);
	void save(std::string filename);
	void restore(std::string filename);
	void countInY();
	void countInX();
	void DrawMatrix(int**,int,int,RECT _rect);
	~TileMatrix();
};

