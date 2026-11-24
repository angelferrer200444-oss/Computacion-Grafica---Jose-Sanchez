#include "raylib.h"
#include "figuras.h"

void dibujarLinea (Punto inicio, Punto fin, Color color)
{
    Punto puntos[MAX_PUNTOS_LINEA];
    int cantidad = 0;

    trazar(inicio.x, inicio.y, fin.x, fin.y, puntos, cantidad);

    for (int i = 0; i < cantidad; i++)
    {
        Color colorPixel = ColorAlpha(color, puntos[i].alpha);

        DrawPixel ((int)puntos[i].x, (int)puntos[i].y, colorPixel);
    }
    
}

void dibujarContorno(Punto vertices[], int cantidad, Color color)
{
    for (int i = 0; i < cantidad; i++)
    {
        int siguiente = (i + 1) % cantidad;

        dibujarLinea(vertices[i], vertices[siguiente], color);
    }
}

int main()
{
    InitWindow(800, 450, "Figuras DrawPixel");
    SetTargetFPS(60);

    // Triángulo
    Punto posTriangulo = { 400, 150, 1.0f };
    float alturaTriangulo = 150.0f;
    float baseTriangulo = 120.0f;

    // Hexágono
    Punto centroPantalla = { 400, 225, 1.0f };
    float radioHexagono = 100.0f;

    // Cuadrilátero cóncavo
    float anchoDardo = 160.0f;
    float altoDardo = 200.0f;

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
            Punto vertices[3];

            crearVerticesTriangulo( posTriangulo, alturaTriangulo, baseTriangulo, vertices);
            dibujarContorno(vertices, 3, PINK);

        }

        else if (escena == 1)
        {
            ClearBackground(DARKBLUE);
            Punto vertices[6];

            crearVerticesHexagono (centroPantalla, radioHexagono, vertices);
            dibujarContorno(vertices, 6, GREEN);
        }

        else if (escena == 2)

        {
            ClearBackground(WHITE);
            Punto vertices[4];

            crearVerticesCuadrilateroConcavo (centroPantalla, anchoDardo, altoDardo, vertices);

            dibujarLinea(vertices[0], vertices[1], RED);
            dibujarLinea(vertices[1], vertices[2], RED);
            dibujarLinea(vertices[2], vertices[3], BLUE);
            dibujarLinea(vertices[3], vertices[0], BLUE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}

