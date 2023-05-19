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
#include <list>
#include <algorithm>
#include <string>

#define MAXINT 1e6
#define MAXENTRIES 600

using namespace std;

// for save function
std::vector<int> AllPointsSaved;

// save function
void saveFunctionPoints(int x, int y, COLORREF ha)
{
    AllPointsSaved.push_back(x);
    AllPointsSaved.push_back(y);
    AllPointsSaved.push_back(ha);
}

void SaveToFile(const vector<int>& vec, const char* filename)
{
    ofstream outfile(filename, ios::binary);
    outfile.write(reinterpret_cast<const char*>(&vec[0]), vec.size() * sizeof(int));
}

void SaveFunction(const char* filename)
{
    SaveToFile(AllPointsSaved, filename);
    AllPointsSaved.clear();
}

vector<int> readVectorFromFile(const char* filename)
{
    ifstream infile(filename, ios::binary);
    infile.seekg(0, ios::end);
    int filesize = infile.tellg();
    infile.seekg(0, ios::beg);
    vector<int> vec(filesize / sizeof(int));
    infile.read(reinterpret_cast<char*>(&vec[0]), filesize);
    return vec;
}

void loadFunction(HDC hdc)
{
    vector<int> LoadedPoints = readVectorFromFile("points.txt");

    int xload, yload, cload;
    int haha = 0;
    int siz = LoadedPoints.size();


    for (int i = 0 ; i < siz ; i++)
    {
        if( haha == 0)
        {
            xload = LoadedPoints[i];
            haha++;
        }
        else if ( haha == 1)
        {
            yload = LoadedPoints[i];
            haha++;
        }
        else if ( haha == 2)
        {
            cload = LoadedPoints[i];
            haha = 0;
            SetPixel(hdc, xload, yload, cload);
        }
    }
}


// DDA
int Round(double x)
{
    return (int)(x+0.5);
}
void DrawLineDDA(HDC hdc,int xs,int ys,int xe,int ye,COLORREF color)
{
    int dx=xe-xs;
    int dy=ye-ys;
    SetPixel(hdc,xs,ys,color);
    saveFunctionPoints(xs, ys, color);

    if(abs(dx)>=abs(dy))
    {
        int x=xs,xinc= dx>0?1:-1;
        double y = ys,yinc=(double)dy/dx*xinc;
        while(x!=xe)
        {
            x+=xinc;
            y+=yinc;
            SetPixel(hdc, x, Round(y), color);

            saveFunctionPoints(x, Round(y), color);
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
            SetPixel(hdc, Round(x), y, color);
            saveFunctionPoints(Round(x), y, color);
        }
    }
}

//Mid-Point
void PlotLineLow(HDC hdc,int x0,int y0,int x1, int y1,COLORREF c)
{
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
        SetPixel(hdc, x, y, c);
        saveFunctionPoints(x, y, c);
        if (D > 0)
        {
            y = y + yi;
            D = D + (2 * (dy - dx)) ;
        }
        else
            D = D + 2*dy;
    }
}

void PlotLineHigh(HDC hdc,int x0, int y0,int x1, int y1,COLORREF c)
{
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
        saveFunctionPoints(x, y, c);
        if(D > 0)
        {
            x = x + xi;
            D = D + (2 * (dx - dy));
        }
        else
            D = D + 2*dx;
    }
}

void swapp (int &x1,int &y1,int &x2,int &y2)
{
    int temp =x1;
    x1=x2;
    x2=temp;
    temp =y1;
    y1=y2;
    y2=temp;
}

void DrawLineMidPoint(HDC hdc,int x1,int y1,int x2,int y2,COLORREF c)
{
    int dx = x2-x1;
    int dy = y2-y1;
    if (x1>x2) swapp(x1,y1,x2,y2);
    int x=x1, y =y1;
    SetPixel(hdc, x, y, c);
    saveFunctionPoints(x, y, c);
    int d = dx-2*dy, d1=2*dx -2*dy, d2 = -2*dy ;

    while (x<x2)
    {
        if(d<=0)
        {
            d+=d1;
            x++;
            y++;
        }
        else
        {
            d+=d2;
            x++ ;
        }
        SetPixel(hdc,x, y, c);
        saveFunctionPoints(x, y, c);
    }
}

//Parametric
void DrawLineParametric(HDC hdc,int x1,int y1,int x2, int y2,COLORREF c)
{
    double dx=x2-x1;
    double dy=y2-y1;
    SetPixel(hdc, x1, y1, c);
    saveFunctionPoints(x1, y1, c);

    for(double t=0; t<1; t+=0.00001)
    {
        int x=x1+t*dx;
        int y=y1+t*dy;
        SetPixel(hdc,Round(x),Round(y),c);

        saveFunctionPoints(Round(x), Round(y), c);
    }

}

//Circle Direct
void Draw8Points(HDC hdc,int xc,int yc,int a,int b,COLORREF color)
{
    SetPixel(hdc,xc+a,yc+b,color);
    saveFunctionPoints(xc+a, yc+b, color);

    SetPixel(hdc,xc+a,yc-b,color);
    saveFunctionPoints(xc+a, yc-b, color);

    SetPixel(hdc,xc-a,yc+b,color);
    saveFunctionPoints(xc-a, yc+b, color);

    SetPixel(hdc,xc-a,yc-b,color);
    saveFunctionPoints(xc-a, yc-b, color);

    SetPixel(hdc,xc+b,yc+a,color);
    saveFunctionPoints(xc+b, yc+a, color);

    SetPixel(hdc,xc+b,yc-a,color);
    saveFunctionPoints(xc+b, yc-a, color);

    SetPixel(hdc,xc-b,yc+a,color);
    saveFunctionPoints(xc-b, yc+a, color);

    SetPixel(hdc,xc-b,yc-a,color);
    saveFunctionPoints(xc-b, yc-a, color);
}

