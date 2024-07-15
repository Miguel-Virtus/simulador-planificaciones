#ifndef _SEMAFORO_H_
#define _SEMAFORO_H_

#include <stdio.h>
#include <stdlib.h>

#include "proceso.h"

typedef struct semaforo {
    char *nombre;
    int valor_actual;
    Cola cola_procesos;
    int *traza_valor;
} Semaforo;


Semaforo *crear_semaforo(char *nombre, int valor, int n_instrucciones);
int wait(Semaforo *semaforo, Proceso *p);
Proceso * signal(Semaforo *semaforo);


#endif