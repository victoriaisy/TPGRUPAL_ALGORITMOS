#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;

// ============================================================
// PUNTOS 1 Y 2 - ESTRUCTURAS, MEMORIA Y FUNCIONES
// ============================================================
const int N = 200;
const int LINEAS_POR_PAGINA = 20;

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

Orden memoria[N][N];
bool ocupada[N][N];

Orden temporal[N][N];
bool temporalOcupada[N][N];
bool visitada[N][N];

bool cargado = false;
int inicioX = 0;
int inicioY = 0;
int cantOrdenes = 0;

void limpiarTemporal()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            temporal[i][j] = Orden();
            temporalOcupada[i][j] = false;
            visitada[i][j] = false;
        }
    }
}

void copiarAMemoria()
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            memoria[i][j] = temporal[i][j];
            ocupada[i][j] = temporalOcupada[i][j];
        }
    }
}

bool enRango(int valor)
{
    return valor >= 0 && valor < N;
}

int diferencia(int a, int b)
{
    if (a > b)
    {
        return a - b;
    }
    return b - a;
}

bool sonAledanias(int x1, int y1, int x2, int y2)
{
    int difX = diferencia(x1, x2);
    int difY = diferencia(y1, y2);

    if (difX == 0 && difY == 0)
    {
        return false;
    }

    return difX <= 1 && difY <= 1;
}

bool tieneAtaque(int x, int y)
{
    return temporal[x][y].soltarGranada1 ||
           temporal[x][y].soltarGranada2 ||
           temporal[x][y].ataqueKamikaze;
}

bool esFinal(int x, int y)
{
    return temporal[x][y].aterrizaje ||
           temporal[x][y].ataqueKamikaze;
}

bool validar(int cantRegistros, int despegueX, int despegueY,
             int cantDespegue, int cantAterrizaje, int cantKamikaze)
{
    int actualX, actualY;
    int proximoX, proximoY;
    int pasos = 0;

    if (cantDespegue != 1)
    {
        printf("ERROR: debe haber una unica instruccion de despegue (hay %d).\n",
               cantDespegue);
        return false;
    }

    if (tieneAtaque(despegueX, despegueY))
    {
        printf("ERROR: el despegue no puede estar acompanado de ataques.\n");
        return false;
    }

    if (cantAterrizaje + cantKamikaze != 1)
    {
        printf("ERROR: debe haber una unica instruccion de aterrizaje o ataque kamikaze (hay %d).\n",
               cantAterrizaje + cantKamikaze);
        return false;
    }

    actualX = despegueX;
    actualY = despegueY;

    while (true)
    {
        if (visitada[actualX][actualY])
        {
            printf("ERROR: se pasa mas de una vez por la celda (%d,%d).\n",
                   actualX, actualY);
            return false;
        }

        visitada[actualX][actualY] = true;
        pasos++;

        if (esFinal(actualX, actualY))
        {
            break;
        }

        proximoX = temporal[actualX][actualY].siguientex;
        proximoY = temporal[actualX][actualY].siguientey;

        if (!enRango(proximoX) || !enRango(proximoY))
        {
            printf("ERROR: la celda (%d,%d) apunta fuera de la matriz (%d,%d).\n",
                   actualX, actualY, proximoX, proximoY);
            return false;
        }

        if (!sonAledanias(actualX, actualY, proximoX, proximoY))
        {
            printf("ERROR: la celda (%d,%d) apunta a (%d,%d), que no es aledana.\n",
                   actualX, actualY, proximoX, proximoY);
            return false;
        }

        if (!temporalOcupada[proximoX][proximoY])
        {
            printf("ERROR: la celda (%d,%d) apunta a (%d,%d), que no tiene orden (hueco).\n",
                   actualX, actualY, proximoX, proximoY);
            return false;
        }

        actualX = proximoX;
        actualY = proximoY;
    }

    if (pasos != cantRegistros)
    {
        printf("ERROR: la ruta recorre %d celdas pero el archivo tiene %d registros.\n",
               pasos, cantRegistros);
        return false;
    }

    return true;
}