void DrawCircleDirect(HDC hdc,int xc,int yc,double R,COLORREF color)
{
    double y = R;

    for(int x=0; x<y ; x++)
    {
        Draw8Points(hdc,xc,yc,x,y,color);

        y = Round(sqrt(R*R-x*x));
    }

}
//circle polar direct algorithm
void DrawCirclePolar(HDC hdc,int xc,int yc,double R,COLORREF color)
{

    double y=0;
    double theta=0;
    double dtheta=1.0/R;

    for(int x=R; x>y ; theta+=dtheta)
    {
        Draw8Points(hdc,xc,yc,Round(x),Round(y),color);
        x=R*cos(theta);
        y=R*sin(theta);
    }

}
//circle polar iterative algorithm
void DrawCirclePolarIterative(HDC hdc,double xc,double yc,double radius,COLORREF c)
{
    double dtheta=1.0/radius;
    double cdtheta=cos(dtheta);
    double sdtheta=sin(dtheta);
    double x=0;
    double y=radius;
    Draw8Points(hdc,xc,yc,x,y,c);

    while(x<y)
    {
        double x1=x*cdtheta-y*sdtheta;
        y=x*sdtheta+y*cdtheta;
        x=x1;
        Draw8Points(hdc,xc,yc,Round(x),Round(y),c);
    }
}
//circle mid-point
void DrawCircleMidpoint(HDC hdc,int xc,int yc,int radius,COLORREF c)
{
    int x = 0;
    int y = radius;
    int d1 = 1-radius;
    Draw8Points(hdc,xc,yc,x,y,c);

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
    }

}
//circle modified midpoint
void DrawCircleModifiedMidpoint(HDC hdc,int xc,int yc,int radius,COLORREF c)
{
    int x=0;
    int y=radius;
    int d1=1-radius;
    int c1=3;
    int c2=5-2*radius;
    Draw8Points(hdc,xc,yc,x,y,c);


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

    }

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
        saveFunctionPoints(xc+a, yc-b, color);

        SetPixel(hdc,xc+b,yc-a,color);
        saveFunctionPoints(xc+b, yc-a, color);

    }

    else if( Quarter == 2)
    {
        SetPixel(hdc,xc-a,yc-b,color);
        saveFunctionPoints(xc-a, yc-b, color);

        SetPixel(hdc,xc-b,yc-a,color);
        saveFunctionPoints(xc-b, yc-a, color);
    }

    else if( Quarter == 3)
    {
        SetPixel(hdc,xc-a,yc+b,color);
        saveFunctionPoints(xc-a, yc+b, color);

        SetPixel(hdc,xc-b,yc+a,color);
        saveFunctionPoints(xc-b, yc+a, color);
    }
    else
    {
        SetPixel(hdc,xc+a,yc+b,color);
        saveFunctionPoints(xc+a, yc+b, color);

        SetPixel(hdc,xc+b,yc+a,color);
        saveFunctionPoints(xc+b, yc+a, color);
    }
}

