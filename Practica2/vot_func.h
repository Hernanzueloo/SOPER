/**
 * @file vot_func.h
 * @author Diego Rodríguez y Alejandro García
 * @brief 
 * @version 
 * @date 2023-03-23
 *
 */
#ifndef VOT_FUNC_H
#define VOT_FUNC_H

#include <semaphore.h>
#include <unistd.h>
#include <pthread.h>

#include "vot_utils.h"

/**
 * @brief Voting when the voter has executed an effective down
 * @author Alejandro García and Diego Rodríguez
 *
 * @return OK, if everything goes well or ERROR if there was some mistake
 */
STATUS votingCarefully();

/**
 * @brief Generates n_procs and writes their PID in a file
 * @author Alejandro García and Diego Rodríguez
 *
 * @return nothing
 */
void create_sons(int n_procs, sem_t *semV, sem_t *semC, sem_t *semCTRL);

/**
 * @brief Main function executed by the sons
 * @author Alejandro García and Diego Rodríguez
 *
 * @return nothing
 */
void voters(int n_procs, sem_t *semV, sem_t *semC, sem_t *semCTRL);

/**
 * @brief Main function executed by the son candidato
 * @author Alejandro García and Diego Rodríguez
 *
 * @return nothing
 */
void candidato(int n_procs, sem_t *semCTRL);
#endif
