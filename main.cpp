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

//for load, save functions
static int SaveArrPoints[900000];
static int SaveArrNoOfPoints[900000];
static COLORREF SaveArrColorPoints[900000];
static int SP=0,SN=0,SC=0;


// DDA

int Round(double x)
{
    return (int)(x+0.5);
}
void DrawLineDDA(HDC hdc,int xs,int ys,int xe,int ye,COLORREF color)
{
    int Count=0;
    int dx=xe-xs;
    int dy=ye-ys;
    SetPixel(hdc,xs,ys,color);
    SaveArrPoints[SP++]=xs;
    SaveArrPoints[SP++]=ys;
    Count++;
    if(abs(dx)>=abs(dy))
    {
        int x=xs,xinc= dx>0?1:-1;
        double y=ys,yinc=(double)dy/dx*xinc;
        while(x!=xe)
        {
            x+=xinc;
            y+=yinc;
            SetPixel(hdc,x,Round(y),color);
            Count++;
            SaveArrPoints[SP++]=x;
            SaveArrPoints[SP++]=Round(y);
        }
    }
    else
    {
        int y=ys,yinc= dy>0?1:-1;
        double x=xs,xinc=(double)dx/dy*yinc;
        while(y!=ye)
        {
            x+=xinc;
            y+=yinc;
            SetPixel(hdc,Round(x),y,color);
            Count++;
            SaveArrPoints[SP++]=Round(x);
            SaveArrPoints[SP++]=y;
        }
    }
    SaveArrColorPoints[SC++]=color;
    SaveArrNoOfPoints[SN++]=Count;
}

//Mid-Point

void PlotLineLow(HDC hdc,int x0,int y0,int x1, int y1,COLORREF c)
{
    int Count=0;
    int dx = x1 - x0;
    int dy = y1 - y0;
    int yi = 1;
    if (dy < 0)
    {
        yi = -1;
        dy = -dy;
    }
    int D = (2 * dy) - dx;
    int  y = y0;

    for ( int x=x0; x<x1; x++)
    {
        SetPixel(hdc,x,y,c);
        SaveArrPoints[SP++]=x;
        SaveArrPoints[SP++]=y;
        Count++;
        if (D > 0)
        {
            y = y + yi;
            D = D + (2 * (dy - dx)) ;
        }
        else
            D = D + 2*dy;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
void PlotLineHigh(HDC hdc,int x0, int y0,int x1, int y1,COLORREF c)
{
    int Count=0;
    int dx = x1 - x0;
    int dy = y1 - y0;
    int xi = 1;
    if (dx < 0)
    {
        xi = -1;
        dx = -dx;
    }
    int D = (2 * dx) - dy;
    int x = x0;

    for (int y=y0; y<y1; y++)
    {
        SetPixel(hdc,x,y,c);
        SaveArrPoints[SP++]=x;
        SaveArrPoints[SP++]=y;
        Count++;
        if(D > 0)
        {
            x = x + xi;
            D = D + (2 * (dx - dy));
        }
        else
            D = D + 2*dx;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
void DrawLineMidPoint(HDC hdc,int x0,int y0,int x1,int y1,COLORREF c)
{
    if (abs(y1 - y0) < abs(x1 - x0))
    {
        if (x0 > x1)
            PlotLineLow(hdc,x1, y1, x0, y0,c);
        else
            PlotLineLow(hdc,x0, y0, x1, y1,c);
    }
    else
    {
        if (y0 > y1)
            PlotLineHigh(hdc,x1, y1, x0, y0,c);
        else
            PlotLineHigh(hdc,x0, y0, x1, y1,c);
    }
}

//Parametric

void DrawLineParametric(HDC hdc,int x1,int y1,int x2, int y2,COLORREF c)
{
    int Count=0;
    double dx=x2-x1;
    double dy=y2-y1;
    SetPixel(hdc,x1,y1,c);
    SaveArrPoints[SP++]=x1;
    SaveArrPoints[SP++]=y1;
    Count++;
    for(double t=0; t<1; t+=0.00001)
    {
        int x=x1+t*dx;
        int y=y1+t*dy;
        SetPixel(hdc,Round(x),Round(y),c);
        SaveArrPoints[SP++]=Round(x);
        SaveArrPoints[SP++]=Round(y);
        Count++;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
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


static int xstart, ystart, xend, yend;
static int instruction = 0;
COLORREF c = RGB(0, 0, 0);
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
    {
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
        AppendMenu(CIRCLE, MF_SEPARATOR,NULL,NULL);
        AppendMenu(CIRCLE, MF_STRING, 5, "Circle Polar");
        AppendMenu(CIRCLE, MF_STRING, 6, "Circle Iterative Polar");
        AppendMenu(CIRCLE, MF_SEPARATOR,NULL,NULL);
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

        HMENU ColorMenu=CreateMenu();
        AppendMenu(ColorMenu,MF_STRING,26,"Red");
        AppendMenu(ColorMenu,MF_STRING,27,"Green");
        AppendMenu(ColorMenu,MF_STRING,28,"Blue");
        AppendMenu(ColorMenu,MF_SEPARATOR,NULL,NULL);
        AppendMenu(ColorMenu,MF_STRING,29,"White");
        AppendMenu(ColorMenu,MF_STRING,30,"Black");
        AppendMenu(ColorMenu,MF_STRING,31,"Grey");
        AppendMenu(ColorMenu,MF_SEPARATOR,NULL,NULL);
        AppendMenu(ColorMenu,MF_STRING,32,"Light Blue");
        AppendMenu(ColorMenu,MF_STRING,33,"Brown");
        AppendMenu(ColorMenu,MF_STRING,34,"Pink");
        AppendMenu(ColorMenu,MF_STRING,35,"Violet");


        HMENU hmenu = CreateMenu();
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)LINE, "Line");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)CIRCLE, "Circle");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)ELLIPSE, "Ellipse");
        AppendMenu(hmenu, MF_SEPARATOR, NULL,NULL);

        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)FILLINGSHAPES, "Shape Filling");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)FILLINGALG, "Filling Algorithms");
        AppendMenu(hmenu, MF_POPUP, 19, "Cardinal Spline Curve");
        AppendMenu(hmenu, MF_SEPARATOR, NULL,NULL);

        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)ColorMenu, "Colors");
        AppendMenu(hmenu, MF_STRING, 21, "Save");
        AppendMenu(hmenu, MF_STRING, 22, "Load");
        AppendMenu(hmenu, MF_STRING, 23, "Clean screen");
        AppendMenu(hmenu, MF_SEPARATOR, NULL,NULL);

        AppendMenu(hmenu, MF_STRING, 25, "Help");
        AppendMenu(hmenu, MF_STRING, 24, "Exit");
        SetMenu(hwnd,hmenu);
        break;
    }

    case WM_LBUTTONDOWN:
    {
        xstart = LOWORD(lParam);
        ystart = HIWORD(lParam);
        break;
    }

    case WM_LBUTTONUP:
    {
        xend = LOWORD(lParam);
        yend = HIWORD(lParam);
        switch(instruction)
        {
        case 1:
            DrawLineDDA(hdc, xstart, ystart, xend, yend, c);
            cout<<"Line Drawn"<<endl;
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
