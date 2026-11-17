#define ALGORITMOS_H

#include "raylib.h"
#include <stdlib.h>

int algoritmoActual = 0; 

void lineaBresenham(int xa, int ya, int xb, int yb, Color color)
{
    int dx = abs(xb - xa);
    int dy = abs(yb - ya);

    int sx = (xa < xb) ? 1 : -1;
    int sy = (ya < yb) ? 1 : -1;

    int x = xa;
    int y = ya;

    if (dx >= dy) 
    {
        int p = 2 * dy - dx;
        int twoDy = 2 * dy;
        int twoDyDx = 2 * (dy - dx);

        DrawPixel(x, y, color);

        while (x != xb) 
        {
            x += sx;
            if (p < 0) 
            {
                p += twoDy;
            } 
            else 
            {
                y += sy;
                p += twoDyDx;
            }
            DrawPixel(x, y, color);
        }
    }
    else 
    {
        int p = 2 * dx - dy;
        int twoDx = 2 * dx;
        int twoDxDy = 2 * (dx - dy);

        DrawPixel(x, y, color);

        while (y != yb) 
        {
            y += sy;
            if (p < 0) 
            {
                p += twoDx;
            } 
            else 
            {
                x += sx;
                p += twoDxDy;
            }
            DrawPixel(x, y, color);
        }
    }
}

void lineaWu(float x1, float y1, float x2, float y2, Color color)
{
    float dx = x2 - x1;
    float dy = y2 - y1;

    float absDx = (dx < 0.0f) ? -dx : dx;
    float absDy = (dy < 0.0f) ? -dy : dy;

    if (absDx > absDy)
    {
        if (x2 < x1)
        {
            float temp;
            temp = x1; x1 = x2; x2 = temp;
            temp = y1; y1 = y2; y2 = temp;
        }
        
        float gradient = (dx == 0.0f) ? 1.0f : (dy / dx);
        float xend = (float)((int)(x1 + 0.5f));
        float yend = y1 + gradient * (xend - x1);
        float xgap = 1.0f - ((x1 + 0.5f) - (int)(x1 + 0.5f));
        
        int xpxl1 = (int)xend;
        int ypxl1 = (int)yend;
        
        float f1 = yend - (int)yend;
        DrawPixel(xpxl1, ypxl1,     ColorAlpha(color, (1.0f - f1) * xgap));
        DrawPixel(xpxl1, ypxl1 + 1, ColorAlpha(color, f1 * xgap));
        
        float intery = yend + gradient;
 
        xend = (float)((int)(x2 + 0.5f));
        yend = y2 + gradient * (xend - x2);
        xgap = (x2 + 0.5f) - (int)(x2 + 0.5f);
        
        int xpxl2 = (int)xend;
        int ypxl2 = (int)yend;
        
        float f2 = yend - (int)yend;
        DrawPixel(xpxl2, ypxl2,     ColorAlpha(color, (1.0f - f2) * xgap));
        DrawPixel(xpxl2, ypxl2 + 1, ColorAlpha(color, f2 * xgap));
        
        for (int x = xpxl1 + 1; x <= xpxl2 - 1; ++x)
        {
            int y = (int)intery;
            float f = intery - (int)intery;
            DrawPixel(x, y,     ColorAlpha(color, 1.0f - f));
            DrawPixel(x, y + 1, ColorAlpha(color, f));
            intery += gradient;
        }
    } 
    else
    {
        if (y2 < y1)
        {
            float temp;
            temp = x1; x1 = x2; x2 = temp;
            temp = y1; y1 = y2; y2 = temp;
        }

        float gradient = (dy == 0.0f) ? 1.0f : (dx / dy);
        float yend = (float)((int)(y1 + 0.5f));
        float xend = x1 + gradient * (yend - y1);
        float ygap = 1.0f - ((y1 + 0.5f) - (int)(y1 + 0.5f));
        
        int ypxl1 = (int)yend;
        int xpxl1 = (int)xend;

        float f1 = xend - (int)xend;
        DrawPixel(xpxl1,     ypxl1, ColorAlpha(color, (1.0f - f1) * ygap));
        DrawPixel(xpxl1 + 1, ypxl1, ColorAlpha(color, f1 * ygap));
        
        float interx = xend + gradient;
 
        yend = (float)((int)(y2 + 0.5f));
        xend = x2 + gradient * (yend - y2);
        ygap = (y2 + 0.5f) - (int)(y2 + 0.5f);
        
        int ypxl2 = (int)yend;
        int xpxl2 = (int)xend;
        
        float f2 = xend - (int)xend;
        DrawPixel(xpxl2,     ypxl2, ColorAlpha(color, (1.0f - f2) * ygap));
        DrawPixel(xpxl2 + 1, ypxl2, ColorAlpha(color, f2 * ygap));
        
        for (int y = ypxl1 + 1; y <= ypxl2 - 1; ++y)
        {
            int x = (int)interx;
            float f = interx - (int)interx;
            DrawPixel(x,     y, ColorAlpha(color, 1.0f - f));
            DrawPixel(x + 1, y, ColorAlpha(color, f));
            interx += gradient;
        }
    }
}

void trazar(float x0, float y0, float x1, float y1, Color color)
{
    if (algoritmoActual == 1) 
    {
        lineaWu(x0, y0, x1, y1, color);
    } 
    else 
    {
        lineaBresenham((int)x0, (int)y0, (int)x1, (int)y1, color);
    }
}
