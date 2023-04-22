/**
 * @file proj2.c
 * @author Martin Soukup
 * @brief 2.projekt předmětu operační systémy
 * @date 2023-04-21
 * 
 * 
 */

#ifndef PROJ2_H
#define PROJ2_H

#include <sys/types.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>


typedef struct Arguments
{
	int NZ;
	int NU;
	int TZ;
	int TU;
	int F;
}args;

void arg_parse( int argc, char* argv[], args* args);

void map_resources();

#endif

