#define ALGORITMOS_H

#include "raylib.h"
#include <math.h>
#include <vector>
#include <algorithm>

#define MAX_PUNTOS_LINEA 2000

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

void lineaWu(float x1, float y1, float x2, float y2, Punto puntos[], int &cantidad)
{
    auto parteEntera = [](float x) -> int
    {
        return (int)floor((double)x);
    };

    auto parteFraccionaria = [parteEntera](float x) -> float
    {
        return x - parteEntera(x);
    };

    auto parteFraccionariaInversa = [parteFraccionaria](float x) -> float
    {
        return 1.0f - parteFraccionaria(x);
    };

    auto redondear = [](float x) -> int
    {
        return (int)floor((double)(x + 0.5f));
    };

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

struct EstadoFloodFill
{
    std::vector<Punto> cola;
    size_t frente;
    Color colorObjetivo;
    Color colorRelleno;
    bool activo;
};

void iniciarFloodFill(EstadoFloodFill &estado, Image *canvas, int inicioX, int inicioY, Color colorRelleno)
{
    estado.cola.clear();
    estado.frente = 0;
    estado.colorRelleno = colorRelleno;
    estado.activo = false;

    if (canvas == nullptr || canvas->data == nullptr) return;
    if (inicioX < 0 || inicioX >= canvas->width || inicioY < 0 || inicioY >= canvas->height) return;

    estado.colorObjetivo = GetImageColor(*canvas, inicioX, inicioY);
    if (ColorIsEqual(estado.colorObjetivo, colorRelleno)) return;

    ImageDrawPixel(canvas, inicioX, inicioY, colorRelleno);
    estado.cola.push_back({ (float)inicioX, (float)inicioY, 1.0f });
    estado.activo = true;
}

void pasoFloodFill(EstadoFloodFill &estado, Image *canvas, int cuadrosPorFrame)
{
    if (!estado.activo || canvas == nullptr || canvas->data == nullptr || cuadrosPorFrame <= 0) return;

    const int dx[4] = { 1, -1, 0, 0 };
    const int dy[4] = { 0, 0, 1, -1 };

    int pintados = 0;
    while (estado.frente < estado.cola.size() && pintados < cuadrosPorFrame)
    {
        Punto actual = estado.cola[estado.frente++];
        int ax = (int)actual.x;
        int ay = (int)actual.y;

        for (int i = 0; i < 4; i++)
        {
            int nx = ax + dx[i];
            int ny = ay + dy[i];

            if (nx >= 0 && nx < canvas->width && ny >= 0 && ny < canvas->height)
            {
                if (ColorIsEqual(GetImageColor(*canvas, nx, ny), estado.colorObjetivo))
                {
                    ImageDrawPixel(canvas, nx, ny, estado.colorRelleno);
                    estado.cola.push_back({ (float)nx, (float)ny, 1.0f });
                    pintados++;
                }
            }
        }
    }

    if (estado.frente >= estado.cola.size())
    {
        estado.activo = false;
    }
}

void rellenoFloodFill(Image *canvas, int inicioX, int inicioY, Color colorRelleno)
{
    EstadoFloodFill estado;
    iniciarFloodFill(estado, canvas, inicioX, inicioY, colorRelleno);
    while (estado.activo)
    {
        pasoFloodFill(estado, canvas, 1024);
    }
}

struct SegmentoScanline
{
    int y;
    int xInicio;
    int xFin;
};

struct EstadoScanline
{
    std::vector<SegmentoScanline> segmentos;
    size_t indiceSegmento;
    int xActual;
    Color colorRelleno;
    bool activo;
};

void iniciarScanline(EstadoScanline &estado, Image *canvas, Punto vertices[], int cantidad, Color colorRelleno)
{
    estado.segmentos.clear();
    estado.indiceSegmento = 0;
    estado.xActual = 0;
    estado.colorRelleno = colorRelleno;
    estado.activo = false;

    if (canvas == nullptr || canvas->data == nullptr || cantidad < 3) return;

    float minYf = vertices[0].y;
    float maxYf = vertices[0].y;

    for (int i = 1; i < cantidad; i++)
    {
        if (vertices[i].y < minYf) minYf = vertices[i].y;
        if (vertices[i].y > maxYf) maxYf = vertices[i].y;
    }

    int yMin = (int)floor((double)minYf);
    int yMax = (int)ceil((double)maxYf);

    if (yMin < 0) yMin = 0;
    if (yMax >= canvas->height) yMax = canvas->height - 1;

    std::vector<float> intersecciones;

    for (int y = yMin; y <= yMax; y++)
    {
        intersecciones.clear();
        float scanY = (float)y + 0.5f;

        for (int i = 0; i < cantidad; i++)
        {
            int siguiente = (i + 1) % cantidad;
            float x1 = vertices[i].x;
            float y1 = vertices[i].y;
            float x2 = vertices[siguiente].x;
            float y2 = vertices[siguiente].y;

            if ((y1 <= scanY && y2 > scanY) || (y2 <= scanY && y1 > scanY))
            {
                float xInterseccion = x1 + (scanY - y1) * (x2 - x1) / (y2 - y1);
                intersecciones.push_back(xInterseccion);
            }
        }

        std::sort(intersecciones.begin(), intersecciones.end());

        for (size_t k = 0; k + 1 < intersecciones.size(); k += 2)
        {
            int xInicio = (int)floor((double)(intersecciones[k] + 0.5f));
            int xFin = (int)floor((double)(intersecciones[k + 1] + 0.5f));

            if (xInicio < 0) xInicio = 0;
            if (xFin >= canvas->width) xFin = canvas->width - 1;

            if (xInicio <= xFin)
            {
                estado.segmentos.push_back({ y, xInicio, xFin });
            }
        }
    }

    if (!estado.segmentos.empty())
    {
        estado.xActual = estado.segmentos[0].xInicio;
        estado.activo = true;
    }
}

void pasoScanline(EstadoScanline &estado, Image *canvas, int cuadrosPorFrame)
{
    if (!estado.activo || canvas == nullptr || canvas->data == nullptr || cuadrosPorFrame <= 0) return;

    int pintados = 0;
    while (estado.indiceSegmento < estado.segmentos.size() && pintados < cuadrosPorFrame)
    {
        SegmentoScanline &seg = estado.segmentos[estado.indiceSegmento];

        ImageDrawPixel(canvas, estado.xActual, seg.y, estado.colorRelleno);
        pintados++;
        estado.xActual++;

        if (estado.xActual > seg.xFin)
        {
            estado.indiceSegmento++;
            if (estado.indiceSegmento < estado.segmentos.size())
            {
                estado.xActual = estado.segmentos[estado.indiceSegmento].xInicio;
            }
        }
    }

    if (estado.indiceSegmento >= estado.segmentos.size())
    {
        estado.activo = false;
    }
}

void rellenoScanline(Image *canvas, Punto vertices[], int cantidad, Color colorRelleno)
{
    EstadoScanline estado;
    iniciarScanline(estado, canvas, vertices, cantidad, colorRelleno);
    while (estado.activo)
    {
        pasoScanline(estado, canvas, 1024);
    }
}