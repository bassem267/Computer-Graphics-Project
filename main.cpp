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

//Circle Direct
void Draw8Points(HDC hdc,int xc,int yc,int a,int b,COLORREF color)
{
    SetPixel(hdc,xc+a,yc+b,color);
    SaveArrPoints[SP++]=xc+a;
    SaveArrPoints[SP++]=yc+b;

    SetPixel(hdc,xc+a,yc-b,color);
    SaveArrPoints[SP++]=xc+a;
    SaveArrPoints[SP++]=yc-b;

    SetPixel(hdc,xc-a,yc+b,color);
    SaveArrPoints[SP++]=xc-a;
    SaveArrPoints[SP++]=yc+b;

    SetPixel(hdc,xc-a,yc-b,color);
    SaveArrPoints[SP++]=xc-a;
    SaveArrPoints[SP++]=yc-b;

    SetPixel(hdc,xc+b,yc+a,color);
    SaveArrPoints[SP++]=xc+b;
    SaveArrPoints[SP++]=yc+a;

    SetPixel(hdc,xc+b,yc-a,color);
    SaveArrPoints[SP++]=xc+b;
    SaveArrPoints[SP++]=yc-a;

    SetPixel(hdc,xc-b,yc+a,color);
    SaveArrPoints[SP++]=xc-b;
    SaveArrPoints[SP++]=yc+a;

    SetPixel(hdc,xc-b,yc-a,color);
    SaveArrPoints[SP++]=xc-b;
    SaveArrPoints[SP++]=yc-a;
}

