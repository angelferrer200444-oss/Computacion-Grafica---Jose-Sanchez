#define DIBUJO_H

#include "raylib.h"
#include "figuras.h"

void dibujarLinea(Image *canvas, Punto inicio, Punto fin, Color color)
{
    Punto puntos[MAX_PUNTOS_LINEA];
    int cantidad = 0;

    lineaWu(inicio.x, inicio.y, fin.x, fin.y, puntos, cantidad);

    for (int i = 0; i < cantidad; i++)
    {
        int px = (int)puntos[i].x;
        int py = (int)puntos[i].y;

        if (px >= 0 && px < canvas->width && py >= 0 && py < canvas->height)
        {
            float alpha = puntos[i].alpha;
            if (alpha <= 0.0f) continue;
            if (alpha > 1.0f) alpha = 1.0f;
            alpha *= (color.a / 255.0f);

            Color colorActual = GetImageColor(*canvas, px, py);
            if (!ColorIsEqual(colorActual, color))
            {
                float invAlpha = 1.0f - alpha;

                Color colorPixel;
                colorPixel.r = (unsigned char)(color.r * alpha + colorActual.r * invAlpha + 0.5f);
                colorPixel.g = (unsigned char)(color.g * alpha + colorActual.g * invAlpha + 0.5f);
                colorPixel.b = (unsigned char)(color.b * alpha + colorActual.b * invAlpha + 0.5f);
                colorPixel.a = (unsigned char)(color.a * alpha + colorActual.a * invAlpha + 0.5f);

                ImageDrawPixel(canvas, px, py, colorPixel);
            }
        }
    }
}

void dibujarContorno(Image *canvas, Punto vertices[], int cantidad, Color color, bool esDardo = false)
{
    if (esDardo && cantidad == 4)
    {
        dibujarLinea(canvas, vertices[0], vertices[1], RED);
        dibujarLinea(canvas, vertices[1], vertices[2], RED);
        dibujarLinea(canvas, vertices[2], vertices[3], BLUE);
        dibujarLinea(canvas, vertices[3], vertices[0], BLUE);
        return;
    }

    for (int i = 0; i < cantidad; i++)
    {
        int siguiente = (i + 1) % cantidad;
        dibujarLinea(canvas, vertices[i], vertices[siguiente], color);
    }
}

void animarRellenoFigura(Image *canvas, Punto vertices[], int cantidadVertices,
                         Color colorFondo, Color colorContorno, Color colorRelleno,
                         int semillaX, int semillaY, bool esDardo,
                         int algoritmoRelleno, int cuadrosPorFrame,
                         bool &iniciarEscena, EstadoFloodFill &estadoFlood, EstadoScanline &estadoScanline)
{
    if (iniciarEscena)
    {
        ImageClearBackground(canvas, colorFondo);
        dibujarContorno(canvas, vertices, cantidadVertices, colorContorno, esDardo);

        if (algoritmoRelleno == 0) iniciarFloodFill(estadoFlood, canvas, semillaX, semillaY, colorRelleno);
        else iniciarScanline(estadoScanline, canvas, vertices, cantidadVertices, colorRelleno);

        iniciarEscena = false;
    }

    if (algoritmoRelleno == 0)
    {
        pasoFloodFill(estadoFlood, canvas, cuadrosPorFrame);
    }
    else
    {
        bool estabaActivo = estadoScanline.activo;
        pasoScanline(estadoScanline, canvas, cuadrosPorFrame);
        if (estabaActivo && !estadoScanline.activo)
        {
            dibujarContorno(canvas, vertices, cantidadVertices, colorContorno, esDardo);
        }
    }
}

void actualizarPixelCapa(Image *capas, Image *canvas, std::vector<Punto> &pixelesEtapa, int x, int y,
                         Color colorFondo, Color colorFigura, Color colorA, Color colorB, Color colorC)
{
    if (x < 0 || x >= canvas->width || y < 0 || y >= canvas->height) return;

    Color actual = GetImageColor(*capas, x, y);
    Color nuevo = actual;
    Color colorVisible = colorFigura;

    if (ColorIsEqual(actual, colorFondo))
    {
        nuevo = colorA;
        colorVisible = colorFigura;
    }
    else if (ColorIsEqual(actual, colorA))
    {
        nuevo = colorB;
        colorVisible = colorB;
    }
    else if (ColorIsEqual(actual, colorB))
    {
        nuevo = colorC;
        colorVisible = colorC;
    }

    if (!ColorIsEqual(nuevo, actual))
    {
        ImageDrawPixel(capas, x, y, nuevo);
        ImageDrawPixel(canvas, x, y, colorVisible);

        if (ColorIsEqual(nuevo, colorA))
        {
            pixelesEtapa.push_back({ (float)x, (float)y, 1.0f });
        }
    }
}

