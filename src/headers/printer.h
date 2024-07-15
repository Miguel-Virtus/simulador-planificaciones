/*
 * =====================================================================================
 *
 *       Filename:  printer.h
 *
 *    Description:  Archivo de cabecera par el archivo printer.c 
 *
 *        Version:  1.0
 *        Created:  23/11/23 16:46:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Miguel Virtus Rubio (mvr), miguel.virtus@edu.uah.es
 *        Company:  Universidad de Alcalá
 *
 * =====================================================================================
 */


#ifndef  printer_INC
#define  printer_INC

#include <stdio.h>

#include "planificador.h"
#include "proceso.h"

char traducir(Estado_proceso e);
void imprimir_planificacion(Planificador* plan, FILE* out);
void traducir_pid(int pid, char* pid_str);
void generar_latex(Planificador* plan, FILE* out);
#endif   /* ----- #ifndef printer_INC  ----- */
