/**
 * @file miner.h
 * @author Diego Rodriguez y Alejandro Garcia
 * @version 1
 * @date 2023-02-24
 *
 */

#ifndef MINER_H
#define MINER_H

#include "pow.h"

#define MAX_HILOS 100
#define MAX POW_LIMIT -1

typedef struct _entradaHash entradaHash;

/**
 * @brief Crea el proceso monitor, gestión de tuberias y llama a la función a minar
 *
 * @param nHilos numero de hilos a crear
 * @param nbusquedas numero de rondas que se haran
 * @param busq solucion a buscar
 * 
 * @return nada
 */
void minero(int nHilos,long nbusquedas, long busq);

#endif