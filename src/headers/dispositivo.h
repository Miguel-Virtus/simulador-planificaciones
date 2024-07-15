#ifndef DISPOSITIVO_H
#define DISPOSITIVO_H

#include "cola.h"

/**
 * @brief Estructura que representa un dispositivo
 * 
 * @param nombre Nombre del dispositivo
 * @param cola_bloqueados Cola de procesos bloqueados
 * @param traza_procesos Traza de procesos que han pasado por el dispositivo
*/
typedef struct Dispositivo {
    char *nombre;
    Cola cola_bloqueados;
    int *traza_procesos;
    int incremento_por_bloqueo;
} Dispositivo;

Dispositivo *crear_dispositivo(char *nombre, int n_instrucciones, int incremento);

#endif