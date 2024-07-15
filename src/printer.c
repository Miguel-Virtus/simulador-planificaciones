/*
 * =====================================================================================
 *
 *       Filename:  printer.c
 *
 *    Description:  Archivo con las funciones encargadas de mostrar el resultado de una
 *    		        planificación.
 *
 *        Version:  1.0
 *        Created:  23/11/23 16:42:27
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
#include <string.h>

#include "headers/printer.h"
#include "headers/planificador.h"
#include "headers/proceso.h"


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  traducir
 *  Description:  Transforma un estado de un proceso en un carácter, de forma que se
 *	 			  pueda imprimir por pantalla de forma más legible. Por ejemplo, para 
 *				  el estado LISTO, devuelve 'L'.
 *  	   Args:  e: Estado del proceso
 *       Return:  Carácter que representa el estado del proceso
 * =====================================================================================
 */
char traducir(Estado_proceso e) {
	switch (e) {
		case NUEVO:
			return 'N';
		case LISTO:
			return 'L';
		case EJECUTANDO:
			return 'E';
		case TERMINADO:
			return 'T';
		case BLOQUEADO:
			return 'B';
		default:
			return 'X';
	}
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  imprimir_planificacion
 *  Description:  Escribe en un archivo una tabla con la planificación de un planificador
 *  	   Args:  plan: Planificador cuya planificación se quiere imprimir
 *  			  out: archivo en el que se escribe la planificación
 * =====================================================================================
 */
void imprimir_planificacion(Planificador* plan, FILE* out) {
    printf("proc\t");
	for (int i = 0; i < plan->conf.max_instrucciones; i++) {
		fprintf(out,"%3d", i);
	}
	for (int i = 0; i < plan->n_proc; i++) {
		fprintf(out, "\n%-8s", strchr(plan->lista_procesos[i]->nombre, '/') + 1);
		for (int j = 0; j < plan->conf.max_instrucciones; j++) {
			fprintf(out, "%3c", traducir(plan->lista_procesos[i]->traza_estados[j]));
		}
		if (plan->conf.alg == CR) {
			fprintf(out, "\nprio\t");
			for (int j = 0; j < plan->conf.max_instrucciones; j++) {
				fprintf(out, "%3d", plan->lista_procesos[i]->traza_prioridades[j]);
			}
			fprintf(out, "\n");
		}
	}

	printf("\n");

	if (plan->conf.n_dispositivos > 0) {
		for (int i = 0; i < plan->conf.n_dispositivos; i++) {
			fprintf(out, "\n%-8s", plan->dispositivos[i]->nombre);
			for (int j = 0; j < plan->conf.max_instrucciones; j++) {
				char pid[16];
				traducir_pid(plan->dispositivos[i]->traza_procesos[j], pid);
				fprintf(out, "%3s", pid);
			}
		}
		printf("\n");
	}

	if (plan->conf.n_semaforos > 0) {	
		for (int i = 0; i < plan->conf.n_semaforos; i++) {
			fprintf(out, "\n%-8s", plan->semaforos[i]->nombre);
			for (int j = 0; j < plan->conf.max_instrucciones; j++) {
				fprintf(out, "%3d", plan->semaforos[i]->traza_valor[j]);
			}
		}
		printf("\n");

	}
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  traducir_pid
 * 	Description:  Transforma un pid en un string, de forma que se pueda imprimir por
 * 				  pantalla de forma más legible. Si el pid es -1, devuelve un carácter
 * 				  vacío.
 * 	  	   Args:  pid: pid a transformar
 * 			  	  pid_str: string donde se guarda el resultado
 * =====================================================================================
 */

void traducir_pid(int pid, char* pid_str) {
	if (pid == -1) {
		sprintf(pid_str, " ");
	} else {
		sprintf(pid_str, "%d", pid);
	}
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  generar_latex
 *  Description:  Genera un archivo .tex con la planificación de un planificador que
 * 			  	  puede ser compilado con pdflatex para obtener un pdf con la planificación
 *  	   Args:  plan: Planificador cuya planificación se quiere imprimir
 *  			  out: archivo en el que se escribe la planificación
 * =====================================================================================
 */
void generar_latex(Planificador* plan, FILE* out) {
	fprintf(out, "\\documentclass{article}\n");
	fprintf(out, "\\usepackage{pdflscape}\n");
	fprintf(out, "\\begin{document}\n");
	fprintf(out, "\\thispagestyle{empty}\n");
	fprintf(out, "\\begin{landscape}\n");
	fprintf(out, "\\begin{center}\n");
	fprintf(out, "\\hspace*{-4cm}\n");

	fprintf(out, "\\begin{tabular}{|c|");
	for (int i = 0; i < plan->conf.max_instrucciones; i++) {
		fprintf(out, "c|");
	}
	fprintf(out, "}\n");
	fprintf(out, "\\hline\n");
	fprintf(out, "Proceso ");
	for (int i = 0; i < plan->conf.max_instrucciones; i++) {
		fprintf(out, "& %d ", i);
	}
	fprintf(out, "\\\\ [0.5ex] \n");
	fprintf(out, "\\hline\\hline\n");

	for (int i = 0; i < plan->n_proc; i++) {
		fprintf(out, "%s ", strchr(plan->lista_procesos[i]->nombre, '/') + 1);
		for (int j = 0; j < plan->conf.max_instrucciones; j++) {
			fprintf(out, "& %c ", traducir(plan->lista_procesos[i]->traza_estados[j]));
		}
		fprintf(out, "\\\\\n\\hline\n");
		if (plan->conf.alg == CR) {
			fprintf(out, "\nprio\t");
			for (int j = 0; j < plan->conf.max_instrucciones; j++) {
				fprintf(out, "& %d", plan->lista_procesos[i]->traza_prioridades[j]);
			}
			fprintf(out, "\n");
		fprintf(out, "\\\\\n\\hline\\hline\n");
		}
	}

	for (int i = 0; i < plan->conf.n_dispositivos; i++) {
		fprintf(out, "Dispositivo: %s ", plan->dispositivos[i]->nombre);
		for (int j = 0; j < plan->conf.max_instrucciones; j++) {
			char pid[16];
			traducir_pid(plan->dispositivos[i]->traza_procesos[j], pid);
			fprintf(out, "& %s", pid);
		}
		fprintf(out, "\\\\\n\\hline\n");
	}

	for (int i = 0; i < plan->conf.n_semaforos; i++) {
		fprintf(out, "Semáforo: %s ", plan->semaforos[i]->nombre);
		for (int j = 0; j < plan->conf.max_instrucciones; j++) {
			fprintf(out, "& %d", plan->semaforos[i]->traza_valor[j]);
		}
		fprintf(out, "\\\\\n\\hline\n");

	}
	
	
	fprintf(out, "\\end{tabular}\n");
	fprintf(out, "\\end{center}\n");
	fprintf(out, "\\end{landscape}\n");
	fprintf(out, "\\end{document}\n");
}
