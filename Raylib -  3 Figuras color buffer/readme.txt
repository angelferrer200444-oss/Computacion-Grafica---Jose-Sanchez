Buenos días, prof. Sánchez. Para compilar y ejecutar el programa:

--- WINDOWS ---
Compilar:
g++ main.cpp -o main.exe -lraylib -lopengl32 -lgdi32 -lwinmm

Ejecutar:
.\main.exe
(o ./main.exe en terminales tipo bash / MSYS2)


--- LINUX ---
Compilar:
g++ main.cpp -o main -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

Ejecutar:
./main