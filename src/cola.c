#include <stdlib.h>
#include <stdio.h>

#include "headers/cola.h"

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  crear_cola
 *  Description:  Crea una cola de procesos vacía.
 * Return value:  Cola creada
 * =====================================================================================
 */

Cola crear_cola() {
    Cola cola;
    cola.primero = NULL;
    cola.ultimo = NULL;
    cola.tam = 0;
    return cola;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  insertar_proceso
 *  Description:  Inserta un proceso al final de la cola
 *         Args:  cola: cola donde insertar el proceso
 *                proc: proceso a insertar
 * =====================================================================================
 */

void insertar_proceso(Cola *cola, void *proc) {
    Nodo* nodo = malloc(sizeof(Nodo));
    nodo->dato = proc;
    nodo->siguiente = NULL;
    nodo->anterior = NULL;
    if (cola->primero == NULL) {
        cola->primero = nodo;
        cola->ultimo = nodo;
    } else {
        cola->ultimo->anterior = nodo;
        nodo->siguiente = cola->ultimo;
        cola->ultimo = nodo;
    }
    cola->tam++;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  extraer_proceso
 *  Description:  Extrae un proceso del comienzo de la cola.
 *         Args:  cola: cola de donde extraer el proceso
 * Return value:  Proceso extraído
 * =====================================================================================
 */

void *extraer_proceso(Cola *cola) {
    Nodo* nodo = cola->primero;
    void* proc = nodo->dato;
    cola->primero = nodo->anterior;
    free(nodo);
    cola->tam--;
    return proc;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  cola_vacia
 *  Description:  Comprueba si la cola está vacía
 *         Args:  cola: cola a comprobar
 * Return value:  1 si la cola está vacía, 0 en caso contrario
 * =====================================================================================
 */

int cola_vacia(Cola *cola) {
    return cola->tam == 0;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  cima_cola
 *  Description:  Devuelve el proceso que está en el comienzo de la cola
 *         Args:  cola: cola de donde extraer el proceso
 * Return value:  Proceso que está en el comienzo de la cola
 * =====================================================================================
 */
void *cima_cola(Cola *cola) {
    if (cola_vacia(cola)) {
        return NULL;
    }
    return cola->primero->dato;
}
