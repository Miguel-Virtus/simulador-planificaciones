/*
 * =====================================================================================
 *
 *       Filename:  cola.c
 *
 *    Description:  Archivo cabecera de la implementación de una estructura de datos cola.
 *
 *        Version:  1.0
 *        Created:  25/11/23 23:52:14
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Miguel Virtus Rubio (mvr), miguel.virtus@edu.uah.es
 *        Company:  Universidad de Alcalá
 *
 * =====================================================================================
 */

#ifndef _COLA_H_
#define _COLA_H_

#include <stdio.h>
#include <stdlib.h>



typedef struct _nodo {
    void *dato;
    struct _nodo *siguiente;
    struct _nodo *anterior; 
} Nodo;

typedef struct _cola {
    char *nombre;
    Nodo *primero;
    Nodo *ultimo;
    int tam;
} Cola;


Cola crear_cola();
void insertar_proceso(Cola *cola, void *proceso);
void *extraer_proceso(Cola *cola);
int cola_vacia(Cola *cola);
void *cima_cola(Cola *cola);
#endif
