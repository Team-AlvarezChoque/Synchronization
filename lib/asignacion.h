#ifndef ASIGNACION_H
#define ASIGNACION_H

#include "./global.h"

int cantidadEspaciosLibres(struct sm_node * estructura){
	int i, count = 0;
	for(i = 1; i < estructura[0].owner +1; i++){
		if(estructura[i].owner == 0 && 
			estructura[i].nSegment == 0 && 
			estructura[i].nPag_pSeg == 0)
			count++;
	}
	return count;
}

// Verifica si existe una cantidad N de espacios libres contiguos
// Retorna -1 si no existe la cantidad N disponible
// Retorna M, la posición del array a partir del cual
// existe (primera ocurrencia) espacios disponibles
int cantidadEspaciosLibresContiguos(struct sm_node * estructura, int N){
	int i, count = 0;
	for(i = 1; i < estructura[0].owner +1; i++){
		if(estructura[i].owner == 0 && 
			estructura[i].nSegment == 0 && 
			estructura[i].nPag_pSeg == 0){
				count++;
				// Se encuentra la cantidad buscada
				// (espacios contiguos)
				if(count == N){
					return i-(N-1);
				}
			}
		else
			count = 0;
	}
	return -1;
}

void desasignarMemoria(struct sm_node * estructura, unsigned long PID){
	int i;
	for(i = 1; i < estructura[0].owner +1; i++){
		if(estructura[i].owner == PID){
			estructura[i].owner = 0;
			estructura[i].nSegment = 0;
			estructura[i].nPag_pSeg = 0;
		}
	}
}


#endif // ASIGNACION_H