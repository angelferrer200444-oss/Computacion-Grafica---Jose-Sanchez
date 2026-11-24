#define ALGORITMOS_H

#include "raylib.h"
#include <stdlib.h>
#include <math.h>

#define MAX_PUNTOS_LINEA 2000

int algoritmoActual = 0;

struct Punto
{
    float x;
    float y;
    float alpha;
};

void agregarPunto(Punto puntos[], int &cantidad, float x, float y, float alpha)

{
    if (cantidad < MAX_PUNTOS_LINEA)
    {
        puntos[cantidad].x = x;
        puntos[cantidad].y = y;
        puntos[cantidad].alpha = alpha;

        cantidad++;
    }
}

void lineaBresenham(int xa, int ya, int xb, int yb, Punto puntos[], int &cantidad)
{
    cantidad = 0;

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

        agregarPunto(puntos, cantidad, x, y, 1.0f);

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

            agregarPunto(puntos, cantidad, x, y, 1.0f);
        }
    }

    else
    {
        int p = 2 * dx - dy;
        int twoDx = 2 * dx;
        int twoDxDy = 2 * (dx - dy);

        agregarPunto(puntos, cantidad, x, y, 1.0f);

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

            agregarPunto(puntos, cantidad, x, y, 1.0f);
        }
    }
}

int parteEntera(float x)
{
    return (int)floor((double)x);
}

float parteFraccionaria(float x)
{
    return x - parteEntera(x);
}

float parteFraccionariaInversa(float x)
{
    return 1.0f - parteFraccionaria(x);
}

int redondear(float x)
{
    return (int)floor((double)(x + 0.5f));
}

void lineaWu(float x1, float y1, float x2, float y2,
             Punto puntos[], int &cantidad)
{
    cantidad = 0;

    bool empinada = false;

    if (fabs(y2 - y1) > fabs(x2 - x1))
    {
        empinada = true;

        float temp = x1;
        x1 = y1;
        y1 = temp;

        temp = x2;
        x2 = y2;
        y2 = temp;
    }

    if (x1 > x2)
    {
        float temp = x1;
        x1 = x2;
        x2 = temp;

        temp = y1;
        y1 = y2;
        y2 = temp;
    }

    float dx = x2 - x1;
    float dy = y2 - y1;

    float gradient = (dx == 0.0f) ? 0.0f : dy / dx;

    float xend = (float)redondear(x1);
    float yend = y1 + gradient * (xend - x1);

    float xgap = parteFraccionariaInversa(x1 + 0.5f);

    int xpxl1 = (int)xend;
    int ypxl1 = parteEntera(yend);

    float alpha1 = parteFraccionariaInversa(yend) * xgap;
    float alpha2 = parteFraccionaria(yend) * xgap;

    if (empinada)
    {
        agregarPunto(puntos, cantidad, ypxl1, xpxl1, alpha1);
        agregarPunto(puntos, cantidad, ypxl1 + 1, xpxl1, alpha2);
    }
    else
    {
        agregarPunto(puntos, cantidad, xpxl1, ypxl1, alpha1);
        agregarPunto(puntos, cantidad, xpxl1, ypxl1 + 1, alpha2);
    }

    float intery = yend + gradient;

    xend = (float)redondear(x2);
    yend = y2 + gradient * (xend - x2);

    xgap = parteFraccionaria(x2 + 0.5f);

    int xpxl2 = (int)xend;
    int ypxl2 = parteEntera(yend);

    float alpha3 = parteFraccionariaInversa(yend) * xgap;
    float alpha4 = parteFraccionaria(yend) * xgap;

    for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++)
    {
        int y = parteEntera(intery);

        float alpha5 = parteFraccionariaInversa(intery);
        float alpha6 = parteFraccionaria(intery);
        if (empinada)
        {
            agregarPunto(puntos, cantidad, y, x, alpha5);
            agregarPunto(puntos, cantidad, y + 1, x, alpha6);
        }
        else
        {
            agregarPunto(puntos, cantidad, x, y, alpha5);
            agregarPunto(puntos, cantidad, x, y + 1, alpha6);
        }

        intery += gradient;
    }

    if (empinada)
    {
        agregarPunto(puntos, cantidad, ypxl2, xpxl2, alpha3);
        agregarPunto(puntos, cantidad, ypxl2 + 1, xpxl2, alpha4);
    }
    else
    {
        agregarPunto(puntos, cantidad, xpxl2, ypxl2, alpha3);
        agregarPunto(puntos, cantidad, xpxl2, ypxl2 + 1, alpha4);
    }
}

void trazar(float x0, float y0, float x1, float y1, Punto puntos[], int &cantidad)
{
    if (algoritmoActual == 1)
    {
        lineaWu(x0, y0, x1, y1, puntos, cantidad);
    }
    else
    {
        lineaBresenham((int)x0, (int)y0,(int)x1, (int)y1, puntos, cantidad);
    }
}
