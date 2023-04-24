/**
 * @file proj2.c
 * @author Martin Soukup
 * @brief 2.projekt předmětu operační systémy
 * @version 0.1
 * @date 2023-04-19
 * 
 * 
 */
#include "proj2.h"


static int *customer_c = NULL, *worker_c = NULL, *process_c = NULL,
			*first_queue = NULL, *second_queue = NULL, *third_queue = NULL,
			*customers_done_c = NULL;

sem_t *mutex, *init_C, *init_W, *barber_sem, *customer_sem, *customer_done, *barber_done,
	*writer;

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
	first_queue = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1 , 0);
	second_queue = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1 , 0);
	third_queue = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1 , 0);
	customers_done_c = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if( customer_c == MAP_FAILED || worker_c == MAP_FAILED || process_c == MAP_FAILED ||
	 first_queue == MAP_FAILED || second_queue == MAP_FAILED || third_queue == MAP_FAILED 
	 || customers_done_c == MAP_FAILED )
	{
		fprintf(stderr, "One or more resources couldn't be mapped");
		exit(1);
	}

	mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	init_C = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	init_W = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	customer_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	barber_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	customer_done = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	barber_done = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	writer = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

void terminate()
{
	fclose(f);
	if( sem_destroy( mutex ) == -1 || sem_destroy( customer_sem ) || sem_destroy( barber_sem ) )
	{
		fprintf(stderr, "Cannot destroy all semaphores");
		exit(1);
	}

	munmap( mutex, sizeof(sem_t));
	munmap( init_C, sizeof(sem_t));
	munmap( init_W, sizeof(sem_t));
	munmap( customer_sem, sizeof(sem_t));
	munmap( barber_sem, sizeof(sem_t));
	munmap( customer_done, sizeof(sem_t));
	munmap( barber_done, sizeof(sem_t));

}

void customer_function( FILE *f, int customer_number, int TZ, int TU)
{
	//Přidat místo jednoho counteru tři různé pro tři různé řady podle rand <1,3>
	
	int c_time = ( random() % (TZ+1) );

	usleep( c_time * 1000 );

	sem_wait(writer);
	fprintf(f, "%d:\t Z %d:\t entering office for a service X\n", ++(*process_c), customer_number);
	sem_post(writer);

	sem_wait(mutex);

	(*customer_c)++;

	sem_post(mutex);

	sem_post(customer_sem);
	sem_wait(barber_sem);

	//obsluhování
	sem_wait(writer);
	fprintf(f, "%d:\t Z %d:\t called by office worker\n", ++(*process_c), customer_number);
	sem_post(writer);

	sem_post(customer_done);
	sem_wait(barber_done);

	sem_wait(mutex);
	(*customer_c)--;
	sem_post(mutex);
	
}

void worker_function( FILE *f, int worker_number, int TI, int TB)
{

	//přišel
	sem_wait(writer);
	fprintf(f, "%d:\t U %d:\t serving service of type X\n", ++(*process_c), worker_number);
	sem_post(writer);

	if( *customer_c == 0 )
	{

	}
	//Pokud aspoň jedna řada je neprázdná, jde obsloužit jednu řadu
	sem_wait(customer_sem);
	sem_wait(barber_sem);

	sem_wait(customer_done);
	sem_post(barber_done);

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

	pid_t *customer_pids = malloc( sizeof(pid_t) * arguments.NZ);
	for( int i = 0; i < arguments.NZ; i++)
		customer_pids[i] = 0;
	
	pid_t *worker_pids = malloc( sizeof(pid_t) * arguments.NU);
	for( int i = 0; i < arguments.NU; i++)
		worker_pids[i] = 0;

	int c_num = 0;
	int w_num = 0;
	(*customer_c) = arguments.NZ;
	(*worker_c) = arguments.NU;

	int process_number = 0;
	
	sem_init(mutex, 1,1);
	sem_init(init_C, 1, 0);
	sem_init(init_W, 1, 0);
	sem_init(customer_sem, 1, 0);
	sem_init(barber_sem, 1, 0);
	sem_init(customer_done, 1,0);
	sem_init(barber_done, 1, 0);

	if( process_number == 0 )
	{
		for( int i = 0; i < arguments.NZ; ++i)
		{
			if((customer_pids[i] = fork()) < 0)
			{
				fprintf(f, "Vytvoření procesu Zákazníka se nezdařilo\n");
				arguments.NZ--;
			}

			if( customer_pids[i] == 0)
			{
				c_num = i + 1;
				process_number = c_num;

				sem_wait(init_C);
				sem_wait(writer);
				fprintf(f, "%d:\t Z %d:\t started\n", ++(*process_c), i+1);
				sem_post(writer);
				sem_post(init_C);


				free(customer_pids);
				free(worker_pids);
				srand(time(NULL) / getpid());
				break;

			}
		}
	}

	if( process_number == 0 )
	{
		for( int i = 0; i < arguments.NU; ++i)
		{
			if((worker_pids[i] = fork()) < 0)
			{
				fprintf(f, "Vytvoření procesu Úředníka se nezdařilo");
				arguments.NU--;
			}

			if( worker_pids[i] == 0)
			{
				w_num = i + 1;
				process_number = arguments.NZ + w_num;

				sem_wait(init_W);
				sem_wait(writer);
				fprintf(f, "%d:\t U %d:\t started\n", ++(*process_c), i+1);
				sem_post(writer);
				sem_post(init_W);


				free(customer_pids);
				free(worker_pids);
				srand(time(NULL) / getpid());
				break;

			}
		}
	}

	if( process_number != 0 )
	{
		while(1)
		{
			
			if ( process_number < arguments.NZ + 1 )
			{
				customer_function(f, c_num, arguments.TZ, arguments.TU);

				exit(0);
			}
			else if( process_number > arguments.NZ && process_number < arguments.NU + arguments.NZ + 1 )
			{
				worker_function(f , w_num, arguments.TZ, arguments.TU);
				
				fprintf(f, "%d:\t U :\t do funkce\n", ++(*process_c) );

				exit(0);
			}

		}
	}

	if( process_number == 0 )
	{
		srand(time(NULL) / getpid());
		int sleep_time = rand() % arguments.F + 1;
		sleep(sleep_time);
		fprintf(f, "Closed\n");
	}

	free(customer_pids);
	free(worker_pids);
	terminate();
	return 0;
}




