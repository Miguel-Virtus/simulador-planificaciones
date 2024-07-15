#ifndef  CONF_H
#define  CONF_H


typedef enum algoritmo {
    FIFO,
    RR,
    SJF,
    CR,

} algoritmo;

typedef struct Plan_conf {
    int quantum;
    int max_instrucciones;
    int n_dispositivos;
    int n_semaforos;
    int requisa;
    algoritmo alg;
    char** nombre_dispositivos;
    char** nombre_semaforos;
    int n_colas;
    int *incremento_dispositivo;
    int incremento_semaforos;
    int *size_semaforos;
} Plan_conf;

#endif