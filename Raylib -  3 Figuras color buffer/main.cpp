#include "raylib.h"
#include "dibujo.h"

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Figuras DrawPixel");
    SetTargetFPS(60);

    Image canvas = GenImageColor(screenWidth, screenHeight, BLACK);
    Image capas = GenImageColor(screenWidth, screenHeight, BLACK);
    Image mascara = GenImageColor(screenWidth, screenHeight, BLANK);
    Texture2D screenTex = LoadTextureFromImage(canvas);

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

    const int cuadrosPorFrame = 45;
    const int cuadrosHexagono = 135;

    int escena = 0;
    int algoritmoRelleno = GetRandomValue(0, 1);
    bool iniciarEscena = true;

    EstadoFloodFill estadoFlood;
    EstadoScanline estadoScanline;
    std::vector<Punto> pixelesEtapaEscena4;
    int etapaEscena4 = 0;
    size_t indiceFloodEscena4 = 0;

    while (!WindowShouldClose())
    {
        if (IsKeyPressed(KEY_RIGHT))
        {
            escena = escena + 1;
            if (escena > 3) escena = 0;
            iniciarEscena = true;
        }

        if (IsKeyPressed(KEY_LEFT))
        {
            escena = escena - 1;
            if (escena < 0) escena = 3;
            iniciarEscena = true;
        }

        if (IsKeyPressed(KEY_R))
        {
            algoritmoRelleno = GetRandomValue(0, 1);
            iniciarEscena = true;
        }

        if (escena == 0)
        {
            Punto vertices[3];
            crearVerticesTriangulo(posTriangulo, alturaTriangulo, baseTriangulo, vertices);
            animarRellenoFigura(&canvas, vertices, 3, BLACK, PINK, PURPLE,
                                (int)posTriangulo.x, (int)(posTriangulo.y + (alturaTriangulo * 0.5f)),
                                false, algoritmoRelleno, cuadrosPorFrame,
                                iniciarEscena, estadoFlood, estadoScanline);
        }

        else if (escena == 1)
        {
            Punto vertices[6];
            crearVerticesHexagono(centroPantalla, radioHexagono, vertices);
            animarRellenoFigura(&canvas, vertices, 6, DARKBLUE, GREEN, GREEN,
                                (int)centroPantalla.x, (int)centroPantalla.y,
                                false, algoritmoRelleno, cuadrosHexagono,
                                iniciarEscena, estadoFlood, estadoScanline);
        }

        else if (escena == 2)
        {
            Punto vertices[4];
            crearVerticesCuadrilateroConcavo(centroPantalla, anchoDardo, altoDardo, vertices);
            animarRellenoFigura(&canvas, vertices, 4, WHITE, RED, GOLD,
                                (int)centroPantalla.x, (int)(centroPantalla.y + (altoDardo * 0.1f)),
                                true, algoritmoRelleno, cuadrosPorFrame,
                                iniciarEscena, estadoFlood, estadoScanline);
        }

        else if (escena == 3)
        {
            Color colorFondo = BROWN;
            Color colorA = SKYBLUE; // 1 figura (azul definitivo)
            Color colorB = ORANGE;  // 2 figuras (colisión)
            Color colorC = MAGENTA; // 3 figuras (colisión triple)
            Color coloresFiguras[3] = { PURPLE, GREEN, GOLD };

            if (iniciarEscena)
            {
                ImageClearBackground(&capas, colorFondo);
                ImageClearBackground(&canvas, colorFondo);
                dibujarContornosEscena4(&canvas, centroPantalla,
                                        alturaTriangulo, baseTriangulo,
                                        radioHexagono, anchoDardo, altoDardo);

                pixelesEtapaEscena4.clear();
                etapaEscena4 = 0;
                iniciarEtapaEscena4(etapaEscena4, &mascara, centroPantalla,
                                    alturaTriangulo, baseTriangulo, radioHexagono, anchoDardo, altoDardo,
                                    algoritmoRelleno, estadoFlood, estadoScanline, indiceFloodEscena4);
                iniciarEscena = false;
            }

            if (etapaEscena4 < 3)
            {
                Color colorFigura = coloresFiguras[etapaEscena4];
                int cuadrosEtapa = (etapaEscena4 == 1) ? cuadrosHexagono : cuadrosPorFrame;
                bool etapaTerminada = avanzarEtapaEscena4(algoritmoRelleno, cuadrosEtapa,
                                                          &mascara, &capas, &canvas, pixelesEtapaEscena4,
                                                          estadoFlood, estadoScanline, indiceFloodEscena4,
                                                          colorFondo, colorFigura, colorA, colorB, colorC);

                if (etapaTerminada)
                {
                    for (size_t i = 0; i < pixelesEtapaEscena4.size(); i++)
                    {
                        ImageDrawPixel(&canvas, (int)pixelesEtapaEscena4[i].x, (int)pixelesEtapaEscena4[i].y, colorA);
                    }
                    pixelesEtapaEscena4.clear();

                    etapaEscena4++;
                    if (etapaEscena4 < 3)
                    {
                        iniciarEtapaEscena4(etapaEscena4, &mascara, centroPantalla,
                                            alturaTriangulo, baseTriangulo, radioHexagono, anchoDardo, altoDardo,
                                            algoritmoRelleno, estadoFlood, estadoScanline, indiceFloodEscena4);
                    }
                    else
                    {
                        dibujarContornosEscena4(&canvas, centroPantalla,
                                                alturaTriangulo, baseTriangulo,
                                                radioHexagono, anchoDardo, altoDardo);
                    }
                }
            }
        }

        UpdateTexture(screenTex, canvas.data);

        BeginDrawing();

        ClearBackground(BLACK);
        DrawTexture(screenTex, 0, 0, WHITE);

        EndDrawing();
    }

    UnloadTexture(screenTex);
    UnloadImage(mascara);
    UnloadImage(capas);
    UnloadImage(canvas);
    CloseWindow();

    return 0;
}