void obtenerFiguraEscena4(int etapa, Punto centroPantalla,
                          float alturaTriangulo, float baseTriangulo,
                          float radioHexagono, float anchoDardo, float altoDardo,
                          Punto vertices[6], int &cantidad, int &semillaX, int &semillaY,
                          Color &colorContorno, bool &esDardo)
{
    esDardo = false;

    if (etapa == 0)
    {
        Punto pos = { centroPantalla.x - 50.0f, centroPantalla.y - 90.0f, 1.0f };
        crearVerticesTriangulo(pos, alturaTriangulo, baseTriangulo, vertices);
        cantidad = 3;
        semillaX = (int)pos.x;
        semillaY = (int)(pos.y + alturaTriangulo * 0.5f);
        colorContorno = PINK;
    }
    else if (etapa == 1)
    {
        Punto pos = { centroPantalla.x + 55.0f, centroPantalla.y - 20.0f, 1.0f };
        crearVerticesHexagono(pos, radioHexagono, vertices);
        cantidad = 6;
        semillaX = (int)pos.x;
        semillaY = (int)pos.y;
        colorContorno = GREEN;
    }
    else
    {
        Punto pos = { centroPantalla.x, centroPantalla.y + 65.0f, 1.0f };
        crearVerticesCuadrilateroConcavo(pos, anchoDardo, altoDardo, vertices);
        cantidad = 4;
        semillaX = (int)pos.x;
        semillaY = (int)(pos.y + altoDardo * 0.1f);
        colorContorno = RED;
        esDardo = true;
    }
}

void dibujarContornosEscena4(Image *canvas, Punto centroPantalla,
                             float alturaTriangulo, float baseTriangulo,
                             float radioHexagono, float anchoDardo, float altoDardo)
{
    for (int etapa = 0; etapa < 3; etapa++)
    {
        Punto vertices[6];
        int cantidad = 0, sx = 0, sy = 0;
        Color color = WHITE;
        bool esDardo = false;

        obtenerFiguraEscena4(etapa, centroPantalla, alturaTriangulo, baseTriangulo,
                             radioHexagono, anchoDardo, altoDardo,
                             vertices, cantidad, sx, sy, color, esDardo);
        dibujarContorno(canvas, vertices, cantidad, color, esDardo);
    }
}

void iniciarEtapaEscena4(int etapa, Image *mascara, Punto centroPantalla,
                         float alturaTriangulo, float baseTriangulo,
                         float radioHexagono, float anchoDardo, float altoDardo,
                         int algoritmoRelleno, EstadoFloodFill &estadoFlood,
                         EstadoScanline &estadoScanline, size_t &indiceFloodEscena4)
{
    ImageClearBackground(mascara, BLANK);

    Punto vertices[6];
    int cantidad = 0, semillaX = 0, semillaY = 0;
    Color color = WHITE;
    bool esDardo = false;

    obtenerFiguraEscena4(etapa, centroPantalla, alturaTriangulo, baseTriangulo,
                         radioHexagono, anchoDardo, altoDardo,
                         vertices, cantidad, semillaX, semillaY, color, esDardo);

    if (algoritmoRelleno == 0)
    {
        dibujarContorno(mascara, vertices, cantidad, WHITE);
        iniciarFloodFill(estadoFlood, mascara, semillaX, semillaY, WHITE);
        indiceFloodEscena4 = 0;
    }
    else
    {
        iniciarScanline(estadoScanline, mascara, vertices, cantidad, WHITE);
    }
}

bool avanzarEtapaEscena4(int algoritmoRelleno, int cuadrosEtapa,
                         Image *mascara, Image *capas, Image *canvas,
                         std::vector<Punto> &pixelesEtapa,
                         EstadoFloodFill &estadoFlood, EstadoScanline &estadoScanline,
                         size_t &indiceFloodEscena4,
                         Color colorFondo, Color colorFigura,
                         Color colorA, Color colorB, Color colorC)
{
    if (algoritmoRelleno == 0)
    {
        pasoFloodFill(estadoFlood, mascara, cuadrosEtapa);
        while (indiceFloodEscena4 < estadoFlood.cola.size())
        {
            int px = (int)estadoFlood.cola[indiceFloodEscena4].x;
            int py = (int)estadoFlood.cola[indiceFloodEscena4].y;
            actualizarPixelCapa(capas, canvas, pixelesEtapa, px, py,
                                colorFondo, colorFigura, colorA, colorB, colorC);
            indiceFloodEscena4++;
        }
        return !estadoFlood.activo;
    }

    if (estadoScanline.activo && canvas != nullptr && canvas->data != nullptr && cuadrosEtapa > 0)
    {
        int pintados = 0;
        while (estadoScanline.indiceSegmento < estadoScanline.segmentos.size() && pintados < cuadrosEtapa)
        {
            SegmentoScanline &seg = estadoScanline.segmentos[estadoScanline.indiceSegmento];
            actualizarPixelCapa(capas, canvas, pixelesEtapa, estadoScanline.xActual, seg.y,
                                colorFondo, colorFigura, colorA, colorB, colorC);
            pintados++;
            estadoScanline.xActual++;

            if (estadoScanline.xActual > seg.xFin)
            {
                estadoScanline.indiceSegmento++;
                if (estadoScanline.indiceSegmento < estadoScanline.segmentos.size())
                {
                    estadoScanline.xActual = estadoScanline.segmentos[estadoScanline.indiceSegmento].xInicio;
                }
            }
        }

        if (estadoScanline.indiceSegmento >= estadoScanline.segmentos.size())
        {
            estadoScanline.activo = false;
        }
    }

    return !estadoScanline.activo;
}
