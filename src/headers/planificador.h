/*
 * =====================================================================================
 *
 *       Filename:  planificador.h
 *
 *    Description:  Archivo de cabecera de planificador.c 
 *
 *        Version:  1.0
 *        Created:  23/11/23 02:09:27
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Miguel Virtus Rubio (mvr), miguel.virtus@edu.uah.es
 *        Company:  Universidad de Alcalá
 *
 * =====================================================================================
 */

#ifndef  planificador_INC
#define  planificador_INC

#include "proceso.h"
#include "cola.h"
#include "dispositivo.h"
#include "conf.h"
#include "semaforo.h"

typedef enum Estado_planificador {
	CONTINUE,	// El planificador sigue ejecutando
	STOP,		// El planificador se detiene
} Estado_planificador;


/**
 * @brief Estructura que representa un planificador
 * 
 * @param lista_procesos Lista de procesos del planificador, sin ningún orden específico.
 * @param proceso_ejecucion Proceso que se está ejecutando en el planificador (Un único procesador)
 * @param cola_listos Cola de procesos listos
 * @param dispositivos Lista de dispositivos del planificador (Cada uno con su respectiva cola de bloqueados)
 * @param n_dispositivos Número de dispositivos del planificador
 * @param quantum Quantum del planificador
 * @param quantum_actual Quantum actual del proceso que está en ejecución
 * @param max_instrucciones Máximo de instrucciones que ejecutará el planificador
 * @param n_proc Número de procesos del planificador 
*/
typedef struct Planificador {
	Proceso **lista_procesos;
	Proceso *proceso_ejecucion;
	Cola  *cola_listos;
	Dispositivo **dispositivos;
	Semaforo **semaforos;
	int quantum_actual;
	int n_proc;
	Plan_conf conf;
} Planificador;


Planificador* crear_planificador(Proceso** lista_procesos, int n_proc, Dispositivo **lista_dispositivos, Semaforo **lista_semaforos, Plan_conf conf);
void entrada_proceso(Planificador *planificador, Proceso *proceso);
int activar_requisa(Planificador *plan, Proceso *p);
void salida_proceso(Planificador *planificador);
Estado_planificador activar_proceso(Planificador *planificador);
Proceso * extraer_proceso_SJF(Cola *cola);
void detener_proceso(Planificador *planificador);
void bloquear_proceso(Proceso *p);
void despertar_proceso(Proceso *p);
int hay_procesos_bloqueados(Planificador *planificador);
void simular_planificador(Planificador *planificador);
Estado_planificador ejecutar_ciclo(Planificador *planificador, int instante);
void avanzar_bloqueos(Planificador *plan);
Estado_planificador avanzar_proceso(Planificador* plan, Proceso *p) ;
void guardar_estados(Planificador *planificador, int instante);
void liberar_memoria(Planificador *planificador);
int pasos_restantes(Proceso *p);
int todas_colas_vacias(Planificador *plan);
Proceso * extraer_proceso_CR(Planificador * plan);
void incrementar_prioridad(Proceso *p, int valor, int max);
int todos_semaforos_vacios(Planificador *plan);
#endif   /* ----- #ifndef planificador_INC  ----- */
