#define FIGURAS_H

#include "raylib.h"
#include "algoritmos.h" 

struct Punto {
    float x;
    float y;
};

void crearVerticesTriangulo(Punto posicion, float altura, float base)
{
    Punto v1 = { posicion.x, posicion.y };
    Punto v2 = { posicion.x - (base / 2.0f), posicion.y + altura };
    Punto v3 = { posicion.x + (base / 2.0f), posicion.y + altura };

    trazar(v1.x, v1.y, v2.x, v2.y, PINK);
    trazar(v2.x, v2.y, v3.x, v3.y, PINK);
    trazar(v3.x, v3.y, v1.x, v1.y, PINK);
}

void crearVerticesHexagono(Punto posicion, float radio)
{
    float dx = radio * 0.5f;
    float dy = radio * 0.866f;

    Punto v1 = { posicion.x + radio, posicion.y };        
    Punto v2 = { posicion.x + dx,    posicion.y + dy };  
    Punto v3 = { posicion.x - dx,    posicion.y + dy };  
    Punto v4 = { posicion.x - radio, posicion.y };        
    Punto v5 = { posicion.x - dx,    posicion.y - dy };   
    Punto v6 = { posicion.x + dx,    posicion.y - dy };   

    trazar(v1.x, v1.y, v2.x, v2.y, GREEN);
    trazar(v2.x, v2.y, v3.x, v3.y, GREEN);
    trazar(v3.x, v3.y, v4.x, v4.y, GREEN);
    trazar(v4.x, v4.y, v5.x, v5.y, GREEN);
    trazar(v5.x, v5.y, v6.x, v6.y, GREEN);
    trazar(v6.x, v6.y, v1.x, v1.y, GREEN);
}

void crearVerticesCuadrilateroConcavo(Punto posicion, float ancho, float alto)
{
    Punto v1 = { posicion.x,                   posicion.y + (alto / 2.0f) }; 
    Punto v2 = { posicion.x + (ancho / 2.0f), posicion.y - (alto / 2.0f) }; 
    Punto v3 = { posicion.x,                   posicion.y - (alto / 8.0f) }; 
    Punto v4 = { posicion.x - (ancho / 2.0f), posicion.y - (alto / 2.0f) }; 
    
    trazar(v1.x, v1.y, v2.x, v2.y, RED);
    trazar(v2.x, v2.y, v3.x, v3.y, RED);
    trazar(v3.x, v3.y, v4.x, v4.y, BLUE);
    trazar(v4.x, v4.y, v1.x, v1.y, BLUE);
}

