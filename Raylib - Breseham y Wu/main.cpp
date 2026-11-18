#include "raylib.h"
#include "figuras.h"

int main() 
{
    InitWindow(800, 450, "Figuras DrawPixel");
    SetTargetFPS(60);

    // Triángulo
    Punto posTriangulo = { 400, 150 };
    float alturaTriangulo = 150.0;
    float baseTriangulo = 120.0;

    // Hexágono 
    Punto centroPantalla = { 400, 225 };
    float radioHexagono = 100.0;

    // Cuadrilátero Cóncavo
    float anchoDardo = 160.0;
    float altoDardo = 200.0;

    int escena = 0;

    while (!WindowShouldClose()) 
    {
        if (IsKeyPressed(KEY_RIGHT)) 
        {
            escena = escena + 1;
            if (escena > 2) escena = 0;
        }

        if (IsKeyPressed(KEY_LEFT)) 
        {
            escena = escena - 1;
            if (escena < 0) escena = 2;
        }

        if (IsKeyPressed(KEY_L)) 
        {
            if (algoritmoActual == 0) 
            {
                algoritmoActual = 1; 
            } 
            else 
            {
                algoritmoActual = 0; 
            }
        }
        
        BeginDrawing();
            if (escena == 0) 
            {
                ClearBackground(BLACK);
                crearVerticesTriangulo(posTriangulo, alturaTriangulo, baseTriangulo);
            } 
            else if (escena == 1) 
            {
                ClearBackground(DARKBLUE);
                crearVerticesHexagono(centroPantalla, radioHexagono); 
            }
            else if (escena == 2) 
            {
                ClearBackground(WHITE);
                crearVerticesCuadrilateroConcavo(centroPantalla, anchoDardo, altoDardo);
            }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

