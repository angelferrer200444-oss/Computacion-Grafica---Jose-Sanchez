#define ALGORITMOS_H

#include "raylib.h"
#include <stdlib.h>
#include <math.h>

#define MAX_PUNTOS_LINEA 1000
#define MAX_COLA 360000

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

void lineaWu(float x1, float y1, float x2, float y2, Punto puntos[], int &cantidad)
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

void floodFill(Image *imagen, int xInicial, int yInicial, Punto puntos[], int &cantidad)
{
    cantidad = 0;

    if (xInicial < 0 || xInicial >= imagen->width || yInicial < 0 || yInicial >= imagen->height)
        return;

    Color colorOriginal = GetImageColor(*imagen, xInicial, yInicial);

    static Punto cola[MAX_COLA];

    int frente = 0;
    int final = 0;

    Image imagenTrabajo = ImageCopy(*imagen);

    cola[final].x = xInicial;
    cola[final].y = yInicial;
    cola[final].alpha = 1.0f;
    final++;

    ImageDrawPixel(&imagenTrabajo, xInicial, yInicial, YELLOW);

    while (frente < final && cantidad < MAX_COLA)
    {
        Punto actual = cola[frente];
        frente++;

        puntos[cantidad] = actual;
        puntos[cantidad].alpha = 1.0f;
        cantidad++;

        int dx[4] = { 0, 0, -1, 1 };
        int dy[4] = { -1, 1, 0, 0 };

        for (int i = 0; i < 4; i++)
        {
            int nx = (int)actual.x + dx[i];
            int ny = (int)actual.y + dy[i];

            if (nx >= 0 && nx < imagen->width && ny >= 0 && ny < imagen->height)
            {
                Color vecino = GetImageColor(imagenTrabajo, nx, ny);

                if (ColorIsEqual(vecino, colorOriginal) && final < MAX_COLA)
                {
                    ImageDrawPixel(&imagenTrabajo, nx, ny, YELLOW);

                    cola[final].x = nx;
                    cola[final].y = ny;
                    cola[final].alpha = 1.0f;
                    final++;
                }
            }
        }
    }

    UnloadImage(imagenTrabajo);
}

void scanlineFill(Image *imagen, int xInicial, int yInicial, Punto puntos[], int &cantidad)
{
    cantidad = 0;

    if (xInicial < 0 || xInicial >= imagen->width || yInicial < 0 || yInicial >= imagen->height)
        return;

    Color colorOriginal = GetImageColor(*imagen, xInicial, yInicial);

    static Punto cola[MAX_COLA];

    int frente = 0;
    int final = 0;

    Image imagenTrabajo = ImageCopy(*imagen);

    cola[final].x = xInicial;
    cola[final].y = yInicial;
    cola[final].alpha = 1.0f;
    final++;

    while (frente < final && cantidad < MAX_COLA)
    {
        Punto actual = cola[frente];
        frente++;

        int x = (int)actual.x;
        int y = (int)actual.y;

        if (y < 0 || y >= imagen->height || x < 0 || x >= imagen->width)
            continue;

        if (!ColorIsEqual(GetImageColor(imagenTrabajo, x, y), colorOriginal))
            continue;

        int izquierda = x;

        while (izquierda > 0 && ColorIsEqual(GetImageColor(imagenTrabajo, izquierda - 1, y), colorOriginal))
            izquierda--;

        int derecha = x;

        while (derecha < imagen->width - 1 && ColorIsEqual(GetImageColor(imagenTrabajo, derecha + 1, y), colorOriginal))
            derecha++;

        for (int i = izquierda; i <= derecha && cantidad < MAX_COLA; i++)
        {
            puntos[cantidad].x = (float)i;
            puntos[cantidad].y = (float)y;
            puntos[cantidad].alpha = 1.0f;
            cantidad++;

            ImageDrawPixel(&imagenTrabajo, i, y, YELLOW);
        }

        for (int i = izquierda; i <= derecha; i++)
        {
            if (y > 0 && ColorIsEqual(GetImageColor(imagenTrabajo, i, y - 1), colorOriginal) && final < MAX_COLA)
            {
                cola[final].x = i;
                cola[final].y = y - 1;
                cola[final].alpha = 1.0f;
                final++;

                while (i <= derecha && ColorIsEqual(GetImageColor(imagenTrabajo, i, y - 1), colorOriginal))
                    i++;
            }
        }

        for (int i = izquierda; i <= derecha; i++)
        {
            if (y < imagen->height - 1 && ColorIsEqual(GetImageColor(imagenTrabajo, i, y + 1), colorOriginal) && final < MAX_COLA)
            {
                cola[final].x = i;
                cola[final].y = y + 1;
                cola[final].alpha = 1.0f;
                final++;

                while (i <= derecha && ColorIsEqual(GetImageColor(imagenTrabajo, i, y + 1), colorOriginal))
                    i++;
            }
        }
    }

    cantidad = 0;

    for (int y = 0; y < imagen->height; y++)
    {
        for (int x = 0; x < imagen->width; x++)
        {
            Color pixel = GetImageColor(imagenTrabajo, x, y);

            if (ColorIsEqual(pixel, YELLOW) && cantidad < MAX_COLA)
            {
                puntos[cantidad].x = (float)x;
                puntos[cantidad].y = (float)y;
                puntos[cantidad].alpha = 1.0f;
                cantidad++;
            }
        }
    }

    UnloadImage(imagenTrabajo);
}



