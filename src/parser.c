#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "headers/planificador.h"
#include "headers/proceso.h"
#include "headers/parser.h"
#include "headers/dispositivo.h"

int pid = 1;
int max_instrucciones = 20;
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parsear_entrada
 *  Description:  Función que analiza un archivo de texto y devuelve un proceso con las
 *                instrucciones que contiene.
 *         Args:  archivo: nombre del archivo a analizar
 *                max_instr:  número máximo de ciclos que se van a ejecutar en la simulación
 * Return value:  puntero al proceso creado
 * =====================================================================================
 */
    
Proceso* parsear_proceso(char* archivo, Dispositivo** dispositivos, Semaforo** semaforos, Plan_conf conf) {
    FILE* fp = fopen(archivo, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir el archivo %s\n", archivo);
        exit(1);
    }
    char buff[32];
    Instruccion* instrucciones = (Instruccion*) malloc(sizeof(Instruccion) * max_instrucciones);
    int instr = 0;
    int prioridad = 0;
    int t_llegada = 0;
    while (fgets(buff, 32, fp)) {
        if (buff[0] == '#') { // prioridad
            char *prio = strchr(buff, '#') + 1;
            prioridad = atoi(prio);
            instr--;
            if (conf.alg != CR) {
                prioridad = 0;
            }
        } else if (buff[0] == '@') { // instante de llegada
            char *llegada = strchr(buff, '@') + 1;
            t_llegada = atoi(llegada);
            instr--;
        } else if (strncmp(buff, "cpu", strlen("cpu")) == 0) { // instrucción de CPU
            instrucciones[instr].tipo = CPU;
            sscanf(buff, "cpu(%d)", &(instrucciones[instr].duracion));
        } else if (strncmp(buff, "wait", strlen("wait")) == 0) {    // instrucción de WAIT
            instrucciones[instr].tipo = WAIT;
            instrucciones[instr].duracion = 1;
            instrucciones[instr].objetivo = buscar_semaforo(buff, semaforos, conf);
        } else if (strncmp(buff, "signal", strlen("signal")) == 0) {    // instrucción de SIGNAL
            instrucciones[instr].tipo = SIGNAL;
            instrucciones[instr].duracion = 1;
            instrucciones[instr].objetivo = buscar_semaforo(buff, semaforos, conf);
        } else {
            instrucciones[instr].tipo = ES; // instrucción de E/S
            char *fin_nombre = strchr(buff, '(');
            char nombre[16];
            strncpy(nombre, buff, fin_nombre - buff);
            nombre[fin_nombre - buff] = '\0';
            sscanf(fin_nombre, "(%d)", &(instrucciones[instr].duracion));
            for (int i = 0; i < conf.n_dispositivos; i++) {
                if (strcmp(nombre, dispositivos[i]->nombre) == 0) {
                    instrucciones[instr].objetivo = dispositivos[i];
                    break;
                }
            }
        }
        instr++;
    }
    fclose(fp);
    return crear_proceso(archivo, pid++, instrucciones, instr, conf.max_instrucciones, prioridad, t_llegada);
    
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  buscar_semaforo
 *  Description:  Función que busca un semáforo en un array de semáforos
 *         Args:  buff: cadena que contiene el nombre del semáforo
 *                semaforos: arreglo de semáforos
 *                conf: configuración del planificador
 * Return value:  puntero al semáforo encontrado
 * =====================================================================================
 */
Semaforo * buscar_semaforo(char *buff, Semaforo **semaforos, Plan_conf conf) {
    char *comienzo_nombre = strchr(buff, '(') + 1;
    char *fin_nombre = strchr(buff, ')');
    char nombre[16];
    strncpy(nombre, comienzo_nombre, fin_nombre - comienzo_nombre);
    nombre[fin_nombre - comienzo_nombre] = '\0';
    for (int i = 0; i < conf.n_semaforos; i++) {
        if (strcmp(nombre, semaforos[i]->nombre) == 0) {
            return semaforos[i];
        }
    }
    return NULL;
}

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  parsear_configuracion
 *  Description:  Función que analiza un archivo y devuelve una configuración de un planificador
 *         Args:  archivo: nombre del archivo a analizar
 * Return value:  configuración del planificador
 * =====================================================================================
 */

Plan_conf parsear_configuracion(char* archivo) {
    FILE* fp = fopen(archivo, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error al abrir el archivo %s\n", archivo);
        exit(1);
    }
    char buff[32];
    Plan_conf conf;
    conf.quantum = -1;
    conf.max_instrucciones = 128;
    conf.alg = FIFO;
    conf.n_colas = 1;
    conf.n_semaforos = 0;
    conf.n_dispositivos = 0;
    conf.requisa = 'N';
    while (fgets(buff, 32, fp)) {
        if (strncmp(buff, "quantum=", strlen("quantum=")) == 0) {
            sscanf(buff, "quantum=%d", &(conf.quantum));
        } else if (strncmp(buff, "algoritmo=%s", strlen("algoritmo=")) == 0) {
            char* algoritmo = strchr(buff, '=') + 1;
            if (strncmp(algoritmo, "FIFO", strlen("FIFO")) == 0) {
                conf.alg = FIFO;
            } else if (strncmp(algoritmo, "SJF", strlen("SJF")) == 0) {
                conf.alg = SJF;
            } else if (strncmp(algoritmo, "CR", strlen("CR")) == 0) {
                conf.alg = CR;
            } else if (strncmp(algoritmo, "RR", strlen("RR")) == 0) {
                conf.alg = RR;
            } 
            /** add more algorithms */
        } else if (strncmp(buff, "max_ut=", strlen("max_ut=")) == 0) {
            sscanf(buff, "max_ut=%d", &(conf.max_instrucciones));
        } else if (strncmp(buff, "incremento_semaforos=", strlen("incremento_semaforos=")) == 0) {
            sscanf(buff, "incremento_semaforos=%d", &(conf.incremento_semaforos));
        } else if (strncmp(buff, "n_colas=", strlen("n_colas=")) == 0) {
            sscanf(buff, "n_colas=%d", &(conf.n_colas));
        } else if (strncmp(buff, "n_semaforos=", strlen("n_semaforos=")) == 0) {
            sscanf(buff, "n_semaforos=%d", &(conf.n_semaforos));
            printf("n_semaforos=%d\n", conf.n_semaforos);
            conf.nombre_semaforos = (char**) malloc(sizeof(char*) * conf.n_semaforos);
            conf.size_semaforos = (int *) malloc(sizeof(int) * conf.n_semaforos);
            memset(conf.size_semaforos, 0, sizeof(int) * conf.n_semaforos);
        } else if (strncmp(buff, "n_dispositivos=", strlen("n_dispositivos=")) == 0) {
            sscanf(buff, "n_dispositivos=%d", &(conf.n_dispositivos));
            conf.nombre_dispositivos = (char**) malloc(sizeof(char*) * conf.n_dispositivos);
            conf.incremento_dispositivo = (int *) malloc(sizeof(int) * conf.n_dispositivos);
            memset(conf.incremento_dispositivo, 0, sizeof(int) * conf.n_dispositivos);
        } else if (strncmp(buff, "incremento_dispositivos=", strlen("incremento_dispositivos=")) == 0) {
            parse_int_list(conf.incremento_dispositivo, buff);
        } else if (strncmp(buff, "dispositivos=", strlen("dispositivos=")) == 0) {
            parse_name_list(conf.nombre_dispositivos, buff);
        } else if (strncmp(buff, "valor_semaforos=", strlen("valor_semaforos=")) == 0) {
            parse_int_list(conf.size_semaforos, buff);
        } else if (strncmp(buff, "semaforos=", strlen("semaforos=")) == 0) {
            parse_name_list(conf.nombre_semaforos, buff);
        }else if (strncmp(buff, "requisa=", strlen("requisa=")) == 0) {
            sscanf(buff, "requisa=%lc", &(conf.requisa));
        }
    }
    if (conf.alg == FIFO) {
        conf.quantum = -1;
    }
    fclose(fp);
    return conf;
}

void parse_name_list(char **dst, char* buff) {
            char* comienzo = strchr(buff, '{') + 1;
            int disp = 0;
            int i = -1;
            int desp = 0;
            char nombre[16];
            do {
                i++;
                if (comienzo[i] == ',' || comienzo[i] == '}') {
                    nombre[desp] = '\0';
                    dst[disp] = (char*) malloc(sizeof(char) * (strlen(nombre) + 1));
                    strcpy(dst[disp], nombre);
                    desp = 0;
                    disp++;
                } else {
                    nombre[desp++] = comienzo[i];
                }                                
            } while (comienzo[i] != '}');

}

void parse_int_list(int *dst, char* buff) {
            char* comienzo = strchr(buff, '{') + 1;
            int disp = 0;
            int i = -1;
            do {
                i++;
                if (comienzo[i] == ',' || comienzo[i] == '}') {
                    disp++;
                } else {
                    dst[disp] = dst[disp] * 10 + (comienzo[i] - '0');
                }                                
            } while (comienzo[i] != '}');
}