#include "TileMatrix.h"
#include"resource.h"
#define COUNT_MATRIX_RATIO 0.25
template<class T>
T ** TileMatrix::memAlloc(int _dim_Xs,int _dimYs)
{
	T** dest = new T*[_dim_Xs];
	for (int i = 0; i < _dim_Xs; i++)
		dest[i] = new T[_dimYs];
	return dest;
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
	dim_X = 0;
	dim_Y = 0;
	mem_allocated = false;
}


void TileMatrix::fillMatrix(int ** matr, int _r, int _c)
{
	for (int i = 0; i < _r; i++)
		for (int j = 0; j < _c; j++)
			matr[i][j] = 0;
}

void TileMatrix::rectCalculate(LPRECT leftRect, LPRECT topRect)
{

	int width = (rect.right - rect.left);
	int height = (rect.bottom - rect.top);
	topRect->bottom = rect.top + (height*COUNT_MATRIX_RATIO);
	topRect->left += (width*COUNT_MATRIX_RATIO);
	leftRect->right = rect.left + (width*COUNT_MATRIX_RATIO);
	leftRect->top += (height*COUNT_MATRIX_RATIO);
}

TileMatrix::TileMatrix():mem_allocated(false)
{

}

TileMatrix::TileMatrix(int x,int y):dim_X(x),dim_Y(y)
{
	if (x == 0 || y == 0) return;
	createTables();
}

void TileMatrix::InitializeMatrix() {
	leftRect = rect;
	topRect = rect;
	rectCalculate(&leftRect, &topRect);
		fillMatrix(matr_x, dim_X, dim_YDM);
		fillMatrix(matr_y, dim_XDM, dim_Y);
		countInY();
		countInX();
}
void TileMatrix::Draw(bool highlightErrors)
{
	if (state == STATE_MAINMENU) return;
	/////////////////////////////////
	int fill_inv = 0;
	int wrongs = 0;
	hdc = GetDC(hwnd);
	for (int i = 0; i < dim_X; i++) {
		for (int j = 0; j < dim_Y; j++) {
			matr[i][j].attachHDC(hdc);
			matr[i][j].Draw(highlightErrors, state);
			if (matr[i][j].getValue() == TILE_FILLED) fill_inv++;
			if (matr[i][j].getValue() == TILE_CROSSED_WRONG || matr[i][j].getValue() == TILE_MARKED_WRONG) wrongs++;
		}
	}
	if (STATE_GAME) 
	{
		DrawMatrix(matr_x, dim_X, dim_YDM, topRect);
		DrawMatrix(matr_y, dim_XDM, dim_Y, leftRect);
	}
	ReleaseDC(hwnd,hdc);
	if (fill_inv == 0 && dim_X > 0 && state == STATE_GAME && wrongs == 0) SendMessage(hwnd, WM_COMMAND,IDB_WIN,0);
}


void TileMatrix::attachHWND(HWND _hwnd)
{
	hwnd = _hwnd;
}

void TileMatrix::attachRECT(RECT _rect)
{
	rect = _rect;
	if (dim_X == 0 || dim_Y == 0) return;
	_rect.left += (rect.right - rect.left)*COUNT_MATRIX_RATIO;
	_rect.top += (rect.bottom - rect.top)*COUNT_MATRIX_RATIO;
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
	InitializeMatrix();
}

char TileMatrix::getState()
{
	return state;
}

void TileMatrix::setState(char _st)
{
	state = _st;
}

void TileMatrix::setValueByPress(LPARAM lParam,char new_value)
{
	if (new_value == TILE_CROSSED && state == STATE_EDITOR) return;
	int xPos, yPos;
	char old_value;
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
				old_value=matr[i][j].getValue();
				if (state == STATE_EDITOR)
				{
					if (new_value == old_value) new_value = TILE_EMPTY;
				}
				if (state == STATE_GAME) 
				{
					if (old_value == TILE_EMPTY) 
					{
						if (new_value == TILE_MARKED) new_value = TILE_MARKED_WRONG;
						//if (new_value == TILE_CROSSED) ok
					}
					if (old_value == TILE_FILLED) 
					{
						//if (new_value == TILE_MARKED) ok
						if (new_value == TILE_CROSSED) new_value = TILE_CROSSED_WRONG;
					}
					if (old_value == TILE_MARKED) 
					{
						if (new_value == TILE_MARKED) new_value = TILE_FILLED;
						if (new_value == TILE_CROSSED) new_value = TILE_CROSSED_WRONG;
					}
					if (old_value == TILE_CROSSED)
					{
						if (new_value == TILE_MARKED) new_value = TILE_MARKED_WRONG;
						if (new_value == TILE_CROSSED) new_value = TILE_EMPTY;
					}
					if (old_value == TILE_CROSSED_WRONG)
					{
						//if (new_value == TILE_MARKED) ok
						if (new_value == TILE_CROSSED) new_value = TILE_FILLED;
					}
					if (old_value == TILE_MARKED_WRONG)
					{
						if (new_value == TILE_MARKED) new_value = TILE_EMPTY;
						//if (new_value == TILE_CROSSED) ok
					}
				}
				matr[i][j].setValue(new_value);
			}
		}
	}
}

void TileMatrix::save(LPWSTR filename)
{
	std::ofstream file(filename);
	file << dim_X << " " << dim_Y<<"\n";
	for (int i = 0; i < dim_X; i++,file<<"\n")
		for (int j = 0; j < dim_Y; j++)
			file << (int)matr[i][j].getValue()<<" ";

}

void TileMatrix::restore(LPWSTR filename)
{

	InvalidateRect(hwnd, NULL, TRUE);
	std::ifstream file(filename);
	destroyTables();
	file >> dim_X >> dim_Y;
	createTables();
	int tempval;
	for (int i = 0; i < dim_X; i++)
		for (int j = 0; j < dim_Y; j++) {
			file >> tempval;
			matr[i][j].setValue(tempval);
		}
}

void TileMatrix::create(int dx, int dy) {
	InvalidateRect(hwnd, NULL, TRUE);
	destroyTables();
	dim_X = dx, dim_Y = dy;
	createTables();
}

int TileMatrix::returnx()
{
	return dim_X;
}

int TileMatrix::returny()
{
	return dim_Y;
}



void TileMatrix::DrawMatrix(int** matrix, int _x, int _y, RECT _rect)
{
	if (dim_X == 0 || dim_Y == 0) return;
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
			if (matr[i][j].getValue() == TILE_FILLED||
				matr[i][j].getValue() == TILE_MARKED||
				matr[i][j].getValue() == TILE_CROSSED_WRONG) { matr_x[i][inMatrixPos]++; }
			else { if (matr_x[i][inMatrixPos] != 0) inMatrixPos--; }
		}
	}

}

void TileMatrix::countInX() {
	int inMatrixPos;
	for (int i = 0; i <dim_Y-1; i++)
	{
		inMatrixPos = dim_XDM-1;
		for (int j = dim_X-1 ;j>=0; j--)
		{
			if (matr[j][i].getValue() == TILE_FILLED||
				matr[j][i].getValue() == TILE_MARKED||
				matr[j][i].getValue() == TILE_CROSSED_WRONG) { matr_y[inMatrixPos][i]++; }
			else { if (matr_y[inMatrixPos][i] != 0) inMatrixPos--; }
		}
	}
}




TileMatrix::~TileMatrix()
{
	destroyTables();
}
