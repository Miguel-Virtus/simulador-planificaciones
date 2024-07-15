#include <stdio.h>
#include <stdlib.h>

#include "headers/dispositivo.h"


/**
 * ===  FUNCTION  ======================================================================
 *        Name:  crear_dispositivo
 * Description:  Inicializa la estructura de un dispositivo con una cola de procesos
 *               bloqueados y una traza de procesos.
 *        Args:  nombre: nombre del dispositivo
 *               n_instrucciones: número de instrucciones máximas que guardará en la traza
 * Return value: Dispositivo creado
 * =====================================================================================
*/
Dispositivo *crear_dispositivo(char *nombre, int n_instrucciones, int incremento) {
    Dispositivo *d = malloc(sizeof(Dispositivo));
    d->nombre = nombre;
    d->cola_bloqueados = crear_cola();
    d->traza_procesos = (int *) malloc(sizeof(int) * n_instrucciones);
    d->incremento_por_bloqueo = incremento;
    return d;
}