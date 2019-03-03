#include "TileMatrix.h"
template<class T>
T ** TileMatrix::memAlloc(int _dim_Xs,int _cols)
{
	T** dest = new T*[_dim_Xs];
	for (int i = 0; i < _dim_Xs; i++)
		dest[i] = new T[_cols];
	return dest;
}

void TileMatrix::createTables()
{
	dim_XDM = 0, dim_YDM = 0;
	if (dim_X % 2 != 0) dim_XDM++; dim_XDM += (dim_X / 2);
	if (dim_Y % 2 != 0) dim_YDM++; dim_YDM += (dim_Y / 2);
	matr = memAlloc<Tile>(dim_X,dim_Y);
	matr_x = memAlloc<int>(dim_X, dim_YDM);
	fillMatrix(matr_x, dim_X, dim_YDM);
	matr_y = memAlloc<int>(dim_XDM, dim_Y);
	fillMatrix(matr_y, dim_XDM, dim_Y);
	mem_allocated = true;
}

void TileMatrix::destroyTables()
{
	if (!mem_allocated) return;
	memRelease(matr, dim_X);
	memRelease(matr_x, dim_X);
	memRelease(matr_y, dim_XDM);
	mem_allocated = false;
}
template <class T>
void TileMatrix::memRelease(T ** dest, int _size)
{
	{
		for (int i = 0; i < _size; i++)
			delete[] dest[i];
		delete[] dest;
	}
}

void TileMatrix::fillMatrix(int ** matr, int _r, int _c)
{
	for (int i = 0; i < _r; i++)
		for (int j = 0; j < _c; j++)
			matr[i][j] = 0;
}

void TileMatrix::highlightErrors()
{
	hlErr = !hlErr;
}

int TileMatrix::deleteZeros(int ** matr, int dim_Xs, int dim_Ys,bool indim_Xs)
{
	int count = 0;
	int result=0;
	if (indim_Xs) 
	{
		for (int i = 0; i < dim_Xs; i++) {
			for (int j = 0; j < dim_Ys; j++) {
				if (matr[j][i] != 0) count++;
		}
			if (count > result) result = count;
			count = 0;
		}
	}
	return result;
}

TileMatrix::TileMatrix():mem_allocated(false)
{

}

TileMatrix::TileMatrix(int x,int y):dim_X(x),dim_Y(y)
{
	createTables();
}

void TileMatrix::setValue(int x, int y, int val)
{
	matr[x][y].setValue(val);
}

int TileMatrix::getValue(int x, int y)
{
	return matr[x][y].getValue();
}

void TileMatrix::Draw()
{
	RECT leftRect = rect;
	RECT topRect = rect;
	int width = (rect.right - rect.left);
	int height = (rect.bottom - rect.top);
	topRect.bottom = rect.top + (height*0.25);
	topRect.left += (width*0.25);
	leftRect.right = rect.left + (width*0.25);
	leftRect.top += (height*0.25);
	/////////////////////////////////
	fillMatrix(matr_x, dim_X, dim_YDM);
	fillMatrix(matr_y, dim_XDM, dim_Y);
	countInY();
	countInX();
	//int new_dim_Y=deleteZeros(matr_x, dim_X, dim_YDM,true);
	//int new_dim_X = deleteZeros(matr_y, dim_XDM, dim_Y);
	DrawMatrix(matr_x, dim_X, dim_YDM, topRect);
	DrawMatrix(matr_y, dim_XDM, dim_Y, leftRect);
	for (int i = 0; i < dim_X; i++)
		for (int j = 0; j < dim_Y; j++)
			matr[i][j].Draw(hlErr);
}

void TileMatrix::attachHDC(HDC _hdc)
{
	hdc = _hdc;
	for (int i = 0; i < dim_X; i++)
		for (int j = 0; j < dim_Y; j++)
			matr[i][j].attachHDC(_hdc);
}

void TileMatrix::attachRECT(RECT _rect)
{
	rect = _rect;
	_rect.left += (rect.right - rect.left)*0.25;
	_rect.top += (rect.bottom - rect.top)*0.25;
	RECT temprect;
	int dim_X_step = (_rect.right - _rect.left) / dim_X;
	int col_step = (_rect.bottom - _rect.top) / dim_Y;
	for(int i=_rect.left,ik=0;ik<dim_X;i+=dim_X_step,ik++)
		for (int j = _rect.top, jk = 0; jk < dim_Y; j += col_step, jk++) {
			temprect.bottom = j+col_step;
			temprect.top = j;
			temprect.left = i;
			temprect.right = i + dim_X_step;
			matr[ik][jk].attachRECT(temprect);
		}


}

char TileMatrix::getState()
{
	return state;
}

void TileMatrix::setState(char _st)
{
	state = _st;
}