void cargarArchivo()
{
    char ruta[256];
    FILE *archivo;
    OrdenArchivo reg;
    int cantRegistros = 0;
    int cantDespegue = 0;
    int cantAterrizaje = 0;
    int cantKamikaze = 0;
    int despegueX = -1;
    int despegueY = -1;
    int largo;

    printf("Ingrese ruta y nombre del archivo: ");
    if (fgets(ruta, sizeof(ruta), stdin) == NULL)
    {
        printf("ERROR: no se pudo leer la ruta.\n");
        return;
    }

    largo = (int)strlen(ruta);
    if (largo > 0 && ruta[largo - 1] == '\n')
    {
        ruta[largo - 1] = '\0';
    }

    archivo = fopen(ruta, "rb");
    if (archivo == NULL)
    {
        printf("ERROR: no se pudo abrir el archivo '%s'.\n", ruta);
        return;
    }

    limpiarTemporal();

    while (true)
    {
        size_t bytesLeidos = fread(&reg, 1, sizeof(OrdenArchivo), archivo);

        if (bytesLeidos == 0)
        {
            if (ferror(archivo))
            {
                printf("ERROR: ocurrio un problema al leer el archivo.\n");
                fclose(archivo);
                return;
            }
            break;
        }

        if (bytesLeidos != sizeof(OrdenArchivo))
        {
            printf("ERROR: el archivo termina con un registro incompleto.\n");
            fclose(archivo);
            return;
        }

        if (!enRango(reg.x) || !enRango(reg.y))
        {
            printf("ERROR: coordenadas fuera de rango (%d,%d). Archivo no cargado.\n",
                   reg.x, reg.y);
            fclose(archivo);
            return;
        }

        if (temporalOcupada[reg.x][reg.y])
        {
            printf("ERROR: la celda (%d,%d) esta repetida. Archivo no cargado.\n",
                   reg.x, reg.y);
            fclose(archivo);
            return;
        }

        temporalOcupada[reg.x][reg.y] = true;
        temporal[reg.x][reg.y].espera = reg.espera;
        temporal[reg.x][reg.y].soltarGranada1 = reg.soltarGranada1;
        temporal[reg.x][reg.y].soltarGranada2 = reg.soltarGranada2;
        temporal[reg.x][reg.y].ataqueKamikaze = reg.ataqueKamikaze;
        temporal[reg.x][reg.y].aterrizaje = reg.aterrizaje;
        temporal[reg.x][reg.y].despegue = reg.despegue;
        temporal[reg.x][reg.y].siguientex = reg.siguientex;
        temporal[reg.x][reg.y].siguientey = reg.siguientey;

        cantRegistros++;

        if (reg.despegue)
        {
            cantDespegue++;
            despegueX = reg.x;
            despegueY = reg.y;
        }

        if (reg.aterrizaje)
        {
            cantAterrizaje++;
        }

        if (reg.ataqueKamikaze)
        {
            cantKamikaze++;
        }
    }

    fclose(archivo);

    if (cantRegistros == 0)
    {
        printf("ERROR: el archivo esta vacio. Archivo no cargado.\n");
        return;
    }

    if (!validar(cantRegistros, despegueX, despegueY,
                 cantDespegue, cantAterrizaje, cantKamikaze))
    {
        printf("El archivo no fue cargado.\n");
        return;
    }

    copiarAMemoria();
    cargado = true;
    inicioX = despegueX;
    inicioY = despegueY;
    cantOrdenes = cantRegistros;

    printf("Archivo cargado correctamente (%d ordenes).\n", cantRegistros);
}

void esperarTecla()
{
    char basura[32];
    printf("-- Presione ENTER para ver la siguiente pagina --");
    fgets(basura, sizeof(basura), stdin);
}