void DrawCircleModifiedForFilling(HDC hdc,int xc,int yc,int radius, int Quarter, COLORREF c)
{
    int x=0;
    int y=radius;
    int d1=1-radius;
    int c1=3;
    int c2=5-2*radius;
    DrawPartCircle(hdc, xc, yc, x, y, Quarter, c);

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

    }
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
    saveFunctionPoints(xc+a, yc+b, c);

    SetPixel(hdc,xc+a,yc-b,c);
    saveFunctionPoints(xc+a, yc-b, c);

    SetPixel(hdc,xc-a,yc+b,c);
    saveFunctionPoints(xc-a, yc+b, c);

    SetPixel(hdc,xc-a,yc-b,c);
    saveFunctionPoints(xc-a, yc-b, c);
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
    double x=0;
    double y=r2;
    Draw4Points(hdc,xc,yc,x,y,c);

    while(x*r2*r2<y*r1*r1)
    {
        x++;
        y=sqrt(1-(x*x)/(r1*r1))*r2;
        Draw4Points(hdc,xc,yc,x,Round(y),c);
    }

    x=r1;
    y=0;

    while(y*r1*r1<x*r2*r2)
    {
        y++;
        x=sqrt(1-(y*y)/(r2*r2))*r1;
        Draw4Points(hdc,xc,yc,Round(x),y,c);
    }
}
//ellipse Polar drawing
void DrawEllipsePolar(HDC hdc, int xc, int yc, int r1, int r2, COLORREF c)
{
    double dtheta=1.0/Max(r1,r2);
    double x=r1;
    double y=0;
    double theta=0;

    for( ; x*r2*r2>y*r1*r1; theta+=dtheta)
    {
        Draw4Points(hdc,xc,yc,Round(x),Round(y),c);
        x = (r1*cos(theta));
        y = (r2*sin(theta));
    }
    x=0;
    y=r2;

    for( ; x*r2*r2<y*r1*r1; theta+=dtheta)
    {
        Draw4Points(hdc,xc,yc,Round(x),Round(y),c);
        x = (r1*cos(theta));
        y = (r2*sin(theta));
    }

}
// ellipse Mid-point
void DrawEllipseMidPoint(HDC hdc, int xc, int yc, int rx, int ry,COLORREF c)
{
    int x = 0;
    int y = ry;
    int p1 = pow(ry, 2) - pow(rx, 2) * ry + pow(rx, 2) / 4;

    while (2 * pow(ry, 2) * x <= 2 * pow(rx, 2) * y)
    {
        Draw4Points(hdc, xc, yc, x, y, c);

        if (p1 < 0)
        {
            x++;
            p1 += 2 * pow(ry, 2) * x + pow(ry, 2);
        }
        else
        {
            x++;
            y--;
            p1 += 2 * pow(ry, 2) * x - 2 * pow(rx, 2) * y + pow(ry, 2);
        }
    }

    int p2 = pow(ry, 2) * pow(x + 0.5, 2) + pow(rx, 2) * pow(y - 1, 2) - pow(rx, 2) * pow(ry, 2);

    while (y >= 0)
    {
        Draw4Points(hdc, xc, yc, x, y, c);

        if (p2 > 0)
        {
            y--;
            p2 -= 2 * pow(rx, 2) * y + pow(rx, 2);
        }
        else
        {
            x++;
            y--;
            p2 += 2 * pow(ry, 2) * x - 2 * pow(rx, 2) * y + pow(rx, 2);
        }
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
struct VertexFill
{
    int x,y;
    VertexFill(int x,int y):x(x),y(y)
    {
    }
};
void NRFloodFill(HDC hdc,int x,int y,COLORREF Cb,COLORREF Cf)
{
    stack<VertexFill> S;
    S.push(VertexFill(x,y));
    while(!S.empty())
    {
        VertexFill v=S.top();
        S.pop();
        COLORREF c = GetPixel(hdc, v.x, v.y);
        if(c==Cb || c==Cf)continue;

        SetPixel(hdc, v.x, v.y, Cf);
        S.push(VertexFill(v.x+1,v.y));
        S.push(VertexFill(v.x-1,v.y));
        S.push(VertexFill(v.x,v.y+1));
        S.push(VertexFill(v.x,v.y-1));
    }
}

// convex filling

void swap (POINT &a,POINT &b)
{
    POINT tmp=a;
    a=b;
    b=tmp;
}

struct Entry
{
    int xmin, xmax;
};

void InitEntries(Entry table[])
{
    for (int i = 0; i < MAXENTRIES; i++)
    {
        table[i].xmin = MAXINT;
        table[i].xmax = -MAXINT;
    }
}

void ScanEdge(POINT v1, POINT v2, Entry table[])
{
    if (v1.y == v2.y)
        return;
    if (v1.y > v2.y)
        swap(v1, v2);
    double minv = (double)(v2.x - v1.x) / (v2.y - v1.y);
    double x = v1.x;
    int y = v1.y;
    while (y < v2.y)
    {
        if (x < table[y].xmin) table[y].xmin = (int)ceil(x);
        if (x > table[y].xmax) table[y].xmax = (int)floor(x);
        y++;
        x += minv;
    }
}

void DrawScanLines(HDC hdc, Entry table[], COLORREF color)
{
    for (int y = 0; y < MAXENTRIES; y++)
    {
        if (table[y].xmin < table[y].xmax)
        {
            for (int x = table[y].xmin; x <= table[y].xmax; x++)
            {
                SetPixel(hdc, x, y, color);
                saveFunctionPoints(x, y, color);
            }
        }
    }
}

void ConvexFill(HDC hdc, POINT p[], int n, COLORREF color)
{
    Entry *table = new Entry[MAXENTRIES];
    InitEntries(table);
    POINT v1 = p[n - 1];
    for (int i = 0; i < n; i++)
    {
        POINT v2 = p[i];
        ScanEdge(v1, v2, table);
        v1 = p[i];
    }
    DrawScanLines(hdc, table, color);
    delete[] table;
}

// GeneralFill
struct EdgeRec
{
    double x;
    double minv;
    int ymax;
    bool operator<(EdgeRec r)
    {
        return x<r.x;
    }
};
typedef list<EdgeRec> EdgeList;

EdgeRec InitEdgeRec(POINT& v1,POINT& v2)
{
    if(v1.y>v2.y)swap(v1,v2);
    EdgeRec rec;
    rec.x=v1.x;
    rec.ymax=v2.y;
    rec.minv=(double)(v2.x-v1.x)/(v2.y-v1.y);
    return rec;
}


void InitEdgeTable(POINT *polygon,int n,EdgeList table[])
{
    POINT v1=polygon[n-1];
    for(int i=0; i<n; i++)
    {
        POINT v2=polygon[i];
        if(v1.y==v2.y)
        {
            v1=v2;
            continue;
        }
        EdgeRec rec=InitEdgeRec(v1, v2);
        table[v1.y].push_back(rec);
        v1=polygon[i];
    }
}

void GeneralPolygonFill(HDC hdc,POINT *polygon,int n,COLORREF c)
{
    EdgeList *table=new EdgeList [MAXENTRIES];
    InitEdgeTable(polygon,n,table);
    int y=0;

    while(y<MAXENTRIES && table[y].size()==0)
        y++;
    if( y == MAXENTRIES)
        return;

    EdgeList ActiveList=table[y];

    while (ActiveList.size()>0)
    {
        ActiveList.sort();
        for(EdgeList::iterator it=ActiveList.begin(); it!=ActiveList.end(); it++)
        {
            int x1=(int)ceil(it->x);
            it++;
            int x2=(int)floor(it->x);
            for(int x=x1; x<=x2; x++)
            {
                SetPixel(hdc,x,y,c);
                saveFunctionPoints(x, y, c);
            }
        }
        y++;
        EdgeList::iterator it=ActiveList.begin();
        while(it!=ActiveList.end())
            if(y==it->ymax) it=ActiveList.erase(it);
            else it++;
        for(EdgeList::iterator it=ActiveList.begin(); it!=ActiveList.end(); it++)
            it->x+=it->minv;
        ActiveList.insert(ActiveList.end(),table[y].begin(),table[y].end());
    }
    delete[] table;
}


// clipping algorithm

// point clipping

void PointClipping(HDC hdc, int x, int y, int xleft, int ytop, int xright, int ybottom, COLORREF color)
{
    if(x>=xleft && x<= xright && y>=ytop && y<=ybottom)
        SetPixel(hdc,x,y,color);
}


// line clipping
union OutCode
{
    unsigned All:4;
    struct
    {
        unsigned left:1,top:1,right:1,bottom:1;
    };
};

OutCode GetOutCode(double x,double y,int xleft,int ytop,int xright,int ybottom)
{
    OutCode out;
    out.All=0;

    if(x<xleft)
        out.left=1;

    else if(x>xright)
        out.right=1;

    if(y<ytop)
        out.top=1;

    else if(y>ybottom)
        out.bottom=1;

    return out;
}

void VIntersect(double xs,double ys,double xe,double ye,int x,double *xi,double *yi)
{
    *xi=x;
    *yi=ys+(x-xs)*(ye-ys)/(xe-xs);
}
void HIntersect(double xs,double ys,double xe,double ye,int y,double *xi,double *yi)
{
    *yi=y;
    *xi=xs+(y-ys)*(xe-xs)/(ye-ys);
}

void LineClipping(HDC hdc,int xs,int ys,int xe,int ye,int xleft,int ytop,int xright,int ybottom)
{
    double x1=xs,y1=ys,x2=xe,y2=ye;
    OutCode out1=GetOutCode(x1,y1,xleft,ytop,xright,ybottom);
    OutCode out2=GetOutCode(x2,y2,xleft,ytop,xright,ybottom);
    while( (out1.All || out2.All) && !(out1.All & out2.All))
    {
        double xi,yi;
        if(out1.All)
        {
            if(out1.left)VIntersect(x1,y1,x2,y2,xleft,&xi,&yi);
            else if(out1.top)HIntersect(x1,y1,x2,y2,ytop,&xi,&yi);
            else if(out1.right)VIntersect(x1,y1,x2,y2,xright,&xi,&yi);
            else HIntersect(x1,y1,x2,y2,ybottom,&xi,&yi);
            x1=xi;
            y1=yi;
            out1=GetOutCode(x1,y1,xleft,ytop,xright,ybottom);
        }
        else
        {
            if(out2.left)VIntersect(x1,y1,x2,y2,xleft,&xi,&yi);
            else if(out2.top)HIntersect(x1,y1,x2,y2,ytop,&xi,&yi);
            else if(out2.right)VIntersect(x1,y1,x2,y2,xright,&xi,&yi);
            else HIntersect(x1,y1,x2,y2,ybottom,&xi,&yi);
            x2=xi;
            y2=yi;
            out2=GetOutCode(x2,y2,xleft,ytop,xright,ybottom);
        }
    }
    if(!out1.All && !out2.All)
    {
        MoveToEx(hdc,Round(x1),Round(y1),NULL);
        LineTo(hdc,Round(x2),Round(y2));
    }
}

// polygon clipping
struct Vertex
{
    double x,y;
    Vertex(int x1=0,int y1=0)
    {
        x=x1;
        y=y1;
    }
};
typedef vector<Vertex> VertexList;
typedef bool (*IsInFunc)(Vertex& v,int edge);
typedef Vertex (*IntersectFunc)(Vertex& v1,Vertex& v2,int edge);



VertexList ClipWithEdge(VertexList p,int edge,IsInFunc In,IntersectFunc Intersect)
{
    VertexList OutList;
    Vertex v1=p[p.size()-1];
    bool v1_in=In(v1,edge);
    for(int i=0; i<(int)p.size(); i++)
    {
        Vertex v2=p[i];
        bool v2_in=In(v2,edge);
        if(!v1_in && v2_in)
        {
            OutList.push_back(Intersect(v1,v2,edge));
            OutList.push_back(v2);
        }
        else if(v1_in && v2_in) OutList.push_back(v2);
        else if(v1_in) OutList.push_back(Intersect(v1,v2,edge));
        v1=v2;
        v1_in=v2_in;
    }
    return OutList;
}


bool InLeft(Vertex& v,int edge)
{
    return v.x>=edge;
}
bool InRight(Vertex& v,int edge)
{
    return v.x<=edge;
}
bool InTop(Vertex& v,int edge)
{
    return v.y>=edge;
}
bool InBottom(Vertex& v,int edge)
{
    return v.y<=edge;
}


Vertex VIntersect(Vertex& v1,Vertex& v2,int xedge)
{
    Vertex res;
    res.x=xedge;
    res.y=v1.y+(xedge-v1.x)*(v2.y-v1.y)/(v2.x-v1.x);
    return res;
}
Vertex HIntersect(Vertex& v1,Vertex& v2,int yedge)
{
    Vertex res;
    res.y=yedge;
    res.x=v1.x+(yedge-v1.y)*(v2.x-v1.x)/(v2.y-v1.y);
    return res;
}


void PolygonClip(HDC hdc,POINT *p,int n,int xleft,int ytop,int xright,int ybottom)
{
    VertexList vlist;
    for(int i=0; i<n; i++)vlist.push_back(Vertex(p[i].x,p[i].y));
    vlist=ClipWithEdge(vlist,xleft,InLeft,VIntersect);
    vlist=ClipWithEdge(vlist,ytop,InTop,HIntersect);
    vlist=ClipWithEdge(vlist,xright,InRight,VIntersect);
    vlist=ClipWithEdge(vlist,ybottom,InBottom,HIntersect);
    Vertex v1=vlist[vlist.size()-1];
    for(int i=0; i<(int)vlist.size(); i++)
    {
        Vertex v2=vlist[i];
        MoveToEx(hdc,Round(v1.x),Round(v1.y),NULL);
        LineTo(hdc,Round(v2.x),Round(v2.y));
        v1=v2;
    }
}

// Curve Drawing
struct vector2
{
    double x,y;
    vector2(double a=0,double b=0)
    {
        x=a;
        y=b;
    }
};

class vector4
{
    double v[4];
public:
    vector4(double a=0, double b=0, double c=0, double d=0)
    {
        v[0]=a;
        v[1]=b;
        v[2]=c;
        v[3]=d;
    }
    vector4(double a[])
    {
        memcpy(v,a,4* sizeof(double));
    }
    double& operator[](int i)
    {
        return v[i];
    }
};

class matrix4
{
    vector4 M[4];
public:
    matrix4(double  A[])
    {
        memcpy(M,A,16* sizeof(double));
    }
    vector4& operator[](int i)
    {
        return M[i];
    }
};

vector4 operator*(matrix4 M, vector4& b) //right multiplication of M and b
{
    vector4 res;
    for(int i=0; i<4; i++)
    {
        for(int j=0; j<4; j++)
        {
            res[i]+=M[i][j]*b[j];
        }
    }
    return res;
}

double DotProduct(vector4& a, vector4& b) //multiplying a row vector by a column vector
{
    return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3];
}

vector4 GetHermiteCoeff(double x0,double s0,double x1,double s1)
{
    static double H[16]= {2,1,-2,1,-3,-2,3,-1,0,1,0,0,1,0,0,0};
    static matrix4 basis(H);
    vector4 v(x0,s0,x1,s1);
    return basis*v;
}

void DrawHermiteCurve(HDC hdc,vector2& p0,vector2& t0,vector2& p1,vector2& t1,int numpoints)
{
    vector4 xcoeff= GetHermiteCoeff(p0.x,t0.x,p1.x,t1.x);
    vector4 ycoeff= GetHermiteCoeff(p0.y,t0.y,p1.y,t1.y);

    if(numpoints<2)
    {
        return;
    }
    double dt=0.00001;

    for(double t=0; t<=1; t+=dt)
    {
        vector4 vt;
        vt[3]=1;
        for(int i=2; i>=0; i--)
        {
            vt[i]=vt[i+1]*t;
        }

        int x=round(DotProduct(xcoeff,vt));
        int y=round(DotProduct(ycoeff,vt));

        if(t==0)
        {
            MoveToEx(hdc,x,y,NULL);
        }
        else
        {
            LineTo(hdc,x,y);
        }
    }
}

void DrawBezierCurve(HDC hdc, vector2& P0, vector2& P1, vector2& P2, vector2& P3, int numpoints)
{
    vector2 T0(3*(P1.x-P0.x),3*(P1.y-P0.y));
    vector2 T1(3*(P3.x-P2.x),3*(P3.y-P2.y));
    DrawHermiteCurve(hdc,P0,T0,P3,T1,numpoints);
}

void DrawCardinalSpline(HDC hdc, vector2 P[], int n, double c, int numpix)
{
    double c1=1-c;
    vector2 T0(c1*(P[2].x-P[0].x),c1*(P[2].y-P[0].y));
    for(int i=2; i<n-1; i++)
    {
        vector2 T1(c1*(P[i+1].x-P[i-1].x),c1*(P[i+1].y-P[i-1].y));
        DrawHermiteCurve(hdc,P[i-1],T0,P[i],T1,numpix);
        T0=T1;
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
    ShowWindow(hwnd, SW_MAXIMIZE);
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

COLORREF c = RGB(0, 0, 0);
static int xstart, ystart, xmid, ymid, xend, yend;
static int instruction = 0;
double r, a, b;
//
static int xleft, xright, ytop, ybottom;
static int xinter, yinter;
static int clippingCounter = 0;
//
static float s_facx = 0.0;
static float s_facy = 0.0;
static int tx = 0;
static int ty = 0;
//
static int counter = 0;
POINT p[5];
POINT pTemp;
//
static vector2 x0,x1,s0,s1;
static int counterC=0;
//
vector2 po[7];
vector2 ptemp;
//
vector2 pointss[4];
static int counterS=0;
vector2 midpoint1, midpoint2, P1, P2;
vector2 Tan0,Tan1;

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
        case 9:
            cout << "Enter Scaling factor in x-axis:";
            cin >> s_facx;  // Scaling factor in x-axis
            cout << "Enter Scaling factor in y-axis:";
            cin >> s_facy ;  // Scaling factor in y-axis

            // Apply scaling
            xstart = static_cast<int>(xstart * s_facx);
            ystart = static_cast<int>(ystart * s_facy);
            xend = static_cast<int>(xend * s_facx);
            yend = static_cast<int>(yend * s_facy);

            cout << "Scaling Done" << endl;

            DrawLineDDA(hdc, xstart, ystart, xend, yend, c);

            break;
        case 10:
            instruction = 10;
            MessageBeep(MB_OK);
            cout<<"Square with Hermit Curve"<<endl;

        // curve hermit bezier
        case 11:
            instruction = 11;
            MessageBeep(MB_OK);
            cout<<"Filling A square with Hermit Curve"<<endl;
            break;
        case 12:
            instruction = 12;
            MessageBeep(MB_OK);
            cout<<"Filling A Rectangle with Bezier Curve"<<endl;
            break;

        // filling
        case 13:
            instruction = 13;
            counter = 0;
            MessageBeep(MB_OK);
            cout<<"Convex Filling A Shape with 5 points"<<endl;
            break;
        case 14:
            instruction = 14;
            counter = 0;
            MessageBeep(MB_OK);
            cout<<"NoN Convex Filling A Shape with 5 points"<<endl;
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
            SaveFunction("points.txt");
            cout<<"Saved Successfully"<<endl;
            break;

        // load
        case 23:
            MessageBeep(MB_OK);
            cout<<"Loading...."<<endl;
            loadFunction(hdc);
            cout<<"Loaded Successfully"<<endl;
            break;

        // clear
        case 24:
            cout<<"Clearing....."<<endl;

            // InvalidateRect (hwnd, NULL, NULL);
            ShowWindow(hwnd, SW_HIDE);
            ShowWindow(hwnd, SW_SHOW);
            AllPointsSaved.clear();

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
        case 41:
            MessageBox(hwnd, "To Draw A Line: Drag the Start point to the end point of line With Left Click \n"
                       "To Draw A Circle: press left click to define the center and release to the edge of the circle you like \n"
                       "To Draw An Ellipse: "
                       , "How To Draw", MB_OK);
            break;
        // clipping algorithms
        case 43:
            instruction = 43;
            MessageBeep(MB_OK);
            break;
        case 44:
            instruction = 44;
            MessageBeep(MB_OK);
            break;
        case 45:
            instruction = 45;
            MessageBeep(MB_OK);
            break;
        case 46:
            instruction = 46;
            MessageBeep(MB_OK);
            break;
        case 47:
            instruction = 47;
            MessageBeep(MB_OK);
            break;
        // 2D translation
        case 48:
            cout << "Enter Translation in x-axis:";
            cin >> tx;  // Translation in x-axis
            cout << "Enter Translation in y-axis:";
            cin >> ty;  // Translation in y-axis

            // Apply translation
            xstart += tx;
            ystart += ty;
            xend += tx;
            yend += ty;

            DrawLineDDA(hdc, xstart, ystart, xend, yend, c);

            cout << " Translation Done " << endl;

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

        HMENU ColorMenu = CreateMenu();
        AppendMenu(ColorMenu, MF_STRING,26,"Red");
        AppendMenu(ColorMenu, MF_STRING,27,"Green");
        AppendMenu(ColorMenu, MF_STRING,28,"Blue");
        AppendMenu(ColorMenu, MF_SEPARATOR, 0, 0);
        AppendMenu(ColorMenu, MF_STRING,29,"White");
        AppendMenu(ColorMenu, MF_STRING,30,"Black");
        AppendMenu(ColorMenu, MF_STRING,31,"Grey");
        AppendMenu(ColorMenu, MF_SEPARATOR, 0, 0);
        AppendMenu(ColorMenu, MF_STRING,32,"Light Blue");

        HMENU ClippingMenu = CreateMenu();

        HMENU ClippingRectangel = CreateMenu();
        AppendMenu(ClippingMenu, MF_POPUP, (UINT_PTR)ClippingRectangel, "Clipping Rectangle");
        AppendMenu(ClippingRectangel, MF_STRING, 43, "Point");
        AppendMenu(ClippingRectangel, MF_STRING, 44, "Line");
        AppendMenu(ClippingRectangel, MF_STRING, 45, "Polygon");

        HMENU ClippingSquare = CreateMenu();
        AppendMenu(ClippingMenu, MF_POPUP, (UINT_PTR)ClippingSquare, "Clipping Square");
        AppendMenu(ClippingSquare, MF_STRING, 46, "Point");
        AppendMenu(ClippingSquare, MF_STRING, 47, "Line");

        HMENU Transformation = CreateMenu();
        AppendMenu(Transformation, MF_STRING, 48, "Translate");
        AppendMenu(Transformation, MF_STRING, 9, "Scale");

        // main menu
        HMENU hmenu = CreateMenu();
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)LINE, "Line");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)CIRCLE, "Circle");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)ELLIPSE, "Ellipse");
        AppendMenu(hmenu, MF_SEPARATOR, 0, 0);

        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)FILLINGSHAPES, "Shape Filling");
        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)FILLINGALG, "Filling Algorithms");
        AppendMenu(hmenu, MF_POPUP, 21, "Cardinal Spline Curve");
        AppendMenu(hmenu, MF_SEPARATOR, 0, 0);

        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)ClippingMenu, "Clipping Algorithms");
        AppendMenu(hmenu, MF_SEPARATOR, 0, 0);

        AppendMenu(hmenu, MF_POPUP, (UINT_PTR)Transformation, "2D Transformation");
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
        if ( instruction == 11)
        {
            Tan0.x=150,Tan0.y=0,Tan1.x=-150,Tan1.y=0;
            if(counterS<=5)
            {
                pointss[counterS].x = LOWORD(lParam);
                pointss[counterS].y = HIWORD(lParam);
                counterS++;

                if(counterS==4)
                {
                    midpoint1.x=(pointss[0].x + pointss[1].x)/2;
                    midpoint1.y=(pointss[0].y + pointss[1].y)/2;
                    midpoint2.x=(pointss[2].x + pointss[3].x)/2;
                    midpoint2.y=(pointss[2].y + pointss[3].y)/2;

                    xleft = pointss[0].x;
                    ytop = pointss[0].y;

                    xright = pointss[1].x;

                    ybottom = pointss[2].y;
                    // upwards
                    if ( ytop > ybottom)
                    {
                        int temp = xright - xleft;
                        ybottom = ytop - temp;
                        swap(ytop, ybottom);
                    }
                    // downwards
                    else
                    {
                        int temp = xright - xleft;
                        ybottom = ytop + temp;
                    }
                    DrawLineDDA(hdc, xleft, ytop, xright, ytop, c);
                    DrawLineDDA(hdc, xleft, ybottom, xright, ybottom, c);
                    DrawLineDDA(hdc, xleft, ybottom, xleft, ytop, c);
                    DrawLineDDA(hdc, xright, ybottom, xright, ytop, c);
                    cout<<"Square Drawn"<<endl;



                    DrawHermiteCurve(hdc,pointss[0],Tan0,pointss[3],Tan1,c);

                    Tan0.x=-150,Tan1.x=150;

                    DrawHermiteCurve(hdc,pointss[1],Tan0,pointss[2],Tan1,c);

                    Tan0.x=150,Tan1.x=-150;

                    DrawHermiteCurve(hdc,midpoint1,Tan0,midpoint2,Tan1,c);

                    ReleaseDC(hwnd, hdc);

                    counterS=0;
                }
            }
        }

        break;
    }

    case WM_RBUTTONDOWN:
    {
        xmid = LOWORD(lParam);
        ymid = HIWORD(lParam);

        if (instruction == 43 || instruction == 46)
        {
            PointClipping(hdc, xmid, ymid, xleft, ytop, xright, ybottom, c);
            cout<<"Point Drawn"<<endl;
        }
        else if (instruction == 45)
        {
            if(counter == 0)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter++;
            }
            else if(counter == 1)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter++;
            }
            else if(counter == 2)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter++;
            }
            else if(counter == 3)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter++;
                cout<<"Shape Drawn"<<endl;
            }
            else if(counter == 4)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter = 0;
                PolygonClip(hdc, p, 5, xleft, ytop, xright, ybottom);
                cout<<"Shape Drawn"<<endl;
            }
        }

        break;
    }

    case WM_RBUTTONUP:
    {
        xinter = LOWORD(lParam);
        yinter = HIWORD(lParam);

        if (instruction == 44 || instruction == 47)
        {
            LineClipping(hdc, xmid, ymid, xinter, yinter, xleft, ytop, xright, ybottom);
            cout<<"Line Drawn"<<endl;
        }

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
        // Curve Filling
        case 11:
            if(counterC == 0)
            {
                x0.x = LOWORD(lParam);
                x0.y = HIWORD(lParam);
                counterC++;
            }
            else if(counterC == 1)
            {
                s0.x = LOWORD(lParam);
                s0.y = HIWORD(lParam);
                counterC++;
            }
            else if (counterC == 2)
            {
                s1.x = LOWORD(lParam);
                s1.y = HIWORD(lParam);
                counterC++;
            }
            else
            {
                x1.x=LOWORD(lParam);
                x1.y=HIWORD(lParam);
                DrawHermiteCurve(hdc, x0, s0, x1, s1, 4);
                cout<<"Curve is Drawn"<<endl;
                counterC = 0;
            }
            break;
        case 12:
            if(counterC == 0)
            {
                x0.x = LOWORD(lParam);
                x0.y = HIWORD(lParam);
                counterC++;
            }
            else if(counterC == 1)
            {
                s0.x = LOWORD(lParam);
                s0.y = HIWORD(lParam);
                counterC++;
            }
            else if (counterC == 2)
            {
                s1.x = LOWORD(lParam);
                s1.y = HIWORD(lParam);
                counterC++;
            }
            else
            {
                x1.x=LOWORD(lParam);
                x1.y=HIWORD(lParam);
                DrawBezierCurve(hdc, x0, s0, s1, x1, 4);
                cout<<"Curve is Drawn"<<endl;
                counterC = 0;
            }
            break;

        // convex and Non convex filling

        case 13:
            if(counter == 0)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter++;
            }
            else if(counter == 1)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter++;
            }
            else if(counter == 2)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter++;
            }
            else if(counter == 3)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter++;
            }
            else if(counter == 4)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter = 0;
                ConvexFill(hdc, p, 5, c);
                cout<<"Shape Filled"<<endl;
            }

            break;
        case 14:
            if(counter == 0)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter++;
            }
            else if(counter == 1)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter++;
            }
            else if(counter == 2)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter++;
            }
            else if(counter == 3)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter++;
            }
            else if(counter == 4)
            {
                pTemp.x = xend;
                pTemp.y = yend;
                p[counter] = pTemp;
                counter = 0;
                GeneralPolygonFill(hdc, p, 5, c);
                cout<<"Shape Filled"<<endl;
            }

            break;

        // filling algorithm
        case 15:
            FloodFill(hdc, xend, yend,  RGB(255, 0,0), c);
            cout<<"Filling Done"<<endl;
            break;
        case 16:
            NRFloodFill(hdc, xend, yend, RGB(255, 0,0), c);
            cout<<"Filling Done"<<endl;
            break;

        // ellipse drawing
        case 18:
            a = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            b = pow(pow(xmid - xstart, 2) + pow(ymid - ystart, 2) * 1.0, 0.5);
            DrawEllipseDirect(hdc, xstart, ystart, a, b, c);
            cout<<"Ellipse Drawn"<<endl;
            break;
        case 19:
            a = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            b = pow(pow(xmid - xstart, 2) + pow(ymid - ystart, 2) * 1.0, 0.5);
            DrawEllipsePolar(hdc, xstart, ystart, a, b, c);
            cout<<"Ellipse Drawn"<<endl;
            break;
        case 20:
            a = pow(pow(xend - xstart, 2) + pow(yend - ystart, 2) * 1.0, 0.5);
            b = pow(pow(xmid - xstart, 2) + pow(ymid - ystart, 2) * 1.0, 0.5);
            DrawEllipseMidPoint(hdc, xstart, ystart, a, b, c);
            cout<<"Ellipse Drawn"<<endl;
            break;

        // cardinal spinal curve
        case 21:
            if(counterC == 0)
            {
                ptemp.x = LOWORD(lParam);
                ptemp.y = HIWORD(lParam);
                po[0] = ptemp;
                counterC++;
            }
            else if(counterC == 1)
            {
                ptemp.x = LOWORD(lParam);
                ptemp.y = HIWORD(lParam);
                po[1] = ptemp;
                counterC++;
            }
            else if (counterC == 2)
            {
                ptemp.x = LOWORD(lParam);
                ptemp.y = HIWORD(lParam);
                po[2] = ptemp;
                counterC++;
            }
            else if (counterC == 3)
            {
                ptemp.x = LOWORD(lParam);
                ptemp.y = HIWORD(lParam);
                po[3] = ptemp;
                counterC++;
            }
            else if (counterC == 4)
            {
                ptemp.x = LOWORD(lParam);
                ptemp.y = HIWORD(lParam);
                po[4] = ptemp;
                counterC++;
            }
            else if (counterC == 5)
            {
                ptemp.x = LOWORD(lParam);
                ptemp.y = HIWORD(lParam);
                po[5] = ptemp;
                counterC++;
            }
            else
            {
                ptemp.x = LOWORD(lParam);
                ptemp.y = HIWORD(lParam);
                po[6] = ptemp;
                DrawCardinalSpline(hdc, po, 7, 0.5, 7);
                cout<<"Spinal Curve is Drawn"<<endl;
                counterC = 0;
            }
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

        // clipping
        case 43:
            if(clippingCounter == 0)
            {
                xleft = LOWORD(lParam);
                ytop = HIWORD(lParam);
                clippingCounter++;
            }
            else if(clippingCounter == 1)
            {
                xright = LOWORD(lParam);
                clippingCounter++;
            }
            else
            {
                ybottom = HIWORD(lParam);
                if ( ytop > ybottom)
                {
                    swap(ytop, ybottom);
                }
                DrawLineDDA(hdc, xleft, ytop, xright, ytop, c);
                DrawLineDDA(hdc, xleft, ybottom, xright, ybottom, c);
                DrawLineDDA(hdc, xleft, ybottom, xleft, ytop, c);
                DrawLineDDA(hdc, xright, ybottom, xright, ytop, c);
                cout<<"Clipping Window Drawn"<<endl;
                clippingCounter = 0;
            }
            break;
        case 44:
            if(clippingCounter == 0)
            {
                xleft = LOWORD(lParam);
                ytop = HIWORD(lParam);
                clippingCounter++;
            }
            else if(clippingCounter == 1)
            {
                xright = LOWORD(lParam);
                clippingCounter++;
            }
            else
            {
                ybottom = HIWORD(lParam);
                if ( ytop > ybottom)
                {
                    swap(ytop, ybottom);
                }
                DrawLineDDA(hdc, xleft, ytop, xright, ytop, c);
                DrawLineDDA(hdc, xleft, ybottom, xright, ybottom, c);
                DrawLineDDA(hdc, xleft, ybottom, xleft, ytop, c);
                DrawLineDDA(hdc, xright, ybottom, xright, ytop, c);
                cout<<"Clipping Window Drawn"<<endl;
                clippingCounter = 0;
            }
            break;
        case 45:
            if(clippingCounter == 0)
            {
                xleft = LOWORD(lParam);
                ytop = HIWORD(lParam);
                clippingCounter++;
            }
            else if(clippingCounter == 1)
            {
                xright = LOWORD(lParam);
                clippingCounter++;
            }
            else
            {
                ybottom = HIWORD(lParam);
                if ( ytop > ybottom)
                {
                    swap(ytop, ybottom);
                }
                if ( xright < xleft)
                {
                    swap(xright, xleft);
                }
                DrawLineDDA(hdc, xleft, ytop, xright, ytop, c);
                DrawLineDDA(hdc, xleft, ybottom, xright, ybottom, c);
                DrawLineDDA(hdc, xleft, ybottom, xleft, ytop, c);
                DrawLineDDA(hdc, xright, ybottom, xright, ytop, c);
                cout<<"Clipping Window Drawn"<<endl;
                clippingCounter = 0;
            }
            break;
        // square
        case 46:
            if(clippingCounter == 0)
            {
                xleft = LOWORD(lParam);
                ytop = HIWORD(lParam);
                clippingCounter++;
            }
            else if(clippingCounter == 1)
            {
                xright = LOWORD(lParam);
                clippingCounter++;
            }
            else
            {
                ybottom = HIWORD(lParam);
                // upwards
                if ( ytop > ybottom)
                {
                    int temp = xright - xleft;
                    ybottom = ytop - temp;
                    swap(ytop, ybottom);
                }
                // downwards
                else
                {
                    int temp = xright - xleft;
                    ybottom = ytop + temp;
                }
                DrawLineDDA(hdc, xleft, ytop, xright, ytop, c);
                DrawLineDDA(hdc, xleft, ybottom, xright, ybottom, c);
                DrawLineDDA(hdc, xleft, ybottom, xleft, ytop, c);
                DrawLineDDA(hdc, xright, ybottom, xright, ytop, c);
                cout<<"Clipping Window Drawn"<<endl;
                clippingCounter = 0;
            }
            break;
        case 47:
            if(clippingCounter == 0)
            {
                xleft = LOWORD(lParam);
                ytop = HIWORD(lParam);
                clippingCounter++;
            }
            else if(clippingCounter == 1)
            {
                xright = LOWORD(lParam);
                clippingCounter++;
            }
            else
            {
                ybottom = HIWORD(lParam);
                // upwards
                if ( ytop > ybottom)
                {
                    int temp = xright - xleft;
                    ybottom = ytop - temp;
                    swap(ytop, ybottom);
                }
                // downwards
                else
                {
                    int temp = xright - xleft;
                    ybottom = ytop + temp;
                }
                DrawLineDDA(hdc, xleft, ytop, xright, ytop, c);
                DrawLineDDA(hdc, xleft, ybottom, xright, ybottom, c);
                DrawLineDDA(hdc, xleft, ybottom, xleft, ytop, c);
                DrawLineDDA(hdc, xright, ybottom, xright, ytop, c);
                cout<<"Clipping Window Drawn"<<endl;
                clippingCounter = 0;
            }
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
