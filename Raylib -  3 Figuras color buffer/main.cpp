#include "raylib.h"
#include "figuras.h"
#include <stdlib.h>
#include <time.h>

void dibujarPixelImagen(Image *imagen, int x, int y, Color color, float alpha)
{
    if (x >= 0 && x < imagen->width && y >= 0 && y < imagen->height)
    {
        Color colorActual = GetImageColor(*imagen, x, y);

        float alphaFuente = alpha * (color.a / 255.0f);
        float alphaDestino = colorActual.a / 255.0f;
        float alphaResultado = alphaFuente + alphaDestino * (1.0f - alphaFuente);

        if (alphaResultado <= 0.0f)
            return;

        Color colorMezclado;

        colorMezclado.r = (unsigned char)((color.r * alphaFuente + colorActual.r * alphaDestino * (1.0f - alphaFuente)) / alphaResultado + 0.5f);
        colorMezclado.g = (unsigned char)((color.g * alphaFuente + colorActual.g * alphaDestino * (1.0f - alphaFuente)) / alphaResultado + 0.5f);
        colorMezclado.b = (unsigned char)((color.b * alphaFuente + colorActual.b * alphaDestino * (1.0f - alphaFuente)) / alphaResultado + 0.5f);
        colorMezclado.a = (unsigned char)(alphaResultado * 255.0f + 0.5f);

        if (!ColorIsEqual(colorActual, colorMezclado))
            ImageDrawPixel(imagen, x, y, colorMezclado);
    }
}

void dibujarLinea(Image *imagen, Punto inicio, Punto fin, Color color)
{
    Punto puntos[MAX_PUNTOS_LINEA];
    int cantidad = 0;

    lineaWu(inicio.x, inicio.y, fin.x, fin.y, puntos, cantidad);

    for (int i = 0; i < cantidad; i++)
        dibujarPixelImagen(imagen, (int)puntos[i].x, (int)puntos[i].y, color, puntos[i].alpha);
}

void dibujarContorno(Image *imagen, Punto vertices[], int cantidad, Color color)
{
    for (int i = 0; i < cantidad; i++)
    {
        int siguiente = (i + 1) % cantidad;
        dibujarLinea(imagen, vertices[i], vertices[siguiente], color);
    }
}

void combinarImagen(Image *destino, Image origen, int dx, int dy)
{
    for (int y = 0; y < origen.height; y++)
    {
        for (int x = 0; x < origen.width; x++)
        {
            Color pixel = GetImageColor(origen, x, y);

            if (pixel.a > 0)
                dibujarPixelImagen(destino, x + dx, y + dy, pixel, 1.0f);
        }
    }
}

void iniciarRelleno(Image *figura, Image original, int x, int y, int algoritmo, Punto puntos[], int &cantidad, int &indice, bool &animando)
{
    UnloadImage(*figura);
    *figura = ImageCopy(original);

    cantidad = 0;
    indice = 0;

    Image imagenTrabajo = GenImageColor(original.width, original.height, BLACK);
    combinarImagen(&imagenTrabajo, original, 0, 0);

    if (algoritmo == 0)
        floodFill(&imagenTrabajo, x, y, puntos, cantidad);
    else
        scanlineFill(&imagenTrabajo, x, y, puntos, cantidad);

    UnloadImage(imagenTrabajo);

    animando = (cantidad > 0);
}

void finalizarFrentePendiente(Image *triangulo, Image *hexagono, Image *cuadrilatero, Punto puntos[], int inicio, int fin, int escena)
{
    Image *figura;
    Color colorDefinitivo;

    if (escena == 0)
    {
        figura = triangulo;
        colorDefinitivo = VIOLET;
    }
    else if (escena == 1)
    {
        figura = hexagono;
        colorDefinitivo = PINK;
    }
    else
    {
        figura = cuadrilatero;
        colorDefinitivo = SKYBLUE;
    }

    for (int i = inicio; i < fin; i++)
        ImageDrawPixel(figura, (int)puntos[i].x, (int)puntos[i].y, colorDefinitivo);
}

void crearMascara(Image original, int x, int y, unsigned char mascara[], Punto puntos[], int &cantidad)
{
    for (int i = 0; i < original.width * original.height; i++)
        mascara[i] = 0;

    cantidad = 0;

    Image imagenTrabajo = GenImageColor(original.width, original.height, BLACK);
    combinarImagen(&imagenTrabajo, original, 0, 0);

    floodFill(&imagenTrabajo, x, y, puntos, cantidad);

    for (int i = 0; i < cantidad; i++)
    {
        int px = (int)puntos[i].x;
        int py = (int)puntos[i].y;
        if (px >= 0 && px < original.width && py >= 0 && py < original.height)
            mascara[py * original.width + px] = 1;
    }

    UnloadImage(imagenTrabajo);
}

