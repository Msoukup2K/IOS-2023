/**
 * @file proj2.c
 * @author Martin Soukup
 * @brief 2.projekt předmětu operační systémy
 * @version 0.1
 * @date 2022-04-19
 * 
 * 
 */
#include "proj2.h"


static int *customer_c = NULL, *worker_c = NULL, *process_c = NULL;

sem_t *mutex, *init_C, *init_W, *barber, *customer, *customer_done, *barber_done;

FILE *f;

void arg_parse( int argcount, char *argval[] ,args *arguments)
{
	if( argcount != 6 )
	{
		fprintf(stderr, "Wrong number of arguments");
		exit(1);
	}

	arguments->NZ = strtol(argval[1], NULL, 10);
	arguments->NU = strtol(argval[2], NULL, 10);
	arguments->TZ = strtol(argval[3], NULL, 10);
	arguments->TU = strtol(argval[4], NULL, 10);
	arguments->F = strtol(argval[5], NULL, 10);

	// Check if number of clients are not zero or less
	if( arguments->NZ <= 0 )
	{
		fprintf(stderr, "Number of clients cannot be zero or less");
		exit(1);
	}

	// Check if number of office workers are not zero or less
	if( arguments->NU <= 0 )
	{
		fprintf(stderr, "Number of office workers cannot be zero or less");
		exit(1);
	}

	// Check if time of client waiting are in the right interval
	if( arguments->TZ < 0 || arguments->TZ > 10000 )
	{
		fprintf(stderr, "Time of client waiting must be in interval <0,10000> ");
		exit(1);
	}

	// Check if time of break for workers are in the right interval
	if( arguments->TU < 0 || arguments->TU > 100 )
	{
		fprintf(stderr, "Break time of workers must be in interval <0,100> ");
		exit(1);
	}

	// Check if time after the post is closing is more in the right interval
	if( arguments->F < 0 || arguments->F > 10000 )
	{
		fprintf(stderr, "Time of post open must be in interval <0,10000> ");
		exit(1);
	}

}

void map_resources()
{
	customer_c = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	worker_c = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	process_c = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if( customer_c == MAP_FAILED || worker_c == MAP_FAILED || process_c == MAP_FAILED )
	{
		fprintf(stderr, "One or more resources couldn't be mapped");
		exit(1);
	}

	mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	init_C = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	init_W = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	customer = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	barber = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	customer_done = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	barber_done = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);


}


int main( int argc, char *argv[] )
{

	args arguments;

	arg_parse( argc, argv, &arguments);

	map_resources();

	f = fopen( "proj2.out", "wb" );

	if( f == NULL )
	{
		fprintf(stderr, "Couldn't open output file");
		exit(1);
	}

	if( setvbuf( f, NULL, _IONBF, 0) != 0 )
	{
		fprintf(stderr, "Couldn't setup buffer for output file" );
		exit(1);
	}

	
	sem_init(mutex, 1,1);
	sem_init(init_C, 1, 0);
	sem_init(init_W, 1, 0);
	sem_init(customer, 1, 0);
	sem_init(barber, 1, 0);
	sem_init(customer_done, 1,0);
	sem_init(barber_done, 1, 0);

	exit(0);
   

}




