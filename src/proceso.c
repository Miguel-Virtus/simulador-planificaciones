#include <stdio.h>
#include <stdlib.h>

#include "headers/proceso.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  crear_proceso
 *  Description:  Crea un proceso a partir de los parámetros dados
 *  	   Args:  nombre: nombre del proceso
 * 				  pid: identificador del proceso
 *				  instrucciones: lista de instrucciones del proceso
 *      		  n_instr: número de instrucciones del proceso
 * 				  maximo_estados: número máximo de estados que puede tener el proceso, 
 * 				  es decir, el número máximo de ciclos que va a ejecutarse el planifador
 * =====================================================================================
 */

Proceso* crear_proceso(char* nombre, int pid, Instruccion* instrucciones, 
					   int n_instr, int maximo_estados, int prioridad, int t_llegada) {
	Proceso* p = (Proceso*) malloc(sizeof(Proceso));
	p->nombre = nombre;
	p->pid = pid;
	p->codigo = instrucciones;
	p->n_instrucciones = n_instr;
	p->contador_programa [0] = 0;
	p->contador_programa [1] = 0;
	p->estado_actual = NUEVO;
	p->traza_estados = malloc(sizeof(Estado_proceso) * maximo_estados);
	p->traza_prioridades = malloc(sizeof(int) * maximo_estados);
	p->prioridad = prioridad;
	p->t_llegada = t_llegada;
	return p;
}