void TileMatrix::setValueByPress(LPARAM lParam,int val)
{
	if (val == 2 && state == 1) return;
	int xPos, yPos;
	char tmpval;
	RECT tempR;
	xPos = GET_X_LPARAM(lParam);
	yPos = GET_Y_LPARAM(lParam);
	
	if (xPos > rect.right || xPos<rect.left || yPos>rect.bottom || yPos < rect.top) return;
	for (int i = 0; i < dim_X; i++) {
		for (int j = 0; j < dim_Y; j++) {

			tempR = matr[i][j].getRECT();
			if ((tempR.top<yPos&&tempR.bottom>yPos) &&
				(tempR.left<xPos&&tempR.right>xPos))
			{
				tmpval=matr[i][j].getValue();
				/*
				edit mode          game mode
				0 empty				0 empty
				1 filled			1 [] visible correct
									2 X visible correct
									3  filled invisible
									4 X visible wrong
									5 [] visible wrong

				*/
				if (state == 1)
				{
					if (val == tmpval) val = 0;
				}
				if (state == 2) 
				{
					if (tmpval == 0) 
					{
						if (val == 1) val = 5;
						//if (val == 2) val = 2;
					}
					
					if (tmpval == 3) 
					{
						//if (val == 1) val = 1;
						if (val == 2) val = 4;
					}
					if (tmpval == 1) 
					{
						if (val == 1) val = 3;
						if (val == 2) val = 4;
					}
					if (tmpval == 2)
					{
						if (val == 1) val = 5;
						if (val == 2) val = 0;
					}
					if (tmpval == 4)
					{
						//if (val == 1) val = 1;
						if (val == 2) val = 3;
					}
					if (tmpval == 5)
					{
						if (val == 1) val = 0;
						//if (val == 2) val = 2;
					}
				}
				matr[i][j].setValue(val);
			}
		}
	}
}

void TileMatrix::save(std::string filename)
{
	std::ofstream file(filename);
	file << dim_X << " " << dim_Y<<"\n";
	for (int i = 0; i < dim_X; i++,file<<"\n")
		for (int j = 0; j < dim_Y; j++)
			file << (int)matr[i][j].getValue()<<" ";

}

void TileMatrix::restore(std::string filename)
{
	std::ifstream file(filename);
	file >> dim_X >> dim_Y;
	destroyTables();
	createTables();
	int tempval;
	attachHDC(hdc);
	attachRECT(rect);
	char junk;
	file >> junk;
	for (int i = 0; i < dim_X; i++)
		for (int j = 0; j < dim_Y; j++) {
			file >> tempval;
			if (tempval == 1) tempval = 3;
			matr[i][j].setValue(tempval);
		}
}



void TileMatrix::DrawMatrix(int** matrix, int _x, int _y, RECT _rect)
{
	RECT textrect;
	std::wstring tmpstr;
	int rect_height = (_rect.bottom - _rect.top), rect_width = (_rect.right - _rect.left);
	if (_y == 0) _y = 1; if (_x == 0) _x = 1;
	int step_y = rect_height / _y, step_x = rect_width / _x;
	for (int i = 0, ik = _rect.left; i < _x; i++, ik += step_x) {
		for (int j = 0, jk = _rect.top; j < _y; j++, jk += step_y) {
			textrect.left = ik;
			textrect.right = ik + step_x;
			textrect.top = jk;
			textrect.bottom = jk + step_y;
			tmpstr=std::to_wstring(matrix[i][j]);
			if (matrix[i][j] == 0) tmpstr = L"";
			DrawText(hdc, tmpstr.c_str(), tmpstr.length(), &textrect, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		}
	}
}

void TileMatrix::countInY() {
	int inMatrixPos;
	for (int i = 0; i < dim_X; i++)
	{
		inMatrixPos = dim_YDM;
		for (int j = dim_Y; j >= 0; j--)
		{
			if (matr[i][j].getValue() == 3|| matr[i][j].getValue() == 1|| matr[i][j].getValue() == 4) { matr_x[i][inMatrixPos]++; }
			else { if (matr_x[i][inMatrixPos] != 0) inMatrixPos--; }
		}
	}

}

void TileMatrix::countInX() {
	int inMatrixPos;
	int counter = 0;
	for (int i = dim_Y-1; i >=0; i--)
	{
		inMatrixPos = dim_XDM-1;
		for (int j = dim_X-1 ;j>=0; j--)
		{
			if (matr[j][i].getValue() == 3|| matr[j][i].getValue() == 1|| matr[j][i].getValue() == 4) { matr_y[inMatrixPos][i]++; }
			else { if (matr_y[inMatrixPos][i] != 0) inMatrixPos--; }
		}
	}
}




TileMatrix::~TileMatrix()
{
	destroyTables();
}
