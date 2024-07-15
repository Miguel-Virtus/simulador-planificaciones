/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description: Script princial del programa 
 *
 *        Version:  1.0
 *        Created:  22/11/23 16:23:20
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Miguel Virtus Rubio (mvr), miguel.virtus@edu.uah.es
 *        Company:  Universidad de Alcalá
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include "headers/planificador.h"
#include "headers/proceso.h"
#include "headers/printer.h"
#include "headers/parser.h"
#include "headers/semaforo.h"

int main(int argc, char *argv[])
{
	if (argc < 3) {
		printf("Uso: %s <conf_file> <proc1_file> <proc2_file> ... <procN_file>\n", argv[0]);
		exit(1);
	}
	
	Plan_conf conf = parsear_configuracion(argv[1]); 	
	int n_proc = argc - 2;
	Proceso** lista_procesos = malloc(sizeof(Proceso*) * n_proc);
	Dispositivo **lista_dispositivos = malloc(sizeof(Dispositivo*) * conf.n_dispositivos);
	for (int i = 0; i < conf.n_dispositivos; i++) {
		lista_dispositivos[i] = crear_dispositivo(conf.nombre_dispositivos[i], conf.max_instrucciones, conf.incremento_dispositivo[i]);
	}
	Semaforo **lista_semaforos = malloc(sizeof(Semaforo*) * conf.n_semaforos);
	for (int i = 0; i < conf.n_semaforos; i++) {
		lista_semaforos[i] = crear_semaforo(conf.nombre_semaforos[i], conf.size_semaforos[i], conf.max_instrucciones);
	}
    Planificador* plan = crear_planificador(lista_procesos, n_proc, lista_dispositivos, lista_semaforos, conf);	
	
	for (int i = 2; i < argc; i++) {
		Proceso* p = parsear_proceso(argv[i], lista_dispositivos, lista_semaforos, conf);
		lista_procesos[i-2] = p;
		if (p->t_llegada == 0) {
			entrada_proceso(plan, p);
		}
	}

	
	simular_planificador(plan);
	

	if (todas_colas_vacias(plan) && !plan->proceso_ejecucion) {
		printf("Todos los procesos han terminado\n");
	} else {
		printf("Se ha alcanzado el máximo de unidades de tiempo a simular indicado, pero aún quedan procesos por ejecutar\n");
	}

	imprimir_planificacion(plan, stdout);
	FILE* latex = fopen("../planificacion.tex", "w");
	generar_latex(plan, latex);
	fclose(latex);

	if (conf.n_dispositivos > 0) {
		free(conf.nombre_dispositivos);
		free(conf.incremento_dispositivo);
	}
	if (conf.n_semaforos > 0) {
		free(conf.nombre_semaforos);
		free(conf.size_semaforos);
	}
	liberar_memoria(plan);
	
	return 0;
	
}			/* ----------  end of function main  ---------- */
