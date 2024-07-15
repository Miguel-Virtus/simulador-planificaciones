#include "headers/semaforo.h"
#include "headers/proceso.h"
#include "headers/cola.h"


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  crear_semaforo
 *  Description:  Crea un semáforo con los valores iniciales dados
 *  	   Args:  nombre: nombre del semáforo
 *  			  valor: valor máximo que puede tener el semáforo
 *  			  n_instrucciones: número de instrucciones que se van a ejecutar
 *       Return:  Puntero al semáforo creado
 * =====================================================================================
 */
Semaforo *crear_semaforo(char *nombre, int valor, int n_instrucciones) {
    Semaforo *semaforo = (Semaforo *) malloc(sizeof(Semaforo));
    semaforo->nombre = nombre;
    semaforo->valor_actual = valor;
    semaforo->cola_procesos = crear_cola();
    semaforo->traza_valor = (int *) malloc(sizeof(int) * n_instrucciones);
    return semaforo;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  wait
 *  Description:  Realiza la operación wait sobre un semáforo. Si el valor del semáforo
 *				  es mayor que 0, lo decrementa y devuelve 0. Si es 0, bloquea el proceso
 *				  y lo inserta en la cola de procesos del semáforo.
 *  	   Args:  semaforo: semáforo sobre el que se realiza la operación
 *  			  p: proceso que realiza la operación
 *       Return:  0 si el proceso no se bloquea, 1 si se bloquea
 * =====================================================================================
 */
int wait(Semaforo *semaforo, Proceso *p) {
    if (semaforo->valor_actual > 0) {
        semaforo->valor_actual--;
        return 0;
    } else {
        insertar_proceso(&(semaforo->cola_procesos), p);
        p->estado_actual = BLOQUEADO;
        return 1;
    }
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  signal
 *  Description:  Realiza la operación signal sobre un semáforo. Si hay procesos en la
 *				  cola de procesos del semáforo, desbloquea el primero y lo devuelve.
 *				  Si no hay procesos en la cola, incrementa el valor del semáforo.
 *  	   Args:  semaforo: semáforo sobre el que se realiza la operación
 *       Return:  Proceso desbloqueado, NULL si no hay procesos en la cola
 * =====================================================================================
 */
Proceso * signal(Semaforo *semaforo) {
    if (cola_vacia(&(semaforo->cola_procesos))) {
        semaforo->valor_actual++;
        return NULL;
    } else {
        Proceso *p = extraer_proceso(&(semaforo->cola_procesos));
        p->estado_actual = LISTO;
        p->contador_programa[0]++;
        p->contador_programa[1] = 0;
        return p;
    }
}