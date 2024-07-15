#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "headers/planificador.h"
#include "headers/proceso.h"
#include "headers/cola.h"
#include "headers/semaforo.h"

# define MIN(a,b) (((a)<(b))?(a):(b))

Cola procesos_entrada;
/*
 * ===  FUNCTION  ======================================================================
 *         Name:  crear_planificador
 *  Description:  Crea un planificador
 *    	   Args:  quantum: número de ciclos que puede ejecutar un proceso de forma seguida
*                 max_instrucciones: número máximo de ciclos que se van a ejecutar en la simulación
*                 lista_procesos: lista con los procesos que se van a simular
*                 n_proc: número de procesos que se van a simular
	Return value: puntero al planificador creado
 * =====================================================================================
 */

Planificador* crear_planificador(Proceso** lista_procesos, int n_proc, Dispositivo **lista_dispositivos, Semaforo **lista_semaforos, Plan_conf conf) {
	Planificador* p = (Planificador*) malloc(sizeof(Planificador));
	p->lista_procesos = lista_procesos;
	p->proceso_ejecucion = NULL;
	p->cola_listos = malloc(sizeof(Cola) * conf.n_colas);
	for (int i = 0; i < conf.n_colas; i++) {
		p->cola_listos[i] = crear_cola();
	}
	p->semaforos = lista_semaforos;
	p->dispositivos = lista_dispositivos;
	p->quantum_actual = 0;
	p->n_proc = n_proc; 
	p->conf = conf;
	return p;
	procesos_entrada = crear_cola();
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  entrada_proceso 
 *  Description:  añade un proceso al planificador, se ejecuta cuando llega un nuevo 
  * 			  proceso al sistema o cuando un proceso se desbloquea.
 *  	   Args:  plan: planificador donde insertar el proceso
 *  			  p: proceso a insertar
 * =====================================================================================
 */

void entrada_proceso(Planificador* plan, Proceso* p) {
	Instruccion instr = p->codigo[p->contador_programa[0]];
	if (p->contador_programa[0] == p->n_instrucciones) {
		p->estado_actual = TERMINADO;
		return;
	}
	if (instr.tipo == ES) {
		bloquear_proceso(p);
	} else { // CPU, WAIT y SIGNAL SON OPERACIONES EN CPU
		if (activar_requisa(plan, p)) {
			detener_proceso(plan);
			plan->proceso_ejecucion = p;
			p->estado_actual = EJECUTANDO; // TODO: EJECUTANDO_WAIT y EJECUTANDO_SIGNAL???
		} else {
			p->estado_actual = LISTO;
			insertar_proceso(&plan->cola_listos[p->prioridad], p);
		}
	}
}

void calcular_llegadas(Planificador *plan) {
	Proceso *p = cima_cola(&procesos_entrada);
	while (p) {
		entrada_proceso(plan, extraer_proceso(&procesos_entrada));
		p = cima_cola(&procesos_entrada);
	}
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  activar_requisa 
 *  Description:  Indica si un proceso puede activar la requisa del planificador, según 
 * 				  el algoritmo que este tenga.
 *  	   Args:  plan: planificador donde mirar si se activa la requisa
 *  			  p: proceso que puede activar la requisa
 * Return Value:  1 si se puede activar, 0 si no.
 * =====================================================================================
 */
int activar_requisa(Planificador *plan, Proceso *p) {
	int requisa = plan->conf.requisa == 'S';
	switch (plan->conf.alg) {
		case FIFO:
			case RR:
			requisa = 0;
			break;
		case SJF:
			requisa &= pasos_restantes(p) < pasos_restantes(plan->proceso_ejecucion);
			break;
		case CR:
			if (plan->proceso_ejecucion) {
				requisa &= p->prioridad > plan->proceso_ejecucion->prioridad; // TODO: y que sea el primero de su cola?
			} else {
				requisa = 0;
			}
			break;
	}
	return requisa;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  salida_proceso
 *  Description:  Elimina un proceso del planificador, se da cuando un proceso termina
 * 				  el total de sus ejecuciones.
 *   	   Args:  plan: planificador donde eliminar el proceso
 * =====================================================================================
 */
void salida_proceso(Planificador* plan) {
	plan->proceso_ejecucion->estado_actual = TERMINADO; 
	plan->proceso_ejecucion = NULL;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  activar_proceso
 *  Description:  Activa un proceso que se encuentra en estado listo, pasando a ejecución
 *  	   Args:  plan: planificador donde activar el proceso
 * Return value:  Estado del planificador tras la activación del proceso, STOP si no hay
 * 				  más procesos que ejecutar, o CONTINUE en caso contrario
 * =====================================================================================
 */
Estado_planificador activar_proceso(Planificador* plan) {
	plan->quantum_actual = 0;
	// terminamos si no hay procesos ni en cola ni bloqueados
	if (todas_colas_vacias(plan) && !hay_procesos_bloqueados(plan) && todos_semaforos_vacios(plan) && cola_vacia(&procesos_entrada)) {
		return STOP;
	}
	// si hay alguno listo lo activamos
	if (!todas_colas_vacias(plan)) {
		switch (plan->conf.alg) {
			case FIFO:
				case RR:
					plan->proceso_ejecucion = extraer_proceso(&plan->cola_listos[0]);
					break;
			
			case SJF:
				plan->proceso_ejecucion = extraer_proceso_SJF(&plan->cola_listos[0]);
				break;
			case CR:
				plan->proceso_ejecucion = extraer_proceso_CR(plan);
				break;
		}
		plan->proceso_ejecucion->estado_actual = EJECUTANDO;
	}
	return CONTINUE;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  extraer_proceso_SJF
 *  Description:  Extrae el proceso con mayor prioridad de la cola siguiendo el algoritmo SJF
 *  	   Args:  cola: cola de donde extraer el proceso
 * Return value:  Proceso extraido de la cola.
 * =====================================================================================
 */
Proceso * extraer_proceso_SJF(Cola *cola) {
	Nodo *n = cola->primero;
	Nodo *n_minimo;
	int trabajo_minimo = INT_MAX;
	do {
		Proceso *p = (Proceso *)(n->dato);
		int trabajo = pasos_restantes(p);
		if (trabajo < trabajo_minimo) {
			trabajo_minimo = trabajo;
			n_minimo = n;
		}
		n = n->anterior;
	} while (n);

	Proceso *p = (Proceso *)(n_minimo->dato);
	if (n_minimo->anterior) {
		n_minimo->anterior->siguiente = n_minimo->siguiente;
	} else {
		cola->ultimo = n_minimo->siguiente;
	}

	if (n_minimo->siguiente) {
		n_minimo->siguiente->anterior = n_minimo->anterior;
	} else {
		cola->primero = n_minimo->anterior;
	}
	cola->tam--;
	free(n_minimo);
	return p;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  extraer_proceso_CR
 *  Description:  Extrae el proceso con mayor prioridad de la cola siguiendo el algoritmo CR
 *  	   Args:  plan: planificador donde extraer el proceso
 * Return value:  Proceso extraido de la cola.
 * =====================================================================================
 */
Proceso *extraer_proceso_CR(Planificador * plan) {
	for (int i = plan->conf.n_colas - 1; i >= 0; i--) {
		if (!cola_vacia(&plan->cola_listos[i])) {
			return extraer_proceso(&plan->cola_listos[i]);
		}
	}
	return NULL;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  todas_colas_vacias
 *  Description:  Comprueba si todas las colas de listos del planificador están vacías
 *  	   Args:  plan: planificador a comprobar
 * Return value:  1 si todas las colas están vacías, 0 en caso contrario
 * =====================================================================================
 */
int todas_colas_vacias(Planificador *plan) {
	for (int i = 0; i < plan->conf.n_colas; i++) {
		if (!cola_vacia(&plan->cola_listos[i])) {
			return 0;
		}
	}
	return 1;
}

int todos_semaforos_vacios(Planificador *plan) {
	for (int i = 0; i < plan->conf.n_semaforos; i++) {
		if (!cola_vacia(&plan->semaforos[i]->cola_procesos)) {
			return 0;
		}
	}
	return 1;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  detener_proceso
 *  Description:  expulsa a un proceso del procesador, pasando a estado listo.
 *	 	   Args:  plan: planificador a destruir
 * =====================================================================================
 */
void detener_proceso(Planificador* plan) {
	insertar_proceso(&procesos_entrada, plan->proceso_ejecucion);
	plan->quantum_actual = 0;
	plan->proceso_ejecucion = NULL;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  bloquear_proceso
 *  Description:  Bloquea un proceso, que se mete en la cola de bloqueados del dispositivo
 * 				  asociado a la instrucción que le toca ejecutar.
 *	 	   Args:  p: proceso a bloquear
 * =====================================================================================
 */
void bloquear_proceso(Proceso *p) {
	p->estado_actual = BLOQUEADO;
	insertar_proceso(&(((Dispositivo *) p->codigo[p->contador_programa[0]].objetivo)->cola_bloqueados), p);
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  despertar_proceso
 *  Description:  Despierta un proceso bloqueado, que se mete en la cola de listos del planificador
 *	 	   Args:  plan: planificador en el que despertar el proceso
 *  			  p: proceso a despertar
 * =====================================================================================
 */
void despertar_proceso(Proceso *p) {
	insertar_proceso(&procesos_entrada, p);
}

/*
 * ===  FUNCTION  ======================================================================
 *	 	   Name:  hay_procesos_bloqueados
 *	Description:  Comprueba si hay algún proceso bloqueado en el planificador
 *	 	   Args:  plan: planificador en el que buscar procesos bloqueados
 * Return value:  1 si hay procesos bloqueados, 0 en caso contrario
 * =====================================================================================
 */
int hay_procesos_bloqueados(Planificador* plan) {
	for (int i = 0; i < plan->conf.n_dispositivos; i++) {
		if (!cola_vacia(&(plan->dispositivos[i]->cola_bloqueados))) {
			return 1;
		}
	}
	return 0;
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  simular_planificador
 *  Description:  Simula la totalidad del planificador, parando cuando se alcanza el
 * 				  número máximo de ciclos indicado o cuando no hay más procesos que ejecutar
 *  	   Args:  plan: planificador a simular
 * =====================================================================================
 */
void simular_planificador(Planificador* plan) {
	int i = -1;	
	if (activar_proceso(plan) == STOP) {
		printf("No hay ningún proceso\n");
		return;
	}

	while (++i < plan->conf.max_instrucciones && ejecutar_ciclo(plan, i) == CONTINUE);

	// terminamos de rellenar las trazas de los 
	// procesos hasta el tamaño indicado
	while(++i < plan->conf.max_instrucciones) {
		guardar_estados(plan, i);
	}
}	



/*
 * ===  FUNCTION  ======================================================================
 *         Name:  ejecutar_ciclo
 *  Description:  Ejecuta un ciclo del planificador, actualizando los estados de los
 * 				  procesos y guardando la traza de estados de los procesos
 *  	   Args:  plan: planificador a simular
 *  			  instante: instante de tiempo actual
 * Return value:  Estado del planificador tras la ejecución del ciclo (STOP o CONTINUE),
 * =====================================================================================
 */
Estado_planificador ejecutar_ciclo(Planificador* plan, int instante) {
	// llegada de nuevos procesos
	for (int p = 0; p < plan->n_proc; p++) {
		if (instante > 0 && plan->lista_procesos[p]->t_llegada == instante) {
			insertar_proceso(&procesos_entrada, plan->lista_procesos[p]);
		}
	}

	calcular_llegadas(plan);

	Proceso *p = plan->proceso_ejecucion;

	Estado_planificador estado = CONTINUE;
	if (!p) {
		estado = activar_proceso(plan);
	}

	guardar_estados(plan, instante);


	
	p = plan->proceso_ejecucion;

	avanzar_bloqueos(plan);

	if (!p) {
		return estado;
	}

	plan->quantum_actual++;
	p->contador_programa[1]++;
	estado = CONTINUE;


	if (p->contador_programa[1] == p->codigo[p->contador_programa[0]].duracion) { // Si se ha terminado la instrucción actual
		if (p->codigo[p->contador_programa[0]].tipo == WAIT) { // si era un wait
			if (wait(p->codigo[p->contador_programa[0]].objetivo, p) == 1) { // si se bloquea
				plan->proceso_ejecucion = NULL;
				return activar_proceso(plan);
			}
		} else if (p->codigo[p->contador_programa[0]].tipo == SIGNAL) { // si era un signal
			Proceso *extraido = signal(p->codigo[p->contador_programa[0]].objetivo);
			if (extraido) { // si se extrae algún proceso
				if (plan->conf.alg == CR) {
					incrementar_prioridad(extraido, plan->conf.incremento_semaforos, plan->conf.n_colas - 1);
				}
				entrada_proceso(plan, extraido);
			}
		}	
		estado = avanzar_proceso(plan, p);	
	}
	if (plan->quantum_actual == plan->conf.quantum) { // Si se ha agotado el quantum
		if (p->prioridad > 0) {
			p->prioridad -= 1;
		}
		detener_proceso(plan);
		estado = activar_proceso(plan);
	}
	return estado;
}



/*
 * ===  FUNCTION  ======================================================================
 *         Name:  avanzar_proceso
 *  Description:  Se ejecuta cuando un proceso termina una instrucción de CPU sin agotar
 * 				  su quantum, si ha terminado sus instrucciones abandona el planificador,
 * 				  si la siguiente instrucción es de ES se bloquea, si es de CPU sigue ejecutando.
 *  	   Args:  plan: planificador a simular
 *  			  p: proceso a avanzar
 * Return value:  Estado del planificador tras la ejecución del ciclo (STOP o CONTINUE),
 * =====================================================================================
 */
Estado_planificador avanzar_proceso(Planificador* plan, Proceso *p) {

	p->contador_programa[0]++;
	p->contador_programa[1] = 0;
	Estado_planificador estado = CONTINUE;
	if (p->contador_programa[0] == p->n_instrucciones) { // Si no quedan instrucciones
			salida_proceso(plan);
			estado = activar_proceso(plan);
	} 
	else if (p->codigo[p->contador_programa[0]].tipo == ES) { // Si la siguiente instrucción es de ES
		plan->proceso_ejecucion = NULL;
		bloquear_proceso(p);
		estado = activar_proceso(plan);
	}
	return estado;
}
/**
 * ===  FUNCTION  ======================================================================
 * 	   	  Name:  avanzar_bloqueos
 * Description:  Avanza el contador de programa de los procesos bloqueados, despertando
 * 		         a los que han terminado su tiempo de bloqueo.
 *        Args:  plan: planificador a simular
 * =====================================================================================
*/
void avanzar_bloqueos(Planificador *plan) {
	for (int i = 0; i < plan->conf.n_dispositivos; i++) {
		Dispositivo *d = plan->dispositivos[i];
		if (!cola_vacia(&d->cola_bloqueados)) {
			Proceso *p = cima_cola(&(d->cola_bloqueados));
			p->contador_programa[1]++;
			if (p->contador_programa[1] == p->codigo[p->contador_programa[0]].duracion) {
				if (plan->conf.alg == CR) {
					incrementar_prioridad(p, d->incremento_por_bloqueo, plan->conf.n_colas - 1);
				}				
				p->contador_programa[0]++;
				p->contador_programa[1] = 0;
				despertar_proceso(extraer_proceso(&d->cola_bloqueados));
			}
		}
	}
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  guardar_estados
 *  Description:  Guarda el estado de los procesos y los dispositivos en un instante 
 * 				  de tiempo concreto.
 *  	   Args:  planificador: planificador a simular
 *  			  instante: instante de tiempo actual
 * =====================================================================================
 */
void guardar_estados(Planificador *planificador, int instante) {
	for (int i = 0; i < planificador->n_proc; i++) {
		Proceso* p = planificador->lista_procesos[i];
		p->traza_estados[instante] = p->estado_actual;
		p->traza_prioridades[instante] = p->prioridad;
	}
	int mostrado = 0;
	
	for (int i = 0; i < planificador->conf.n_dispositivos; i++) {
		Dispositivo* d = planificador->dispositivos[i];
		Proceso *p = cima_cola(&(d->cola_bloqueados));
		if (p) {
			d->traza_procesos[instante] = p->pid;
		} else {
			d->traza_procesos[instante] = -1;
		}
		if (d->cola_bloqueados.tam > 1) {
			if (mostrado == 0) {
				mostrado = 1;
				printf("Colas en instante t=%d:\n", instante);
			}
			Nodo* nodo = d->cola_bloqueados.primero;
			printf("Dispositivo %s: %d", d->nombre, ((Proceso * ) nodo->dato)->pid);
			nodo = nodo->anterior;
			while (nodo) {
				printf(" <- %d", ((Proceso * ) nodo->dato)->pid);
				nodo = nodo->anterior;
			}
			printf("\n");
		}
	}
	for (int i = 0; i < planificador->conf.n_semaforos; i++) {
		Semaforo* s = planificador->semaforos[i];
		s->traza_valor[instante] = s->valor_actual;
		if (s->cola_procesos.tam > 0) {
			if (mostrado == 0) {
				mostrado = 1;
				printf("Colas en instante t=%d:\n", instante);
			}
			Nodo* nodo = s->cola_procesos.primero;
			printf("Semáforo %s: %d", s->nombre, ((Proceso * ) nodo->dato)->pid);
			nodo = nodo->anterior;
			while (nodo) {
				printf(" <- %d", ((Proceso * ) nodo->dato)->pid);
				nodo = nodo->anterior;
			}
			printf("\n");
		}
	}
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  liberar_memoria
 *  Description:  Libera la memoria reservada para el planificador y sus procesos asociados
 *  	   Args:  plan: planificador a destruir
 * =====================================================================================
 */
void liberar_memoria(Planificador* plan) {
	for (int i = 0; i < plan->n_proc; i++) {
		free(plan->lista_procesos[i]->traza_prioridades);
		free(plan->lista_procesos[i]->traza_estados);
		free(plan->lista_procesos[i]->codigo);
		free(plan->lista_procesos[i]);
	}
	free(plan->lista_procesos);
	for (int i = 0; i < plan->conf.n_dispositivos; i++) {
		free(plan->dispositivos[i]->traza_procesos);
		free(plan->dispositivos[i]->nombre);
		free(plan->dispositivos[i]);
	}
	for (int i = 0; i < plan->conf.n_semaforos; i++) {
		free(plan->semaforos[i]->traza_valor);
		free(plan->semaforos[i]->nombre);
		free(plan->semaforos[i]);
	}
	free(plan->dispositivos);
	free(plan->semaforos);
	free(plan->cola_listos);
	free(plan);
}



/*
 * ===  FUNCTION  ======================================================================
 *         Name:  pasos_restantes
 *  Description:  Devuelve el numero de instrucciones que le quedan por ejecutar al proceso
 * 				  actual.
 *  	   Args:  p: proceso del que se calculan las instrucciones restantes
 * Return Value:  número de isntrucciones restantes. 0 si el proceso no existe.
 * =====================================================================================
 */
int pasos_restantes(Proceso *p) {
	if (!p) {
		return 0;
	}
	return p->codigo[p->contador_programa[0]].duracion - p->contador_programa[1];
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  incrementar_prioridad
 * Description:  Incrementa la prioridad de un proceso en un valor dado, sin sobrepasar el máximo
 * 	   Args:  p: proceso al que incrementar la prioridad
 * 			  valor: valor a incrementar
 * 			  max: valor máximo que puede alcanzar la prioridad
 * =====================================================================================
 */
void incrementar_prioridad(Proceso *p, int valor, int max) {
	p->prioridad += valor;
	p->prioridad = MIN(p->prioridad, max);
}