void iniciarRellenoUnion(Image *imagenUnion, int cantidadUnion, int &indice, bool &animando, bool &frentePendiente)
{
    Color transparente = { 0, 0, 0, 0 };

    ImageClearBackground(imagenUnion, transparente);

    indice = 0;
    animando = (cantidadUnion > 0);
    frentePendiente = false;
}

void finalizarFrenteUnion(Image *imagenUnion, Punto puntosUnion[], Color coloresDefinitivos[], int inicio, int fin)
{
    for (int i = inicio; i < fin; i++)
        ImageDrawPixel(imagenUnion, (int)puntosUnion[i].x, (int)puntosUnion[i].y, coloresDefinitivos[i]);
}

int main()
{
    srand((unsigned int)time(NULL));

    const int ANCHO = 800;
    const int ALTO = 450;

    InitWindow(ANCHO, ALTO, "Figuras");
    SetTargetFPS(60);

    Color transparente = { 0, 0, 0, 0 };

    Image imagenTriangulo = GenImageColor(ANCHO, ALTO, transparente);
    Image imagenHexagono = GenImageColor(ANCHO, ALTO, transparente);
    Image imagenCuadrilatero = GenImageColor(ANCHO, ALTO, transparente);

    Image imagenUnion = GenImageColor(ANCHO, ALTO, transparente);

    Image colorBuffer = GenImageColor(ANCHO, ALTO, BLACK);
    Texture2D textura = LoadTextureFromImage(colorBuffer);

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

    // Crear triángulo
    Punto verticesTriangulo[3];
    crearVerticesTriangulo(posTriangulo, alturaTriangulo, baseTriangulo, verticesTriangulo);
    dibujarContorno(&imagenTriangulo, verticesTriangulo, 3, PINK);

    Image imagenTrianguloOriginal = ImageCopy(imagenTriangulo);

    // Crear hexágono
    Punto verticesHexagono[6];
    crearVerticesHexagono(centroPantalla, radioHexagono, verticesHexagono);
    dibujarContorno(&imagenHexagono, verticesHexagono, 6, GREEN);

    Image imagenHexagonoOriginal = ImageCopy(imagenHexagono);

    // Crear cuadrilátero
    Punto verticesCuadrilatero[4];
    crearVerticesCuadrilateroConcavo(centroPantalla, anchoDardo, altoDardo, verticesCuadrilatero);

    dibujarLinea(&imagenCuadrilatero, verticesCuadrilatero[0], verticesCuadrilatero[1], RED);
    dibujarLinea(&imagenCuadrilatero, verticesCuadrilatero[1], verticesCuadrilatero[2], RED);
    dibujarLinea(&imagenCuadrilatero, verticesCuadrilatero[2], verticesCuadrilatero[3], BLUE);
    dibujarLinea(&imagenCuadrilatero, verticesCuadrilatero[3], verticesCuadrilatero[0], BLUE);

    Image imagenCuadrilateroOriginal = ImageCopy(imagenCuadrilatero);

    static unsigned char mascaraTriangulo[800 * 450];
    static unsigned char mascaraHexagono[800 * 450];
    static unsigned char mascaraCuadrilatero[800 * 450];

    static Punto puntosMascara[MAX_COLA];
    int cantidadMascara = 0;

    crearMascara(imagenTrianguloOriginal, 400, 200, mascaraTriangulo, puntosMascara, cantidadMascara);
    crearMascara(imagenHexagonoOriginal, 400, 225, mascaraHexagono, puntosMascara, cantidadMascara);
    crearMascara(imagenCuadrilateroOriginal, 400, 225, mascaraCuadrilatero, puntosMascara, cantidadMascara);

    int escena = 0;
    int algoritmosRelleno[3] = { rand() % 2, rand() % 2, rand() % 2 };
    const int PIXELES_TRIANGULO = 50;
    const int PIXELES_HEXAGONO = 100;
    const int PIXELES_CUADRILATERO = 50;

    Color pintadoTriangulo = YELLOW;
    Color pintadoHexagono = ORANGE;
    Color pintadoCuadrilatero = LIME;

    static Punto puntosRelleno[MAX_COLA];

    int cantidadRelleno = 0;
    int indiceRelleno = 0;

    int inicioFrente = 0;
    int finFrente = 0;
    int escenaFrente = 0;
    bool frentePendiente = false;

    bool animandoRelleno = false;

    iniciarRelleno(&imagenTriangulo, imagenTrianguloOriginal, 400, 200, algoritmosRelleno[0], puntosRelleno, cantidadRelleno, indiceRelleno, animandoRelleno);

    Color colorA = ORANGE;
    Color colorB = BLUE;
    Color colorC = PURPLE;
    Color colorTemporalUnion = YELLOW;

    static Punto puntosUnion[MAX_COLA];
    static Color coloresDefinitivos[MAX_COLA];
    int cantidadUnion = 0;

    for (int y = 0; y < ALTO; y++)
    {
        for (int x = 0; x < ANCHO; x++)
        {
            int coincidencias = 0;

            int tx = x + 35;
            int ty = y + 15;

            if (tx >= 0 && tx < ANCHO && ty >= 0 && ty < ALTO && mascaraTriangulo[ty * ANCHO + tx])
                coincidencias++;

            int hx = x - 35;
            int hy = y + 10;

            if (hx >= 0 && hx < ANCHO && hy >= 0 && hy < ALTO && mascaraHexagono[hy * ANCHO + hx])
                coincidencias++;

            int qx = x;
            int qy = y - 25;

            if (qx >= 0 && qx < ANCHO && qy >= 0 && qy < ALTO && mascaraCuadrilatero[qy * ANCHO + qx])
                coincidencias++;

            if (coincidencias > 0 && cantidadUnion < MAX_COLA)
            {
                puntosUnion[cantidadUnion].x = (float)x;
                puntosUnion[cantidadUnion].y = (float)y;
                puntosUnion[cantidadUnion].alpha = 1.0f;

                if (coincidencias == 1)
                    coloresDefinitivos[cantidadUnion] = colorA;
                else if (coincidencias == 2)
                    coloresDefinitivos[cantidadUnion] = colorB;
                else
                    coloresDefinitivos[cantidadUnion] = colorC;

                cantidadUnion++;
            }
        }
    }

    const int PIXELES_UNION = 250;

    int indiceUnion = 0;
    int inicioFrenteUnion = 0;
    int finFrenteUnion = 0;

    bool animandoUnion = false;
    bool frentePendienteUnion = false;

    while (!WindowShouldClose())
    {
        bool cambioEscena = false;

        if (IsKeyPressed(KEY_RIGHT))
        {
            escena++;

            if (escena > 3)
                escena = 0;

            cambioEscena = true;
        }

        if (IsKeyPressed(KEY_LEFT))
        {
            escena--;

            if (escena < 0)
                escena = 3;

            cambioEscena = true;
        }

        if (cambioEscena)
        {
            if (frentePendiente)
            {
                finalizarFrentePendiente(&imagenTriangulo, &imagenHexagono, &imagenCuadrilatero, puntosRelleno, inicioFrente, finFrente, escenaFrente);
                frentePendiente = false;
            }

            if (frentePendienteUnion)
            {
                finalizarFrenteUnion(&imagenUnion, puntosUnion, coloresDefinitivos, inicioFrenteUnion, finFrenteUnion);
                frentePendienteUnion = false;
            }

            if (escena == 0)
            {
                algoritmosRelleno[0] = rand() % 2;
                iniciarRelleno(&imagenTriangulo, imagenTrianguloOriginal, 400, 200, algoritmosRelleno[0], puntosRelleno, cantidadRelleno, indiceRelleno, animandoRelleno);
            }
            else if (escena == 1)
            {
                algoritmosRelleno[1] = rand() % 2;
                iniciarRelleno(&imagenHexagono, imagenHexagonoOriginal, 400, 225, algoritmosRelleno[1], puntosRelleno, cantidadRelleno, indiceRelleno, animandoRelleno);
            }
            else if (escena == 2)
            {
                algoritmosRelleno[2] = rand() % 2;
                iniciarRelleno(&imagenCuadrilatero, imagenCuadrilateroOriginal, 400, 225, algoritmosRelleno[2], puntosRelleno, cantidadRelleno, indiceRelleno, animandoRelleno);
            }
            else if (escena == 3)
            {
                animandoRelleno = false;

                iniciarRellenoUnion(&imagenUnion, cantidadUnion, indiceUnion, animandoUnion, frentePendienteUnion);
            }
        }

        if (IsKeyPressed(KEY_R) && escena < 3)
        {
            if (frentePendiente)
            {
                finalizarFrentePendiente(&imagenTriangulo, &imagenHexagono, &imagenCuadrilatero, puntosRelleno, inicioFrente, finFrente, escenaFrente);
                frentePendiente = false;
            }

            algoritmosRelleno[escena]++;

            if (algoritmosRelleno[escena] > 1)
                algoritmosRelleno[escena] = 0;
            if (escena == 0)
                iniciarRelleno(&imagenTriangulo, imagenTrianguloOriginal, 400, 200, algoritmosRelleno[0], puntosRelleno, cantidadRelleno, indiceRelleno, animandoRelleno);
            else if (escena == 1)
                iniciarRelleno(&imagenHexagono, imagenHexagonoOriginal, 400, 225, algoritmosRelleno[1], puntosRelleno, cantidadRelleno, indiceRelleno, animandoRelleno);
            else if (escena == 2)
                iniciarRelleno(&imagenCuadrilatero, imagenCuadrilateroOriginal, 400, 225, algoritmosRelleno[2], puntosRelleno, cantidadRelleno, indiceRelleno, animandoRelleno);
        }

        if (animandoRelleno || frentePendiente)
        {
            if (frentePendiente)
            {
                finalizarFrentePendiente(&imagenTriangulo, &imagenHexagono, &imagenCuadrilatero, puntosRelleno, inicioFrente, finFrente, escenaFrente);
                frentePendiente = false;
            }

            if (animandoRelleno)
            {
                int limiteFotogramas = PIXELES_TRIANGULO;

                if (escena == 1)
                    limiteFotogramas = PIXELES_HEXAGONO;
                else if (escena == 2)
                    limiteFotogramas = PIXELES_CUADRILATERO;

                inicioFrente = indiceRelleno;

                Color colorTemporal;

                if (escena == 0)
                    colorTemporal = pintadoTriangulo;
                else if (escena == 1)
                    colorTemporal = pintadoHexagono;
                else
                    colorTemporal = pintadoCuadrilatero;

                for (int i = 0; i < limiteFotogramas && indiceRelleno < cantidadRelleno; i++)
                {
                    if (escena == 0)
                        ImageDrawPixel(&imagenTriangulo, (int)puntosRelleno[indiceRelleno].x, (int)puntosRelleno[indiceRelleno].y, colorTemporal);
                    else if (escena == 1)
                        ImageDrawPixel(&imagenHexagono, (int)puntosRelleno[indiceRelleno].x, (int)puntosRelleno[indiceRelleno].y, colorTemporal);
                    else if (escena == 2)
                        ImageDrawPixel(&imagenCuadrilatero, (int)puntosRelleno[indiceRelleno].x, (int)puntosRelleno[indiceRelleno].y, colorTemporal);

                    indiceRelleno++;
                }

                finFrente = indiceRelleno;
                escenaFrente = escena;
                frentePendiente = (finFrente > inicioFrente);

                if (indiceRelleno >= cantidadRelleno)
                    animandoRelleno = false;
            }
        }

        if (escena == 3)
        {
            if (frentePendienteUnion)
            {
                finalizarFrenteUnion(&imagenUnion, puntosUnion, coloresDefinitivos, inicioFrenteUnion, finFrenteUnion);
                frentePendienteUnion = false;
            }

            if (animandoUnion)
            {
                inicioFrenteUnion = indiceUnion;

                for (int i = 0; i < PIXELES_UNION && indiceUnion < cantidadUnion; i++)
                {
                    ImageDrawPixel(&imagenUnion, (int)puntosUnion[indiceUnion].x, (int)puntosUnion[indiceUnion].y, colorTemporalUnion);
                    indiceUnion++;
                }

                finFrenteUnion = indiceUnion;
                frentePendienteUnion = (finFrenteUnion > inicioFrenteUnion);

                if (indiceUnion >= cantidadUnion)
                    animandoUnion = false;
            }
        }

        if (escena == 0)
        {
            ImageClearBackground(&colorBuffer, BLACK);
            combinarImagen(&colorBuffer, imagenTriangulo, 0, 0);
        }
        else if (escena == 1)
        {
            ImageClearBackground(&colorBuffer, DARKBLUE);
            combinarImagen(&colorBuffer, imagenHexagono, 0, 0);
        }
        else if (escena == 2)
        {
            ImageClearBackground(&colorBuffer, WHITE);
            combinarImagen(&colorBuffer, imagenCuadrilatero, 0, 0);
        }
        else if (escena == 3)
        {
            ImageClearBackground(&colorBuffer, BROWN);

            combinarImagen(&colorBuffer, imagenUnion, 0, 0);

            combinarImagen(&colorBuffer, imagenTrianguloOriginal, -35, -15);
            combinarImagen(&colorBuffer, imagenHexagonoOriginal, 35, -10);
            combinarImagen(&colorBuffer, imagenCuadrilateroOriginal, 0, 25);
        }

        UpdateTexture(textura, colorBuffer.data);

        BeginDrawing();
        ClearBackground(BLACK);
        DrawTexture(textura, 0, 0, WHITE);
        EndDrawing();
    }

    UnloadImage(imagenTriangulo);
    UnloadImage(imagenTrianguloOriginal);

    UnloadImage(imagenHexagono);
    UnloadImage(imagenHexagonoOriginal);

    UnloadImage(imagenCuadrilatero);
    UnloadImage(imagenCuadrilateroOriginal);

    UnloadImage(imagenUnion);

    UnloadImage(colorBuffer);
    UnloadTexture(textura);

    CloseWindow();

    return 0;
}
