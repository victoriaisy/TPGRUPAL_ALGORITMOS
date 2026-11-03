#include <iostream>
#include <stdio.h>
using namespace std;

struct Orden
{
    unsigned int espera = 0;
    bool soltarGranada1 = false;
    bool soltarGranada2 = false;
    bool ataqueKamikaze = false;
    bool aterrizaje = false;
    bool despegue = false;
    int siguientex = 0;
    int siguientey = 0;
};

struct OrdenArchivo
{
    int x = 0;
    int y = 0;
    unsigned int espera = 0;
    bool soltarGranada1 = false;
    bool soltarGranada2 = false;
    bool ataqueKamikaze = false;
    bool aterrizaje = false;
    bool despegue = false;
    int siguientex = 0;
    int siguientey = 0;
};

void mostrarOrden(Orden orden)
{
    cout << "Espera: " << orden.espera << endl;
    cout << "Granada 1: " << orden.soltarGranada1 << endl;
    cout << "Granada 2: " << orden.soltarGranada2 << endl;
    cout << "Kamikaze: " << orden.ataqueKamikaze << endl;
    cout << "Aterrizaje: " << orden.aterrizaje << endl;
    cout << "Despegue: " << orden.despegue << endl;
    cout << "Siguiente X: " << orden.siguientex << endl;
    cout << "Siguiente Y: " << orden.siguientey << endl;
}


void ingresarOrden(Orden &orden)
{
    cout << "Ingrese espera: ";
    cin >> orden.espera;

    cout << "Granada 1 (0 o 1): ";
    cin >> orden.soltarGranada1;

    cout << "Granada 2 (0 o 1): ";
    cin >> orden.soltarGranada2;

    cout << "Kamikaze (0 o 1): ";
    cin >> orden.ataqueKamikaze;

    cout << "Aterrizaje (0 o 1): ";
    cin >> orden.aterrizaje;

    cout << "Despegue (0 o 1): ";
    cin >> orden.despegue;

    cout << "Siguiente X: ";
    cin >> orden.siguientex;

    cout << "Siguiente Y: ";
    cin >> orden.siguientey;
}

// PUNTO 5: corrige una orden ya cargada, directamente en la matriz

void corregirRegistroMemoria(Orden tabla[][200], bool existe[][200])
{
    int x, y;

    cout << "Ingrese coordenada X: ";
    cin >> x;

    cout << "Ingrese coordenada Y: ";
    cin >> y;

    if (x < 0 || x >= 200 || y < 0 || y >= 200)
    {
        cout << "Coordenadas fuera de rango." << endl;
        return;
    }

    if (existe[x][y] == false)
    {
        cout << "No hay una orden en esa posicion." << endl;
        return;
    }

    cout << "Orden actual:" << endl;
    mostrarOrden(tabla[x][y]);

    cout << "Ingrese los nuevos datos:" << endl;
    ingresarOrden(tabla[x][y]);

    cout << "Orden modificada en memoria." << endl;
}


// PUNTO 6: recorre la matriz y guarda solo las celdas con ordenes
void guardarMemoriaArchivo(Orden tabla[][200], bool existe[][200])
{
    FILE *f;
    OrdenArchivo reg;
    char nombreArchivo[260];

    cout << "Ingrese el nombre del nuevo archivo: ";
    cin >> nombreArchivo;

    f = fopen(nombreArchivo, "wb");

    if (f == NULL)
    {
        cout << "No se pudo crear el archivo." << endl;
        return;
    }

    for (int x = 0; x < 200; x++)
    {
        for (int y = 0; y < 200; y++)
        {
            if (existe[x][y])
            {
                reg.x = x;
                reg.y = y;

                reg.espera = tabla[x][y].espera;
                reg.soltarGranada1 = tabla[x][y].soltarGranada1;
                reg.soltarGranada2 = tabla[x][y].soltarGranada2;
                reg.ataqueKamikaze = tabla[x][y].ataqueKamikaze;
                reg.aterrizaje = tabla[x][y].aterrizaje;
                reg.despegue = tabla[x][y].despegue;
                reg.siguientex = tabla[x][y].siguientex;
                reg.siguientey = tabla[x][y].siguientey;

                if (fwrite(&reg, sizeof(OrdenArchivo), 1, f) != 1)
                {
                    cout << "Error al guardar el registro." << endl;
                    fclose(f);
                    return;
                }
            }
        }
    }

    fclose(f);

    cout << "Archivo guardado." << endl;
}