void imprimirEncabezado()
{
    printf("\n===== ATAQUE CARGADO EN MEMORIA =====\n");
    printf("PASO  CELDA       ACCIONES\n");
    printf("------------------------------------------------------------\n");
}

void armarAcciones(int x, int y, char texto[])
{
    char aux[64];
    texto[0] = '\0';

    if (memoria[x][y].despegue)
    {
        strcat(texto, "Despegue. ");
    }

    if (memoria[x][y].aterrizaje)
    {
        strcat(texto, "Aterrizaje. ");
    }

    if (memoria[x][y].soltarGranada1)
    {
        strcat(texto, "Soltar granada 1. ");
    }

    if (memoria[x][y].soltarGranada2)
    {
        strcat(texto, "Soltar granada 2. ");
    }

    if (memoria[x][y].ataqueKamikaze)
    {
        strcat(texto, "Ataque kamikaze. ");
    }

    if (memoria[x][y].espera > 0)
    {
        sprintf(aux, "Esperar %u. ", memoria[x][y].espera);
        strcat(texto, aux);
    }

    if (!memoria[x][y].aterrizaje && !memoria[x][y].ataqueKamikaze)
    {
        sprintf(aux, "Mover a (%d,%d).",
                memoria[x][y].siguientex, memoria[x][y].siguientey);
        strcat(texto, aux);
    }
}

void mostrarAtaque()
{
    int actualX, actualY;
    int proximoX, proximoY;
    int paso = 1;
    int lineasEnPagina = 0;
    bool terminado = false;
    char acciones[256];
    char celda[32];

    if (!cargado)
    {
        printf("No hay ningun ataque cargado en memoria. Use la opcion 1 primero.\n");
        return;
    }

    actualX = inicioX;
    actualY = inicioY;

    imprimirEncabezado();

    while (!terminado)
    {
        if (lineasEnPagina == LINEAS_POR_PAGINA)
        {
            esperarTecla();
            imprimirEncabezado();
            lineasEnPagina = 0;
        }

        armarAcciones(actualX, actualY, acciones);
        sprintf(celda, "(%d,%d)", actualX, actualY);
        printf("%-5d %-11s %s\n", paso, celda, acciones);

        lineasEnPagina++;
        paso++;

        if (memoria[actualX][actualY].aterrizaje ||
            memoria[actualX][actualY].ataqueKamikaze)
        {
            terminado = true;
        }
        else
        {
            proximoX = memoria[actualX][actualY].siguientex;
            proximoY = memoria[actualX][actualY].siguientey;
            actualX = proximoX;
            actualY = proximoY;
        }
    }

    printf("------------------------------------------------------------\n");
    printf("Fin del ataque. Total de ordenes: %d\n", cantOrdenes);
}

// ============================================================
// PUNTOS 3 Y 4
// ============================================================
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

// ============================================================
// PUNTOS 5 Y 6
// Estas dos funciones auxiliares trabajan con la estructura Orden
// ============================================================

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

// PUNTO 5: corrige una orden que ya esta en memoria
// El segundo parametro dice en que posiciones hay ordenes
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

// PUNTO 6: recorre la matriz y guarda las posiciones ocupadas
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

