/*
 * =====================================================================================
 *
 *       Filename:  proceso.h
 *
 *    Description:  Archivo de cabecera de proceso.c 
 *
 *        Version:  1.0
 *        Created:  22/11/23 20:56:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Miguel Virtus Rubio (mvr), miguel.virtus@edu.uah.es
 *        Company:  Universidad de Alcalá
 *
 * =====================================================================================
 */

#ifndef  PROCESO_H
#define  PROCESO_H

#include "dispositivo.h"

typedef enum Estado_proceso {
	NUEVO,
	LISTO,
	EJECUTANDO,
	TERMINADO,
	BLOQUEADO,
} Estado_proceso;


typedef enum Tipo_instruccion {
	ES,
	CPU,
	WAIT,
	SIGNAL,
} Tipo_instruccion;


typedef struct Instruccion {
	Tipo_instruccion tipo;
	int duracion;
	void *objetivo; // *Dispositivo o *Semaforo
} Instruccion;

/**
 * @brief Estructura que representa un proceso
 *
 * @param nombre Nombre del proceso
 * @param pid Identificador del proceso
 * @param codigo Lista de instrucciones de proceso
 * @param n_instrucciones Número de instrucciones del proceso
 * @param contador_programa Contador de programa del proceso, el primer
 * valor representa la instrucción en la que se encuentra (indice de la 
 * lista anterior) y el segundo valor el número de ciclos ejecutados de 
 * la instrucción actual
 * @param estado_actual Estado actual del proceso
 * @param traza_estados Traza de estados del proceso
 */

typedef struct Proceso {
	char *nombre;
	int pid;
	Instruccion* codigo;
	int n_instrucciones;
	int contador_programa[2];
	Estado_proceso estado_actual;
	Estado_proceso* traza_estados;
	int *traza_prioridades;
	int prioridad;
	int t_llegada;
} Proceso;

Proceso* crear_proceso(char* nombre, int pid, 
					   Instruccion* instrucciones, int n_instr,
					   int maximo_instrucciones, int prioridad, int t_llegada);
					   
#endif

