#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include<commdlg.h>
#include "Resource.h"
#include"TileMatrix.h"
#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE	hInst;                               // текущий экземпляр
WCHAR		szTitle[MAX_LOADSTRING];             // Текст строки заголовка
WCHAR		szWindowClass[MAX_LOADSTRING];       // имя класса главного окна
INT			matr_dimX=0, 
			matr_dimY=0;						 // размеры игрового поля
TileMatrix	matr(matr_dimX, matr_dimY);			 // игровое поле
TileMatrix  title(0, 0);
HWND		editor_button,
			game_button,
			about_button;						 // кнопки в главном меню
HMENU		editor_menu,
			game_menu;							 // панели меню
TCHAR		opened_path[MAX_LOADSTRING];		 // путь к открытому файлу
RECT		field_rect;							 // прямоугльник игрового поля

//Прототипы функций:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    SizeDlg(HWND, UINT, WPARAM, LPARAM);
BOOL CommonFileDlg(HWND hWnd, LPWSTR pstrFileName, LPWSTR filters,BOOL open);
void MainMenu(HWND hWnd);
void calculateRect(HWND hWnd,LPRECT rc);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Загрузка ресурсов
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CROSSWORD, szWindowClass, MAX_LOADSTRING);
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
   MainMenu(hWnd);
   calculateRect(hWnd,&field_rect);
   matr.attachRECT(field_rect);
   matr.attachHWND(hWnd);
   return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
				matr.setState(STATE_EDITOR);
				SetMenu(hWnd, editor_menu);
				DestroyWindow(editor_button);
				DestroyWindow(game_button);
				DestroyWindow(about_button);
				break;
			}
			case IDB_GAME:
			{
				matr.setState(STATE_GAME);
				SetMenu(hWnd, game_menu);
				DestroyWindow(editor_button);
				DestroyWindow(game_button);
				DestroyWindow(about_button);
				break;
			}
			case ID_OPENMENU:
			{
				CommonFileDlg(hWnd, (LPWSTR)&opened_path, (LPWSTR)L"Японский кроссворд (*.jcw)\0*.jcw\0\0",TRUE);
				matr.restore(opened_path);
				matr_dimX = matr.returnx();
				matr_dimY = matr.returny();
				calculateRect(hWnd,&field_rect);
				matr.attachRECT(field_rect);
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
				CommonFileDlg(hWnd, (LPWSTR)&opened_path,(LPWSTR)L"Японский кроссворд (*.jcw)\0*.jcw\0\0",FALSE);
				matr.save((LPWSTR)(std::wstring(opened_path) + L".jcw").c_str());
				break;
			}
			case ID_GAMESAVEMENU:
			{
				CommonFileDlg(hWnd, (LPWSTR)&opened_path, (LPWSTR)L"Сохранение (*.game)\0*.game\0\0",FALSE);
				matr.save((LPWSTR)(std::wstring(opened_path) + L".game").c_str());
				break;
			}
			case ID_GAMELOADMENU:
			{
				CommonFileDlg(hWnd, (LPWSTR)&opened_path, (LPWSTR)L"Сохранение (*.game)\0*.game\0\0",TRUE);
				matr.restore(opened_path);
				matr_dimX = matr.returnx();
				matr_dimY = matr.returny();
				calculateRect(hWnd,&field_rect);
				matr.attachRECT(field_rect);
				break;
			}
			case ID_CREATEMENU:
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd,SizeDlg);
				calculateRect(hWnd,&field_rect);
				matr.attachRECT(field_rect);
				matr.create(matr_dimX, matr_dimY);
				break;
			}
			case IDB_MAINMENU:{
				MainMenu(hWnd);
				break;
			}
			case IDB_WIN:
			{
				SendMessage(hWnd, WM_COMMAND, IDB_MAINMENU, 0);
				MessageBox(hWnd, L"Победа", L"Поздравляем!Вы разгадали кроссворд", MB_OK);
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
		if (matr.getState()== STATE_EDITOR)
		{
			matr.setValueByPress(lParam, TILE_FILLED);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		if (matr.getState() == STATE_GAME)
		{
			matr.setValueByPress(lParam, TILE_MARKED);
			InvalidateRect(hWnd, NULL, FALSE);
		}
		break;
	}
	case WM_RBUTTONDOWN:
	{
		matr.setValueByPress(lParam, TILE_CROSSED);
		if(matr.getState()==STATE_GAME)
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}
	case WM_MBUTTONDOWN: 
	{
		matr.Draw(true);
		//InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
	case WM_MBUTTONUP: 
	{
		InvalidateRect(hWnd, NULL, TRUE);
		break;
	}
    case WM_PAINT:
        {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		if (matr.getState() == STATE_MAINMENU) {
		LOGFONT lf{ 70,0,0,0,FW_THIN,0,0,ANSI_CHARSET,OUT_STRING_PRECIS,
			CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,FIXED_PITCH,FF_MODERN,L"Monospace" };
		HFONT oldFont;
		HFONT newFont = CreateFontIndirect(&lf);

			RECT textrect;
			RECT headerrect;
			GetClientRect(hWnd, &textrect);
			GetClientRect(hWnd, &headerrect);
			headerrect.bottom = 100;
			textrect.left = 200;
			textrect.top += 100;
			TCHAR mtext[447];
			LoadStringW(hInst, IDS_MAINTEXT, mtext, 447);

				DrawText(hdc, mtext,
					447, &textrect, DT_LEFT);
				oldFont= SelectFont(hdc, newFont);
				DrawText(hdc, L"Японский кроссворд", 18, &headerrect, DT_CENTER);
				SelectFont(hdc, oldFont);
				DeleteFont(newFont);
			}
			matr.Draw();
            EndPaint(hWnd, &ps);
        }
        break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
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
			if (dimxstr[0] != '\x2')
			 matr_dimX=std::stoi(std::wstring(dimxstr));
			if (dimystr[0] != '\x2')
			 matr_dimY=std::stoi(std::wstring(dimystr));
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL CommonFileDlg(HWND hWnd, LPWSTR pstrFileName, LPWSTR filters,BOOL open) 
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_LOADSTRING;
	ofn.nFilterIndex = 1;
	ofn.lpstrFilter = filters;
	ofn.lpstrFileTitle = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	if ((open)?GetOpenFileName(&ofn):GetSaveFileName(&ofn))
	{
		return 1;
	}
	return 0;
}

void MainMenu(HWND hWnd) {
	matr.setState(STATE_MAINMENU);
	matr.destroyTables();
	matr.setState(STATE_MAINMENU);
	SetMenu(hWnd, NULL);
	editor_button = CreateWindow(L"button", L"Редактор", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		20, 100, 150, 50, hWnd, (HMENU)IDB_EDITOR, hInst, NULL);
	game_button = CreateWindow(L"button", L"Играть", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		20, 160, 150, 50, hWnd, (HMENU)IDB_GAME, hInst, NULL);
	about_button = CreateWindow(L"button", L"О программе", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
		20, 220, 150, 50, hWnd, (HMENU)IDB_ABOUT, hInst, NULL);
	opened_path[0] = '\0';
}

void calculateRect(HWND hWnd,LPRECT rc) {//передается clientrect и функция правильно выделяет место под кроссворд
	GetClientRect(hWnd, rc);
	float value = 0,width_tile=0,height_tile=0;
	int width = rc->right - rc->left;
	int height = rc->bottom - rc->top;
	if(matr_dimX!=0) 
width_tile = width / matr_dimX;
	if (matr_dimY != 0)
		height_tile = height / matr_dimY;
	if (width_tile >= height_tile)
		rc->right = height_tile * matr_dimX;
	else
		rc->bottom = width_tile * matr_dimY;
}