// ============================================================
// PUNTO 7: Visualizar el ataque en un archivo HTML
// Se usa la matriz cargada en memoria, como indica el Anexo 2
// ============================================================
void visualizarAtaqueHTML(Orden tabla[][200], bool existe[][200])
{
    FILE *archivo;
    char nombreArchivo[260];

    cout << "Ingrese el nombre del archivo HTML a crear: ";
    cin >> nombreArchivo;

    archivo = fopen(nombreArchivo, "w");

    if (archivo == NULL)
    {
        cout << "No se pudo crear el archivo HTML." << endl;
        return;
    }

    fprintf(archivo, "<html>\n");
    fprintf(archivo, "<head><title>Ataque</title></head>\n");
    fprintf(archivo, "<body>\n");
    fprintf(archivo, "<h2>Tabla de ataque del drone</h2>\n");
    fprintf(archivo, "<table border=\"1\">\n");

    for (int x = 0; x < 200; x++)
    {
        fprintf(archivo, "<tr>\n");

        for (int y = 0; y < 200; y++)
        {
            fprintf(archivo, "<td>");

            if (existe[x][y])
            {
                fprintf(archivo, "(%d,%d) ", x, y);

                if (tabla[x][y].despegue)
                    fprintf(archivo, "Despegue ");

                if (tabla[x][y].aterrizaje)
                    fprintf(archivo, "Aterrizaje ");

                if (tabla[x][y].soltarGranada1)
                    fprintf(archivo, "Granada1 ");

                if (tabla[x][y].soltarGranada2)
                    fprintf(archivo, "Granada2 ");

                if (tabla[x][y].ataqueKamikaze)
                    fprintf(archivo, "Kamikaze ");

                if (tabla[x][y].espera > 0)
                    fprintf(archivo, "Espera: %u ", tabla[x][y].espera);

                if (!tabla[x][y].aterrizaje && !tabla[x][y].ataqueKamikaze)
                {
                    fprintf(archivo, "Siguiente: (%d,%d)",
                            tabla[x][y].siguientex, tabla[x][y].siguientey);
                }
            }
            else
            {
                fprintf(archivo, "-");
            }

            fprintf(archivo, "</td>\n");
        }

        fprintf(archivo, "</tr>\n");
    }

    fprintf(archivo, "</table>\n");
    fprintf(archivo, "</body>\n");
    fprintf(archivo, "</html>\n");

    fclose(archivo);
    cout << "Archivo HTML creado correctamente." << endl;
}


// ============================================================
// MENU UNICO DE LOS PUNTOS 1 A 7
// ============================================================
int main()
{
    char linea[32];
    int opcion = -1;

    while (opcion != 0)
    {
        printf("\n===== SISTEMA DE GUIA DE APOYO =====\n");
        printf("1. Cargar archivo de ataque en memoria.\n");
        printf("2. Mostrar ataque cargado.\n");
        printf("3. Crear un archivo de ataque nuevo.\n");
        printf("4. Corregir un registro del archivo.\n");
        printf("5. Corregir un registro en memoria.\n");
        printf("6. Guardar memoria en un archivo nuevo.\n");
        printf("7. Visualizar un archivo de ataque en HTML.\n");
        printf("0. Salir.\n");
        printf("Opcion: ");

        if (fgets(linea, sizeof(linea), stdin) == NULL)
        {
            opcion = 0;
        }
        else
        {
            // cin deja un salto de linea despues de leer numeros;
            // ignoramos esas lineas vacias al volver al menu
            if (linea[0] == '\n')
            {
                continue;
            }

            opcion = atoi(linea);

            if (opcion == 1)
            {
                cargarArchivo();
            }
            else if (opcion == 2)
            {
                mostrarAtaque();
            }
            else if (opcion == 3)
            {
                crearArchivo();
            }
            else if (opcion == 4)
            {
                corregirRegistro();
            }
            else if (opcion == 5)
            {
                if (cargado)
                {
                    corregirRegistroMemoria(memoria, ocupada);
                }
                else
                {
                    printf("Primero hay que cargar un archivo en memoria (opcion 1).\n");
                }
            }
            else if (opcion == 6)
            {
                if (cargado)
                {
                    guardarMemoriaArchivo(memoria, ocupada);
                }
                else
                {
                    printf("Primero hay que cargar un archivo en memoria (opcion 1).\n");
                }
            }
            else if (opcion == 7)
            {
                if (cargado)
                {
                    visualizarAtaqueHTML(memoria, ocupada);
                }
                else
                {
                    printf("Primero hay que cargar un archivo en memoria (opcion 1).\n");
                }
            }
            else if (opcion != 0)
            {
                printf("Opcion invalida.\n");
            }
        }
    }

    return 0;
}

