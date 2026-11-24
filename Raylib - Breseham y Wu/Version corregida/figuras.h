#define FIGURAS_H

#include "algoritmos.h"

void crearVerticesTriangulo (Punto posicion, float altura, float base, Punto vertices[3])
{
    vertices[0].x = posicion.x;
    vertices[0].y = posicion.y;
    vertices[0].alpha = 1.0f;

    vertices[1].x = posicion.x - (base / 2.0f);
    vertices[1].y = posicion.y + altura;
    vertices[1].alpha = 1.0f;

    vertices[2].x = posicion.x + (base / 2.0f);
    vertices[2].y = posicion.y + altura;
    vertices[2].alpha = 1.0f;
}

void crearVerticesHexagono (Punto posicion, float radio, Punto vertices[6])
{
    float dx = radio * 0.5f;
    float dy = radio * 0.866f;

    vertices[0].x = posicion.x + radio;
    vertices[0].y = posicion.y;

    vertices[1].x = posicion.x + dx;
    vertices[1].y = posicion.y + dy;

    vertices[2].x = posicion.x - dx;
    vertices[2].y = posicion.y + dy;

    vertices[3].x = posicion.x - radio;
    vertices[3].y = posicion.y;

    vertices[4].x = posicion.x - dx;
    vertices[4].y = posicion.y - dy;

    vertices[5].x = posicion.x + dx;
    vertices[5].y = posicion.y - dy;

    for (int i = 0; i < 6; i++)
    {
        vertices[i].alpha = 1.0f;
    }
}

void crearVerticesCuadrilateroConcavo (Punto posicion, float ancho, float alto, Punto vertices[4])
{
    vertices[0].x = posicion.x;
    vertices[0].y = posicion.y + (alto / 2.0f);

    vertices[1].x = posicion.x + (ancho / 2.0f);
    vertices[1].y = posicion.y - (alto / 2.0f);

    vertices[2].x = posicion.x;
    vertices[2].y = posicion.y - (alto / 8.0f);

    vertices[3].x = posicion.x - (ancho / 2.0f);
    vertices[3].y = posicion.y - (alto / 2.0f);

    for (int i = 0; i < 4; i++)
    {
        vertices[i].alpha = 1.0f;
    }
}