void DrawCircleDirect(HDC hdc,int xc,int yc,double R,COLORREF color)
{
    int Count=0;
    double y=R;//y=0
    for(int x=0; x<y; x++)//x=R; x>0; x-- you choose your own octant
    {
        Draw8Points(hdc,xc,yc,x,y,color);
        Count+=8;
        y=Round(sqrt(R*R-x*x));
    }
    SaveArrColorPoints[SC++]=color;
    SaveArrNoOfPoints[SN++]=Count;
}
//circle polar direct algorithm
void DrawCirclePolar(HDC hdc,int xc,int yc,double R,COLORREF color)
{
    int Count=0;
    double y=0;
    double theta=0;
    double dtheta=1.0/R;
    for(int x=R; x>y ; theta+=dtheta)
    {
        Draw8Points(hdc,xc,yc,Round(x),Round(y),color);
        Count+=8;
        x=R*cos(theta);
        y=R*sin(theta);
    }
    SaveArrColorPoints[SC++]=color;
    SaveArrNoOfPoints[SN++]=Count;
}
//circle polar iterative algorithm
void DrawCirclePolarIterative(HDC hdc,double xc,double yc,double radius,COLORREF c)
{
    int Count=0;
    double dtheta=1.0/radius;
    double cdtheta=cos(dtheta);
    double sdtheta=sin(dtheta);
    double x=0;
    double y=radius;
    Draw8Points(hdc,xc,yc,x,y,c);
    Count+=8;
    while(x<y)
    {
        double x1=x*cdtheta-y*sdtheta;
        y=x*sdtheta+y*cdtheta;
        x=x1;
        Draw8Points(hdc,xc,yc,Round(x),Round(y),c);
        Count+=8;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
//circle mid-point
void DrawCircleMidpoint(HDC hdc,int xc,int yc,int radius,COLORREF c)
{
    int x=0,Count=0;
    int y=radius;
    int d1=1-radius;
    Draw8Points(hdc,xc,yc,x,y,c);
    Count+=8;
    while(x<y)
    {
        if(d1<0)
        {
            d1+=(2*x+3);

        }
        else
        {
            d1+=(2*(x-y)+5);
            y--;
        }
        x++;
        Draw8Points(hdc,xc,yc,x,y,c);
        Count+=8;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
//circle modified midpoint
void DrawCircleModifiedMidpoint(HDC hdc,int xc,int yc,int radius,COLORREF c)
{
    int x=0,Count=0;
    int y=radius;
    int d1=1-radius;
    int c1=3;
    int c2=5-2*radius;
    Draw8Points(hdc,xc,yc,x,y,c);
    Count+=8;
    while(x<y)
    {
        if(d1<0)
        {
            d1+=c1;
            c2+=2;
        }
        else
        {
            d1+=c2;
            c2+=4;
            y--;
        }
        c1+=2;
        x++;
        Draw8Points(hdc,xc,yc,x,y,c);
        Count+=8;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}

// the quarter circle filling
void DrawLines(HDC hdc,int xc,int yc,int a,int b,int Quarter,COLORREF c)
{
    if(Quarter==1)
    {
        DrawLineDDA(hdc,xc,yc,xc+a,yc-b,c);
        DrawLineDDA(hdc,xc,yc,xc+b,yc-a,c);
    }
    else if (Quarter==2)
    {
        DrawLineDDA(hdc,xc,yc,xc-a,yc-b,c);
        DrawLineDDA(hdc,xc,yc,xc-b,yc-a,c);
    }
    else if(Quarter==3)
    {
        DrawLineDDA(hdc,xc,yc,xc-a,yc+b,c);
        DrawLineDDA(hdc,xc,yc,xc-b,yc+a,c);
    }
    else
    {
        DrawLineDDA(hdc,xc,yc,xc+a,yc+b,c);
        DrawLineDDA(hdc,xc,yc,xc+b,yc+a,c);
    }
}
void DrawCircleWithLines(HDC hdc,int xc,int yc,int radius,int Quarter,COLORREF c)
{
    DrawCircleModifiedMidpoint(hdc, xc, yc, radius, c);
    int x=0;
    int y=radius;
    DrawLines(hdc,xc,yc,x,y,Quarter,c);
    int d=1-radius;
    int d1=3;
    int d2=5-2*radius;
    while(x<y)
    {
        if(d<0)
        {
            d+=d1;
            d1+=2;
            d2+=2;
            x++;
        }
        else
        {
            d+=d2;
            d1+=2;
            d2+=4;
            y--;
            x++;
        }
        DrawLines(hdc,xc,yc,x,y,Quarter,c);
    }
}

// Circle Filled With Circles
void DrawPartCircle(HDC hdc,int xc,int yc,int a,int b,int Quarter, COLORREF color)
{
    if( Quarter == 1)
    {
        SetPixel(hdc,xc+a,yc-b,color);
        SaveArrPoints[SP++]=xc+a;
        SaveArrPoints[SP++]=yc-b;

        SetPixel(hdc,xc+b,yc-a,color);
        SaveArrPoints[SP++]=xc+b;
        SaveArrPoints[SP++]=yc-a;
    }

    else if( Quarter == 2)
    {
        SetPixel(hdc,xc-a,yc-b,color);
        SaveArrPoints[SP++]=xc-a;
        SaveArrPoints[SP++]=yc-b;

        SetPixel(hdc,xc-b,yc-a,color);
        SaveArrPoints[SP++]=xc-b;
        SaveArrPoints[SP++]=yc-a;
    }

    else if( Quarter == 3)
    {
        SetPixel(hdc,xc-a,yc+b,color);
        SaveArrPoints[SP++]=xc-a;
        SaveArrPoints[SP++]=yc+b;

        SetPixel(hdc,xc-b,yc+a,color);
        SaveArrPoints[SP++]=xc-b;
        SaveArrPoints[SP++]=yc+a;
    }
    else
    {
        SetPixel(hdc,xc+a,yc+b,color);
        SaveArrPoints[SP++]=xc+a;
        SaveArrPoints[SP++]=yc+b;

        SetPixel(hdc,xc+b,yc+a,color);
        SaveArrPoints[SP++]=xc+b;
        SaveArrPoints[SP++]=yc+a;
    }
}

void DrawCircleModifiedForFilling(HDC hdc,int xc,int yc,int radius, int Quarter, COLORREF c)
{
    int x=0,Count=0;
    int y=radius;
    int d1=1-radius;
    int c1=3;
    int c2=5-2*radius;
    DrawPartCircle(hdc, xc, yc, x, y, Quarter, c);
    Count+=2;
    while(x<y)
    {
        if(d1<0)
        {
            d1+=c1;
            c2+=2;
        }
        else
        {
            d1+=c2;
            c2+=4;
            y--;
        }
        c1+=2;
        x++;
        DrawPartCircle(hdc, xc, yc, x, y, Quarter, c);
        Count+=2;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}


void DrawCircleWithCircles(HDC hdc,int xc,int yc,int radius,int Quarter,COLORREF c)
{
    DrawCircleModifiedMidpoint(hdc, xc, yc, radius, c);

    for(int i =0 ; i<radius ; i++)
    {
        DrawCircleModifiedForFilling(hdc, xc, yc, i, Quarter, c);
    }
}

// to draw an ellipse
void Draw4Points(HDC hdc, int xc, int yc, int a, int b, COLORREF c )
{
    SetPixel(hdc,xc+a,yc+b,c);
    SaveArrPoints[SP++]=xc+a;
    SaveArrPoints[SP++]=yc+b;
    SetPixel(hdc,xc+a,yc-b,c);
    SaveArrPoints[SP++]=xc+a;
    SaveArrPoints[SP++]=yc-b;
    SetPixel(hdc,xc-a,yc+b,c);
    SaveArrPoints[SP++]=xc-a;
    SaveArrPoints[SP++]=yc+b;
    SetPixel(hdc,xc-a,yc-b,c);
    SaveArrPoints[SP++]=xc-a;
    SaveArrPoints[SP++]=yc-b;
}
int Max(int r1,int r2)
{
    if(r1>r2)
        return r1;
    return r2;
}

// ellipse direct
void DrawEllipseDirect(HDC hdc, int xc, int yc, int r1, int r2, COLORREF c)
{
    int Count=4;
    double x=0;
    double y=r2;
    Draw4Points(hdc,xc,yc,x,y,c);
    while(x*r2*r2<y*r1*r1)
    {
        x++;
        y=sqrt(1-(x*x)/(r1*r1))*r2;
        Draw4Points(hdc,xc,yc,x,Round(y),c);
        Count+=4;
    }
    x=r1;
    y=0;
    while(y*r1*r1<x*r2*r2)
    {
        y++;
        x=sqrt(1-(y*y)/(r2*r2))*r1;
        Draw4Points(hdc,xc,yc,Round(x),y,c);
        Count+=4;
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
//ellipse Polar drawing
void DrawEllipsePolar(HDC hdc, int xc, int yc, int r1, int r2, COLORREF c)
{
    int Count=0;
    double dtheta=1.0/Max(r1,r2);
    double x=r1;
    double y=0;
    double theta=0;
    for( ; x*r2*r2>y*r1*r1; theta+=dtheta)
    {
        Draw4Points(hdc,xc,yc,Round(x),Round(y),c);
        Count+=4;
        x=(r1*cos(theta));
        y=(r2*sin(theta));
    }
    x=0;
    y=r2;
    for( ; x*r2*r2<y*r1*r1; theta+=dtheta)
    {
        Draw4Points(hdc,xc,yc,Round(x),Round(y),c);
        Count+=4;
        x=(r1*cos(theta));
        y=(r2*sin(theta));
    }
    SaveArrColorPoints[SC++]=c;
    SaveArrNoOfPoints[SN++]=Count;
}
// ellipse Mid-point
void DrawEllipseMidPoint(HDC hdc, int xc, int yc, int r1, int r2, COLORREF c)
{
    float dx, dy, d1, d2, x, y;
    x = 0;
    y = r2;
    Draw4Points(hdc, xc, yc, r1, r2, c);
    // Initial decision parameter of region 1
    d1 = (r2 * r2) - (r1 * r1 * r2) + (0.25 * r1 * r1);
    dx = 2 * r2 * r2 * x;
    dy = 2 * r1 * r1 * y;

    // For region 1
    while (dx < dy)
    {
        // Checking and updating value of
        // decision parameter based on algorithm
        if (d1 < 0)
        {
            x++;
            dx = dx + (2 * r2 * r2);
            d1 = d1 + dx + (r2 * r2);
        }
        else
        {
            x++;
            y--;
            dx = dx + (2 * r2 * r2);
            dy = dy - (2 * r1 * r1);
            d1 = d1 + dx - dy + (r2 * r2);
        }
        Draw4Points(hdc, xc, yc, r1, r2, c);
    }

    // Decision parameter of region 2
    d2 = ((r2 * r2) * ((x + 0.5) * (x + 0.5))) +
         ((r1 * r1) * ((y - 1) * (y - 1))) -
         (r1 * r1 * r2 * r2);

    // Plotting points of region 2
    while (y >= 0)
    {
        // Checking and updating parameter
        // value based on algorithm
        if (d2 > 0)
        {
            y--;
            dy = dy - (2 * r1 * r1);
            d2 = d2 + (r1 * r1) - dy;
        }
        else
        {
            y--;
            x++;
            dx = dx + (2 * r2 * r2);
            dy = dy - (2 * r1 * r1);
            d2 = d2 + dx - dy + (r1 * r1);
        }
        Draw4Points(hdc, xc, yc, r1, r2, c);
    }
}

// Flood Fill Warring it has stack overflow

void FloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf)
{
    COLORREF C = GetPixel(hdc,x,y);
    if(C == Cb || C == Cf)return;
    SetPixel(hdc, x, y, Cf);

    FloodFill(hdc, x+1, y, Cb, Cf);
    FloodFill(hdc, x-1, y, Cb, Cf);
    FloodFill(hdc, x, y+1, Cb, Cf);
    FloodFill(hdc, x, y-1, Cb, Cf);
}

// Non-Recursive Flood Fill
struct Vertex
{
    int x,y;
    Vertex(int x,int y):x(x),y(y)
    {
    }
};
void NRFloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf)
{
    stack<Vertex> S;
    S.push(Vertex(x,y));
    while(!S.empty())
    {
        Vertex v=S.top();
        S.pop();
        COLORREF c = GetPixel(hdc, v.x, v.y);
        if(c==Cb || c==Cf)continue;

        SetPixel(hdc,v.x,v.y,Cf);
        S.push(Vertex(v.x+1,v.y));
        S.push(Vertex(v.x-1,v.y));
        S.push(Vertex(v.x,v.y+1));
        S.push(Vertex(v.x,v.y-1));
    }
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
    wincl.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));

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


static int xstart, ystart, xmid, ymid, xend, yend;
static int instruction = 0;
double r, a, b;
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
        // line drawing
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

        // circle drawing
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

        // curve hermit bezier
        case 11:
            instruction = 11;
            MessageBeep(MB_OK);
            cout<<"Circle Modified Mid-Point Drawing Algorithm"<<endl;
            break;
        case 12:
            instruction = 12;
            MessageBeep(MB_OK);
            cout<<"Circle Modified Mid-Point Drawing Algorithm"<<endl;
            break;

        // filling
        case 13:
            instruction = 13;
            MessageBeep(MB_OK);
            cout<<"Circle Modified Mid-Point Drawing Algorithm"<<endl;
            break;
        case 14:
            instruction = 14;
            MessageBeep(MB_OK);
            cout<<"Circle Modified Mid-Point Drawing Algorithm"<<endl;
            break;
        case 15:
            instruction = 15;
            MessageBeep(MB_OK);
            cout<<"Recursive Flood Fill Algorithm"<<endl;
            break;
        case 16:
            instruction = 16;
            MessageBeep(MB_OK);
            cout<<"Non-Recursive Flood Fill Algorithm"<<endl;
            break;
        case 17:
            instruction = 17;
            MessageBeep(MB_OK);
            cout<<"Line PARAMETRIC Drawing Algorithm"<<endl;
            break;

        // Ellipse
        case 18:
            instruction = 18;
            MessageBeep(MB_OK);
            cout<<"Ellipse Direct"<<endl;
            break;
        case 19:
            instruction = 19;
            MessageBeep(MB_OK);
            cout<<"Ellipse Polar"<<endl;
            break;
        case 20:
            instruction = 20;
            MessageBeep(MB_OK);
            cout<<"Ellipse Mid-Point"<<endl;
            break;

        // cardinal spinal filling
        case 21:
            instruction = 21;
            MessageBeep(MB_OK);
            cout<<"Cardinal Spinal Filling Drawing Algorithm"<<endl;
            break;

        // functions
        // save
        case 22:
            MessageBeep(MB_OK);
            cout<<"Saving...."<<endl;
            break;
        // load
        case 23:
            MessageBeep(MB_OK);
            cout<<"Loading...."<<endl;
            break;
        // clear
        case 24:
            cout<<"Clearing....."<<endl;

            // InvalidateRect (hwnd, NULL, NULL);
            ShowWindow(hwnd, SW_HIDE);
            ShowWindow(hwnd, SW_SHOW);

            cout<<"All Items Have Been Cleared"<<endl;
            MessageBeep(MB_OK);
            break;

        // color choosing
        case 26:
            c = RGB(255,0,0);
            MessageBeep(MB_OK);
            cout<<"Color is Red"<<endl;
            break;
        case 27:
            c = RGB(0,255,0);
            MessageBeep(MB_OK);
            cout<<"Color is Green"<<endl;
            break;
        case 28:
            c = RGB(0,0,255);
            MessageBeep(MB_OK);
            cout<<"Color is Blue"<<endl;
            break;
        case 29:
            c = RGB(255,255,255);
            MessageBeep(MB_OK);
            cout<<"Color is White"<<endl;
            break;
        case 30:
            c = RGB(0,0,0);
            MessageBeep(MB_OK);
            cout<<"Color is Black"<<endl;
            break;
        case 31:
            c = RGB(166,166,166);
            MessageBeep(MB_OK);
            cout<<"Color is Gray"<<endl;
            break;
        case 32:
            c = RGB(0,204,204);
            MessageBeep(MB_OK);
            cout<<"Color is Light Blue"<<endl;
            break;

        // circle filling with circles
        case 33:
            instruction = 33;
            MessageBeep(MB_OK);
            break;
        case 34:
            instruction = 34;
            MessageBeep(MB_OK);
            break;
        case 35:
            instruction = 35;
            MessageBeep(MB_OK);
            break;
        case 36:
            instruction = 36;
            MessageBeep(MB_OK);
            break;

        // circle with lines
        case 37:
            instruction = 37;
            MessageBeep(MB_OK);
            break;
        case 38:
            instruction = 38;
            MessageBeep(MB_OK);
            break;
        case 39:
            instruction = 39;
            MessageBeep(MB_OK);
            break;
        case 40:
            instruction = 40;
            MessageBeep(MB_OK);
            break;
        }
    }

    // the Menu Creation
    case WM_CREATE:
    {
        ShowWindow(hwnd, SW_MAXIMIZE);
        HMENU LINE = CreateMenu();
        AppendMenu(LINE, MF_STRING, 1, "Line DDA");
        AppendMenu(LINE, MF_STRING, 2, "Line PARAMETRIC");
        AppendMenu(LINE, MF_STRING, 3, "Line MID-POINT");

        HMENU CIRCLE = CreateMenu();
        AppendMenu(CIRCLE, MF_STRING, 4, "Circle Direct");
        AppendMenu(CIRCLE, MF_SEPARATOR, 0, 0);
        AppendMenu(CIRCLE, MF_STRING, 5, "Circle Polar");
        AppendMenu(CIRCLE, MF_STRING, 6, "Circle Iterative Polar");
        AppendMenu(CIRCLE, MF_SEPARATOR, 0, 0);
        AppendMenu(CIRCLE, MF_STRING, 7, "Circle Mid-Point");
        AppendMenu(CIRCLE, MF_STRING, 8, "Circle Modified Mid-Point");

        HMENU FILLINGSHAPES = CreateMenu();
        HMENU CircleWithLines = CreateMenu();
        AppendMenu(FILLINGSHAPES, MF_POPUP, (UINT_PTR)CircleWithLines, "Circle with lines");
        AppendMenu(CircleWithLines, MF_STRING, 37, "Circle 1st Quarter");
        AppendMenu(CircleWithLines, MF_STRING, 38, "Circle 2nd Quarter");
        AppendMenu(CircleWithLines, MF_SEPARATOR, 0, 0);
        AppendMenu(CircleWithLines, MF_STRING, 39, "Circle 3rd Quarter");
        AppendMenu(CircleWithLines, MF_STRING, 40, "Circle 4th Quarter");

        HMENU CircleWithCircles = CreateMenu();
        AppendMenu(FILLINGSHAPES, MF_POPUP, (UINT_PTR)CircleWithCircles, "Circle with Circles");
        AppendMenu(CircleWithCircles, MF_STRING, 33, "Circle 1st Quarter");
        AppendMenu(CircleWithCircles, MF_STRING, 34, "Circle 2nd Quarter");
        AppendMenu(CircleWithCircles, MF_SEPARATOR, 0, 0);
        AppendMenu(CircleWithCircles, MF_STRING, 35, "Circle 3rd Quarter");
        AppendMenu(CircleWithCircles, MF_STRING, 36, "Circle 4th Quarter");

        AppendMenu(FILLINGSHAPES, MF_SEPARATOR, 0, 0);
        AppendMenu(FILLINGSHAPES, MF_STRING, 11, "Square with Hermit Curve");
        AppendMenu(FILLINGSHAPES, MF_STRING, 12, "Rectangle with Bezier Curve");

        HMENU FILLINGALG = CreateMenu();
        AppendMenu(FILLINGALG, MF_STRING, 13, "Convex");
        AppendMenu(FILLINGALG, MF_STRING, 14, "NoN Convex");
        AppendMenu(FILLINGALG, MF_SEPARATOR, 0, 0);
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
        AppendMenu(ColorMenu,MF_SEPARATOR, 0, 0);
        AppendMenu(ColorMenu,MF_STRING,29,"White");
        AppendMenu(ColorMenu,MF_STRING,30,"Black");
        AppendMenu(ColorMenu,MF_STRING,31,"Grey");
        AppendMenu(ColorMenu,MF_SEPARATOR, 0, 0);
        AppendMenu(ColorMenu,MF_STRING,32,"Light Blue");


        HMENU hmenu = CreateMenu();
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)LINE, "Line");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)CIRCLE, "Circle");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)ELLIPSE, "Ellipse");
        AppendMenu(hmenu, MF_SEPARATOR, 0, 0);

        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)FILLINGSHAPES, "Shape Filling");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)FILLINGALG, "Filling Algorithms");
        AppendMenu(hmenu, MF_POPUP, 21, "Cardinal Spline Curve");
        AppendMenu(hmenu, MF_SEPARATOR, 0, 0);

        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)ColorMenu, "Colors");
        AppendMenu(hmenu, MF_STRING, 22, "Save");
        AppendMenu(hmenu, MF_STRING, 23, "Load");
        AppendMenu(hmenu, MF_STRING, 24, "Clean screen");
        AppendMenu(hmenu, MF_SEPARATOR, 0, 0);

        AppendMenu(hmenu, MF_STRING, 41, "Help");
        AppendMenu(hmenu, MF_STRING, 42, "Exit");
        SetMenu(hwnd,hmenu);
        break;
    }

    case WM_LBUTTONDOWN:
    {
        xstart = LOWORD(lParam);
        ystart = HIWORD(lParam);
        break;
    }

    case WM_RBUTTONDOWN:
    {
        xmid = LOWORD(lParam);
        ymid = HIWORD(lParam);
        break;
    }

    case WM_LBUTTONUP:
    {
        xend = LOWORD(lParam);
        yend = HIWORD(lParam);
        switch(instruction)
        {
        // line drawing
        case 1:
            DrawLineDDA(hdc, xstart, ystart, xend, yend, c);
            cout<<"Line Drawn"<<endl;
            break;
        case 2:
            DrawLineMidPoint(hdc, xstart, ystart, xend, yend, c);
            cout<<"Line Drawn"<<endl;
            break;
        case 3:
            DrawLineParametric(hdc, xstart, ystart, xend, yend, c);
            cout<<"Line Drawn"<<endl;
            break;

        //circle drawing
        case 4:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCircleDirect(hdc, xstart, ystart, r, c);
            cout<<"Circle Drawn"<<endl;
            break;
        case 5:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCirclePolar(hdc, xstart, ystart, r, c);
            cout<<"Circle Drawn"<<endl;
            break;
        case 6:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCirclePolarIterative(hdc, xstart, ystart, r, c);
            cout<<"Circle Drawn"<<endl;
            break;
        case 7:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCircleMidpoint(hdc, xstart, ystart, r, c);
            cout<<"Circle Drawn"<<endl;
            break;
        case 8:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCircleModifiedMidpoint(hdc, xstart, ystart, r, c);
            cout<<"Circle Drawn"<<endl;
            break;

        // filling algorithm
        case 15:
            FloodFill(hdc, xend, yend, RGB(255, 255, 255), c);
            cout<<"Filling Done"<<endl;
            break;
        case 16:
            NRFloodFill(hdc, xend, yend, RGB(255, 255, 255), c);
            cout<<"Filling Done"<<endl;
            break;

        // ellipse drawing
        case 18:
            a = pow(pow(xmid - xstart, 2) + pow(ymid - ystart, 2) * 1.0, 0.5);
            b = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawEllipseDirect(hdc, xstart, ystart, a, b, c);
            cout<<"Ellipse Drawn"<<endl;
            break;
        case 19:
            a = pow(pow(xmid - xstart, 2) + pow(ymid - ystart, 2) * 1.0, 0.5);
            b = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawEllipsePolar(hdc, xstart, ystart, a, b, c);
            cout<<"Ellipse Drawn"<<endl;
            break;
        case 20:
            a = pow(pow(xmid - xstart, 2) + pow(ymid - ystart, 2) * 1.0, 0.5);
            b = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawEllipseMidPoint(hdc, xstart, ystart, a, b, c);
            cout<<"Ellipse Drawn"<<endl;
            break;
        // cardinal spinal curve
        case 21:
            a = pow(pow(xmid - xstart, 2) + pow(ymid - ystart, 2) * 1.0, 0.5);
            b = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawEllipseMidPoint(hdc, xstart, ystart, a, b, c);
            cout<<"Ellipse Drawn"<<endl;
            break;

        // circle filling
        case 33:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCircleWithCircles(hdc, xstart, ystart, r, 1, c);
            cout<<"Circle Drawn"<<endl;
            break;
        case 34:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCircleWithCircles(hdc, xstart, ystart, r, 2, c);
            cout<<"Circle Drawn"<<endl;
            break;
        case 35:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCircleWithCircles(hdc, xstart, ystart, r, 3, c);
            cout<<"Circle Drawn"<<endl;
            break;
        case 36:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCircleWithCircles(hdc, xstart, ystart, r, 4, c);
            cout<<"Circle Drawn"<<endl;
            break;
        case 37:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCircleWithLines(hdc, xstart, ystart, r, 1, c);
            cout<<"Circle Drawn"<<endl;
            break;
        case 38:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCircleWithLines(hdc, xstart, ystart, r, 2, c);
            cout<<"Circle Drawn"<<endl;
            break;
        case 39:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCircleWithLines(hdc, xstart, ystart, r, 3, c);
            cout<<"Circle Drawn"<<endl;
            break;
        case 40:
            r = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            DrawCircleWithLines(hdc, xstart, ystart, r, 4, c);
            cout<<"Circle Drawn"<<endl;
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
