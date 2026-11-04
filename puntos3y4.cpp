#include <iostream>
#include <cstdio>
using namespace std;

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

void crearArchivo()
{
    FILE *archivo;
    OrdenArchivo orden;
    char respuesta = 's';
    char nombreArchivo[260];

    cout << "Ingrese la ruta y el nombre del archivo: ";
    cin.getline(nombreArchivo, 260);
    
    archivo = fopen(nombreArchivo, "wb");

    if (archivo == NULL)
    {
        cout << "No se pudo crear el archivo." << endl;
        return;
    }
    while (respuesta == 's')
    {
        cout << "Ingrese x: ";
        cin >> orden.x;

        cout << "Ingrese y: ";
        cin >> orden.y;

        cout << "Ingrese el tiempo de espera: ";
        cin >> orden.espera;

        cout << "Desea soltar la granada 1? (1 = si, 0 = no): ";
        cin >> orden.soltarGranada1;

        cout << "Desea soltar la granada 2? (1 = si, 0 = no): ";
        cin >> orden.soltarGranada2;

        cout << "Desea realizar un ataque kamikaze? (1 = si, 0 = no): ";
        cin >> orden.ataqueKamikaze;

        cout << "Desea aterrizar? (1 = si, 0 = no): ";
        cin >> orden.aterrizaje;

        cout << "Desea despegar? (1 = si, 0 = no): ";
        cin >> orden.despegue;

        cout << "Ingrese siguientex: ";
        cin >> orden.siguientex;

        cout << "Ingrese siguientey: ";
        cin >> orden.siguientey;

        fwrite(&orden, sizeof(OrdenArchivo), 1, archivo);

        cout << "Registro guardado correctamente." << endl;

        cout << "Desea cargar otra orden? (s/n): ";
        cin >> respuesta;
    }

    fclose(archivo);

    cout << "Archivo cerrado correctamente." << endl;
}
void corregirRegistro()
{
    char nombreArchivo[260];
    int xBuscada, yBuscada;
    OrdenArchivo orden;

    cout << "Ingrese el nombre del archivo: ";
    cin.getline(nombreArchivo, 260);

    FILE* archivo = fopen(nombreArchivo, "r+b");

    if (archivo == NULL)
    {
        cout << "No se pudo abrir el archivo." << endl;
        return;
    }

    cout << "Ingrese la coordenada x del registro: ";
    cin >> xBuscada;

    cout << "Ingrese la coordenada y del registro: ";
    cin >> yBuscada;

    bool encontrado = false;

    while (fread(&orden, sizeof(OrdenArchivo), 1, archivo) == 1)
    {
        if (orden.x == xBuscada && orden.y == yBuscada)
        {
            encontrado = true;

            cout << "\nRegistro encontrado. Datos actuales:" << endl;
            cout << "x: " << orden.x << endl;
            cout << "y: " << orden.y << endl;
            cout << "Espera: " << orden.espera << endl;
            cout << "Granada 1: " << orden.soltarGranada1 << endl;
            cout << "Granada 2: " << orden.soltarGranada2 << endl;
            cout << "Ataque kamikaze: " << orden.ataqueKamikaze << endl;
            cout << "Aterrizaje: " << orden.aterrizaje << endl;
            cout << "Despegue: " << orden.despegue << endl;
            cout << "Siguiente x: " << orden.siguientex << endl;
            cout << "Siguiente y: " << orden.siguientey << endl;

            cout << "\nIngrese los nuevos datos:" << endl;

            cout << "x: ";
            cin >> orden.x;

            cout << "y: ";
            cin >> orden.y;

            cout << "Espera: ";
            cin >> orden.espera;

            cout << "Soltar granada 1 (0 o 1): ";
            cin >> orden.soltarGranada1;

            cout << "Soltar granada 2 (0 o 1): ";
            cin >> orden.soltarGranada2;

            cout << "Ataque kamikaze (0 o 1): ";
            cin >> orden.ataqueKamikaze;

            cout << "Aterrizaje (0 o 1): ";
            cin >> orden.aterrizaje;

            cout << "Despegue (0 o 1): ";
            cin >> orden.despegue;

            cout << "Siguiente x: ";
            cin >> orden.siguientex;

            cout << "Siguiente y: ";
            cin >> orden.siguientey;

            fseek(archivo, -static_cast<long>(sizeof(OrdenArchivo)), SEEK_CUR);

            fwrite(&orden, sizeof(OrdenArchivo), 1, archivo);

            cout << "Registro corregido correctamente." << endl;

            break;
        }
    }

    if (!encontrado)
    {
        cout << "No se encontro un registro con esas coordenadas." << endl;
    }

    fclose(archivo);
}
int main()
{
    int opcion;

    cout << "1. Crear archivo de ataque nuevo" << endl;
    cout << "2. Corregir registro del archivo" << endl;
    cout << "Seleccione una opcion: ";

    cin >> opcion;

    cin.ignore();

    if (opcion == 1)
    {
        crearArchivo();
    }
    else if (opcion == 2)
    {
        corregirRegistro();
    }
    else
    {
        cout << "Opcion incorrecta." << endl;
    }

    return 0;
}
