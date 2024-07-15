
#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "proceso.h"
#include "dispositivo.h"
#include "conf.h"


Proceso* parsear_proceso(char* archivo, Dispositivo** lista_dispositivos, Semaforo **semaforos, Plan_conf conf);
Semaforo * buscar_semaforo(char *buff, Semaforo **semaforos, Plan_conf conf);
Plan_conf parsear_configuracion(char* archivo);
void parse_name_list(char **dst, char* buff);
void parse_int_list(int *dst, char* buff);
#endif