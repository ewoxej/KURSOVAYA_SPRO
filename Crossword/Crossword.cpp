#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include<commdlg.h>
#include "Resource.h"
#include"TileMatrix.h"
#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
INT matr_dimX=0, matr_dimY=0;
TileMatrix matr(matr_dimX, matr_dimY);							// игровое поле
HWND editor_button, game_button, about_button;  // кнопки в главном меню
HMENU editor_menu, game_menu;					// панели меню
HBITMAP hBitmap;
CHAR win_state = 0;//0-mainmenu,1-editor,2-game;
TCHAR opened_path[255];
RECT r;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SizeDlg(HWND, UINT, WPARAM, LPARAM);
void rect_calc(LPRECT rc) {
	rc->bottom = 10 + (30 * matr_dimY);
	rc->top = 10;
	rc->left = 10;
	rc->right = 10 + (30 * matr_dimX);
}
BOOL FileOpenDlg(HWND hWnd, LPWSTR pstrFileName,LPWSTR filters=NULL);
BOOL FileSaveDlg(HWND hWnd, LPWSTR pstrFileName, LPWSTR filters = NULL);
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Загрузка ресурсов
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CROSSWORD, szWindowClass, MAX_LOADSTRING);
	hBitmap = (HBITMAP)LoadImage(NULL, L"pic.bmp",IMAGE_BITMAP,0, 0,LR_LOADFROMFILE);
	editor_menu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU1));
	game_menu = LoadMenu(hInst, MAKEINTRESOURCE(IDR_MENU2));
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {

            TranslateMessage(&msg);
            DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CROSSWORD));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName   = 0;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   //
   //
   editor_button = CreateWindow(L"button", L"Редактор", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
   10, 10, 120, 30, hWnd, (HMENU)IDB_EDITOR, hInstance, NULL);
   game_button = CreateWindow(L"button", L"Играть", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
   10, 50, 120, 30, hWnd, (HMENU)IDB_GAME, hInstance, NULL);
   about_button = CreateWindow(L"button", L"О программе", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	   10, 90, 120, 30, hWnd, (HMENU)IDB_ABOUT, hInstance, NULL);


   rect_calc(&r);
   //GetClientRect(hWnd,&r);
   matr.attachRECT(r);
   //
   //
   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HANDLE hOldBitmap;
	HDC  hCompatibleDC;
	BITMAP Bitmap;
	RECT Rect;
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
			switch (wmId)
			{
			case IDB_EDITOR:
			{
				win_state = 1;
				matr.setState(1);
				SetMenu(hWnd, editor_menu);
				DestroyWindow(editor_button);
				DestroyWindow(game_button);
				DestroyWindow(about_button);
				break; }
			case IDB_GAME:
			{
				win_state = 2;
				matr.setState(2);
				SetMenu(hWnd, game_menu);
				DestroyWindow(editor_button);
				DestroyWindow(game_button);
				DestroyWindow(about_button);
				break; }
			case ID_OPENMENU:
			{
				FileOpenDlg(hWnd, (LPWSTR)&opened_path, (LPWSTR)L"Японский кроссворд (*.jcw)\0*.jcw\0\0");
				MessageBox(hWnd, opened_path, L"msg", MB_OK);
				matr.restore(opened_path);
				matr_dimX = matr.returnx();
				matr_dimY = matr.returny();
				rect_calc(&r);
				matr.attachRECT(r);
				break;
			}
			case ID_SAVEMENU:
			{
				if(opened_path!=nullptr)
				matr.save(opened_path);
				break;
			}
			case ID_SAVEASMENU:
			{
				FileSaveDlg(hWnd, (LPWSTR)&opened_path,(LPWSTR)L"Японский кроссворд (*.jcw)\0*.jcw\0\0");
				MessageBox(hWnd, (LPWSTR)(std::wstring(opened_path) + L".jcw").c_str(), L"msg", MB_OK);
				matr.save((LPWSTR)(std::wstring(opened_path) + L".jcw").c_str());
				break;
			}
			case ID_CREATEMENU:
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd,SizeDlg);
				rect_calc(&r);
				matr.attachRECT(r);
				matr.create(matr_dimX, matr_dimY);
				break;
			}
			case IDB_MAINMENU:{
				matr.setState(0);
				win_state = 0;
				SetMenu(hWnd, NULL);
				   editor_button = CreateWindow(L"button", L"Редактор", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
   10, 10, 120, 30, hWnd, (HMENU)IDB_EDITOR, hInst, NULL);
   game_button = CreateWindow(L"button", L"Играть", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
   10, 50, 120, 30, hWnd, (HMENU)IDB_GAME, hInst, NULL);
   about_button = CreateWindow(L"button", L"О программе", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
	   10, 90, 120, 30, hWnd, (HMENU)IDB_ABOUT, hInst, NULL);
				break;
			}
			case IDB_ABOUT: {
				DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
				break;
			}
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_LBUTTONDOWN:
	{
		if(win_state==1) matr.setValueByPress(lParam, 3);///////////
		else matr.setValueByPress(lParam, 1);
		InvalidateRect(hWnd, NULL, TRUE);
		break; }
	case WM_RBUTTONDOWN:
	{
		matr.setValueByPress(lParam, 2);
		InvalidateRect(hWnd, NULL, TRUE);
		break; }
	case WM_MBUTTONDOWN: {
		matr.highlightErrors();
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			//hCompatibleDC = CreateCompatibleDC(hdc);

			//hOldBitmap = SelectObject(hCompatibleDC, hBitmap);

			//GetClientRect(hWnd, &Rect);
			//GetObject(hBitmap, sizeof(Bitmap), &Bitmap);
			//StretchBlt(hdc, 0, 0, Rect.right, Rect.bottom,
			//	hCompatibleDC, 0, 0, Bitmap.bmWidth,
			//	Bitmap.bmHeight, SRCCOPY);

			//SelectObject(hCompatibleDC, hOldBitmap);

			//DeleteObject(hBitmap);

			//DeleteDC(hCompatibleDC);
					matr.attachHDC(hdc);
					if(win_state!=0)
					matr.Draw();
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK SizeDlg(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	TCHAR dimxstr[2], dimystr[2];
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			Edit_GetLine(GetDlgItem(hDlg, IDC_EDIT1), 2, dimxstr, 2);
			Edit_GetLine(GetDlgItem(hDlg, IDC_EDIT2), 2, dimystr, 2);
			matr_dimX=std::stoi(std::wstring(dimxstr));
			matr_dimY=std::stoi(std::wstring(dimystr));
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL FileOpenDlg(HWND hWnd, LPWSTR pstrFileName, LPWSTR filters) 
{
	OPENFILENAME ofn;
	//TCHAR szFile[MAX_PATH];
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 255;
	ofn.nFilterIndex = 1;
	ofn.lpstrFilter = filters;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 255;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if (GetOpenFileName(&ofn) == TRUE)
	{
		return 1;
	}
	return 0;
}

BOOL FileSaveDlg(HWND hWnd, LPWSTR pstrFileName,LPWSTR filters)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = pstrFileName;
	ofn.nMaxFile = 255;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = 255;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.lpstrFilter = filters;
	ofn.nMaxFileTitle = 255;
	//ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetSaveFileName(&ofn) == TRUE)
	{
		return 1;
	}

	return 0;
}
