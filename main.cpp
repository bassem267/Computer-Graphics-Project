#if defined(UNICODE) && !defined(_UNICODE)
#define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
#define UNICODE
#endif

#include <windowsx.h>
#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <cmath>
#include <math.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>

using namespace std;

void swap(int& x,int& y)
{
    int tmp=x;
    x=y;
    y=tmp;
}
int round(double x)
{
    return (int)(x+0.5);
}



/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("Computer Graphics Project");

int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the window class */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_EXCLAMATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_INFORMATION);
    wincl.hCursor = LoadCursor (NULL, IDC_HAND);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default color as the background of the window */
    wincl.hbrBackground = (HBRUSH) RGB(0, 0, 0);

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
               0,                   /* Extended possibilities for variation */
               szClassName,         /* Class name */
               _T("Computer Graphics Project"),       /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,       /* Windows decides the position */
               CW_USEDEFAULT,       /* where the window ends up on the screen */
               1900,                /* The programs width */
               900,                 /* and height in pixels */
               HWND_DESKTOP,        /* The window is a child-window to desktop */
               NULL,                /* No menu */
               hThisInstance,       /* Program Instance handler */
               NULL                 /* No Window Creation data */
           );
    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }
    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


int x, y;
static int instruction = 0;
int count = 0;
/*  This function is called by the Windows function DispatchMessage()  */
PAINTSTRUCT ps;
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    hdc = GetDC(hwnd);

    switch (message)                  /* handle the messages */
    {
    // Handling The Menu Instructions
    case WM_COMMAND:
        switch(wParam)
        {
        case 1:
            instruction = 1;
            MessageBeep(MB_OK);
            cout<<"Line DDA Drawing Algorithm"<<endl;
            break;
        case 2:
            instruction = 2;
            MessageBeep(MB_OK);
            cout<<"Line PARAMETRIC Drawing Algorithm"<<endl;
            break;
        case 3:
            instruction = 3;
            MessageBeep(MB_OK);
            cout<<"Line MID-POINT Drawing Algorithm"<<endl;
            break;
        case 4:
            instruction = 4;
            MessageBeep(MB_OK);
            cout<<"Circle Direct Drawing Algorithm"<<endl;
            break;
        case 5:
            instruction = 5;
            MessageBeep(MB_OK);
            cout<<"Circle Polar Drawing Algorithm"<<endl;
            break;
        case 6:
            instruction = 6;
            MessageBeep(MB_OK);
            cout<<"Circle Iterative Polar Drawing Algorithm"<<endl;
            break;
        case 7:
            instruction = 7;
            MessageBeep(MB_OK);
            cout<<"Circle Mid-Point Drawing Algorithm"<<endl;
            break;
        case 8:
            instruction = 8;
            MessageBeep(MB_OK);
            cout<<"Circle Modified Mid-Point Drawing Algorithm"<<endl;
            break;
        }

    // the Menu Creation
    case WM_CREATE:
    {
        HMENU LINE = CreateMenu();
        AppendMenu(LINE, MF_STRING, 1, "Line DDA");
        AppendMenu(LINE, MF_STRING, 2, "Line PARAMETRIC");
        AppendMenu(LINE, MF_STRING, 3, "Line MID-POINT");

        HMENU CIRCLE = CreateMenu();
        AppendMenu(CIRCLE, MF_STRING, 4, "Circle Direct");
        AppendMenu(CIRCLE, MF_STRING, 5, "Circle Polar");
        AppendMenu(CIRCLE, MF_STRING, 6, "Circle Iterative Polar");
        AppendMenu(CIRCLE, MF_STRING, 7, "Circle Mid-Point");
        AppendMenu(CIRCLE, MF_STRING, 8, "Circle Modified Mid-Point");

        HMENU FILLINGSHAPES = CreateMenu();
        AppendMenu(FILLINGSHAPES, MF_STRING, 9, "Circle with lines");
        AppendMenu(FILLINGSHAPES, MF_STRING, 10, "Circle with circles");
        AppendMenu(FILLINGSHAPES, MF_STRING, 11, "Square with Hermit Curve");
        AppendMenu(FILLINGSHAPES, MF_STRING, 12, "Rectangle with Bezier Curve");

        HMENU FILLINGALG = CreateMenu();
        AppendMenu(FILLINGALG, MF_STRING, 13, "Convex");
        AppendMenu(FILLINGALG, MF_STRING, 14, "NoN Convex");
        AppendMenu(FILLINGALG, MF_STRING, 15, "Flood Fill (Recursive)");
        AppendMenu(FILLINGALG, MF_STRING, 16, "Flood Fill (NoN-Recursive)");

        HMENU ELLIPSE = CreateMenu();
        AppendMenu(ELLIPSE, MF_STRING, 18, "Ellipse Direct");
        AppendMenu(ELLIPSE, MF_STRING, 19, "Ellipse Polar");
        AppendMenu(ELLIPSE, MF_STRING, 20, "Ellipse MID-POINT");

        HMENU COLOR = CreateMenu();
        AppendMenu(COLOR, MF_STRING, 26, "Red");
        AppendMenu(COLOR, MF_STRING, 27, "Blue");
        AppendMenu(COLOR, MF_STRING, 28, "Green");
        AppendMenu(COLOR, MF_STRING, 29, "Black");
        AppendMenu(COLOR, MF_STRING, 30, "Yellow");


        HMENU hmenu = CreateMenu();
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)LINE, "Line");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)CIRCLE, "Circle");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)FILLINGSHAPES, "Shape Filling");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)FILLINGSHAPES, "Filling Algorithms");
        AppendMenu(hmenu, MF_POPUP, 19, "Cardinal Spline Curve");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)ELLIPSE, "Ellipse");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)COLOR, "SetColor");

        AppendMenu(hmenu, MF_STRING, 21, "Save");
        AppendMenu(hmenu, MF_STRING, 22, "Load");
        AppendMenu(hmenu, MF_STRING, 23, "Clean screen");
        AppendMenu(hmenu, MF_STRING, 25, "Help");
        AppendMenu(hmenu, MF_STRING, 24, "Exit");
        SetMenu(hwnd,hmenu);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);       /* send a WM QUIT to the message queue */
        break;
    default:                 /* for messages that we don't deal with */
        return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
