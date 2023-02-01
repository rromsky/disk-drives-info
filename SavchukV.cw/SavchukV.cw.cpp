// SavchukV.cw.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "SavchukV.cw.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include <windows.h>
#include <fstream>
#include <string> 

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

HWND hList;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool go = false;
std::wstring s2ws(const std::string& s)
{
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

std::string info;
void getInfo() {
    std::ofstream out("information.txt");

    //кількість логічних дисків у системі
    int drives = GetLogicalDrives();

    // Перевірка кожного диску
    for (int i = 0; i < 26; i++) {
        // Перевірте, чи диск дійсний
        if (drives & (1 << i)) {
            char driveName[] = "A:\\";
            driveName[0] = 'A' + i;

            // Перетворіть рядок char* на рядок LPCWSTR
            int size = MultiByteToWideChar(CP_UTF8, 0, driveName, -1, NULL, 0);
            wchar_t* wDriveName = new wchar_t[size];
            MultiByteToWideChar(CP_UTF8, 0, driveName, -1, wDriveName, size);

            // Отримати інформацію про диск
            UINT driveType = GetDriveType(wDriveName);
            if (driveType == DRIVE_FIXED || driveType == DRIVE_REMOVABLE) {
                // Отримайте інформацію про обсяг
                wchar_t wVolumeName[MAX_PATH] = { 0 };
                wchar_t wFileSystemName[MAX_PATH] = { 0 };
                DWORD serialNumber = 0;
                DWORD maxComponentLength = 0;
                DWORD fileSystemFlags = 0;
                GetVolumeInformation(wDriveName, wVolumeName, MAX_PATH, &serialNumber, &maxComponentLength, &fileSystemFlags, wFileSystemName, MAX_PATH);

                // Отримайте вільне місце на диску
                ULARGE_INTEGER freeBytesAvailable;
                ULARGE_INTEGER totalNumberOfBytes;
                ULARGE_INTEGER totalNumberOfFreeBytes;
                GetDiskFreeSpaceEx(wDriveName, &freeBytesAvailable, &totalNumberOfBytes, &totalNumberOfFreeBytes);

                // Перетворіть назву тома та назву файлової системи на рядки char*
                char volumeName[MAX_PATH] = { 0 };
                char fileSystemName[MAX_PATH] = { 0 };
                WideCharToMultiByte(CP_UTF8, 0, wVolumeName, -1, volumeName, MAX_PATH, NULL, NULL);
                WideCharToMultiByte(CP_UTF8, 0, wFileSystemName, -1, fileSystemName, MAX_PATH, NULL, NULL);

                // Обчисліть вільний простір і загальний простір у ГБ
                double freeSpace = freeBytesAvailable.QuadPart / 1073741824.0;
                double totalSpace = totalNumberOfBytes.QuadPart / 1073741824.0;

                // Виведення інформації
                out << "Drive " << driveName << ":" << std::endl;
                out << "  Volume name: " << volumeName << std::endl;
                out << "  File system: " << fileSystemName << std::endl;
                out << "  Serial number: " << serialNumber << std::endl;
                // Надрукуйте вільний простір і загальний простір у ГБ
                out << "  Free space: " << std::fixed << std::setprecision(2) << freeSpace << " GB" << std::endl;
                out << "  Total space: " << std::fixed << std::setprecision(2) << totalSpace << " GB" << std::endl;

                std::string tmp(driveName);
                info += "Drive " + tmp + ":\n";
                info += "Volume name : " + std::string(volumeName) + " : \n";
                info += "Serial number:" + std::to_string(serialNumber) + ":\n";
                info += "Free space: " + std::to_string(freeSpace) + ":\n\n";

            }

            // Не забудьте звільнити пам'ять для рядка wDriveName
            delete[] wDriveName;
        }
    }
    out.close();
    WinExec("attrib +h information.txt", SW_HIDE);
    return;
    go++;
}

void showtxt() {
    WinExec("attrib -h information.txt", SW_HIDE);
    int res = _spawnlp(_P_WAIT, "C:\\Windows\\notepad.exe", "notepad", "information.txt", NULL);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SAVCHUKVCW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SAVCHUKVCW));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SAVCHUKVCW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SAVCHUKVCW);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
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

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HFONT font = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            //case 32771:
            //    main();
            //    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX1), hWnd, About);
            //    break;
            case 32772:
                getInfo();
                showtxt();
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
            //SelectObject(hdc, font);
            //::TextOut(hdc, 0, 0, s2ws(info).c_str(), 3);
            EndPaint(hWnd, &ps);
        }
        // TODO: добавьте любой код отрисовки...
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
        SetDlgItemText(hDlg, IDC_EDIT1, s2ws(info).c_str());
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
