#include <windows.h>
#include "Resource.h"
#include"TileMatrix.h"
#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
TileMatrix matr(9, 12);							// игровое поле
HWND editor_button, game_button, about_button;  // кнопки в главном меню
HMENU editor_menu, game_menu;					// панели меню
HBITMAP hBitmap;
CHAR win_state = 0;//0-mainmenu,1-editor,2-game;


ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

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

   RECT r;
   r.bottom = 500;
   r.top = 10;
   r.left = 10;
   r.right = 400;
  // GetClientRect(hWnd,&r);
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
