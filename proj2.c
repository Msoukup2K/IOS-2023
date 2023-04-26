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

#define rand_service() ((rand() % 3) + 1)

//Declare shared resources and semaphores
static int  *process_c = NULL, *first_queue = NULL, *second_queue = NULL, *third_queue = NULL,
			*customers_done_c = NULL, *post_closed = NULL;

sem_t *mutex, *init_C, *init_W, *barber_sem, *customer_sem, *customer_done, *barber_done, *writer;

FILE *f;

/**
 * Function for argument parsing, parse and checks arguments and stores them in structure 
 * @param int count of arguments
 * @param char* array of arguments
 * @param args* argument structure
 * @return void
 */
void arg_parse( int argcount, char *argval[] ,args *arguments)
{
	//declare error variable for catching strtol error
	char *error = NULL;

	//check if count of arguments is right
	if( argcount != 6 )
	{
		fprintf(stderr, "Wrong number of arguments");
		exit(1);
	}

	//check arguments if they are digits and in the right interval
	arguments->NZ = strtol(argval[1],&error, 10);
	if (*error != 0)
	{
		fprintf(stderr, "Wrong type of parameters.");
		exit(1);
	}
	arguments->NU = strtol(argval[2],&error, 10);
	if (*error != 0)
	{
		fprintf(stderr, "Wrong type of parameters.");
		exit(1);
	}
	arguments->TZ = strtol(argval[3],&error, 10);
	if (*error != 0)
	{
		fprintf(stderr, "Wrong type of parameters.");
		exit(1);
	}
	arguments->TU = strtol(argval[4],&error, 10);
	if (*error != 0)
	{
		fprintf(stderr, "Wrong type of parameters.");
		exit(1);
	}
	arguments->F = strtol(argval[5],&error, 10);
	if (*error != 0)
	{
		fprintf(stderr, "Wrong type of parameters.");
		exit(1);
	}

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

/**
 * Function for mapping shared resources
 * @return void
 */
void map_resources()
{
	//Mapping shared counters
	post_closed = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	process_c = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	first_queue = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1 , 0);
	second_queue = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1 , 0);
	third_queue = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1 , 0);

	if( process_c == MAP_FAILED || first_queue == MAP_FAILED || second_queue == MAP_FAILED || third_queue == MAP_FAILED 
		|| post_closed == MAP_FAILED )
	{
		fprintf(stderr, "One or more resources couldn't be mapped");
		exit(1);
	}

	//Mapping semaphores
	mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	init_C = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	init_W = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	customer_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	barber_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	customer_done = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	barber_done = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	writer = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

	if( mutex == MAP_FAILED || init_C == MAP_FAILED || init_W == MAP_FAILED || customer_sem == MAP_FAILED 
		|| barber_sem == MAP_FAILED || customer_done == MAP_FAILED || barber_done == MAP_FAILED || writer == MAP_FAILED )
	{
		fprintf(stderr, "One or more semaphores couldn't be mapped");
		exit(1);
	}
}

void terminate()
{
	//Closing file
	fclose(f);

	//Destroying all semaphores
	if( sem_destroy( mutex ) == -1 || sem_destroy( customer_sem ) || sem_destroy( barber_sem )
		|| sem_destroy( init_C ) || sem_destroy( init_W ) || sem_destroy( customer_done ) 
		|| sem_destroy( barber_done ) || sem_destroy( writer ) )
	{
		fprintf(stderr, "Cannot destroy all semaphores");
		exit(1);
	}

	//Unmapping shared counters and shared semaphores
	if(	munmap( mutex, sizeof(sem_t)) < 0 || munmap( init_C, sizeof(sem_t)) < 0 || munmap( init_C, sizeof(sem_t)) < 0
		|| 	munmap( init_W, sizeof(sem_t)) < 0 || munmap( customer_sem, sizeof(sem_t)) < 0 || munmap( barber_sem, sizeof(sem_t)) 
		|| 	munmap( customer_done, sizeof(sem_t)) || munmap( barber_done, sizeof(sem_t)) || munmap( post_closed, sizeof(int) ) || munmap( process_c, sizeof(int)) 
		|| munmap(first_queue, sizeof(int)) || munmap(second_queue, sizeof(int)) || munmap(third_queue, sizeof(int)) )
	{
		fprintf(stderr, "Cannot unmap all shared resources");
		exit(1);
	}

}

/**
 * Customer function
 * @param FILE output file
 * @param int number of a customer
 * @param int Time for customer sleep
 */
void customer_function( FILE *f, int customer_number, int TZ)
{

	//Randomize sleep time
	int c_time = ( random() % (TZ+1) );
	usleep( c_time * 1000 );

	if( *post_closed == 1 )
	{
		return;
	}

	//Using macro for service choose
	int service = rand_service();

	//Printing enter
	sem_wait(writer);
	fprintf(f, "%d:\t Z %d:\tentering office for a service %d\n", ++(*process_c), customer_number, service);
	sem_post(writer);

	//Critical section
	sem_wait(mutex);

	if( service == 1 )
	{
		(*first_queue)++;

	}
	else if( service == 2 )
	{
		(*second_queue)++;
	} 
	else
	{
		(*third_queue)++;
	}

	sem_post(mutex);

	// waiting for a worker
	sem_post(customer_sem);
	sem_wait(barber_sem);

	// printing servicing and sleep
	sem_wait(writer);
	fprintf(f, "%d:\t Z %d:\tcalled by office worker\n", ++(*process_c), customer_number);
	sem_post(writer);

	usleep( (random() % (10+1)) * 1000 );

	sem_post(customer_done);
	sem_wait(barber_done);

	
}

/**
 * Function for workers
 * @param FILE File for output
 * @param int Worker number
 * @param int Sleeping time for worker
 * @return void
 */
void worker_function( FILE *f, int worker_number, int TU)
{
	while(1)
	{
		//Critical section for first service
		if( *first_queue > 0 )
		{
			int service = 1;

			sem_wait(mutex);
			(*first_queue)--;
			sem_post(mutex);
			
			sem_wait(writer);
			fprintf(f, "%d:\t U %d:\tserving a service of type %d\n", ++(*process_c), worker_number, service);
			sem_post(writer);

			sem_wait(customer_sem);
			sem_post(barber_sem);


			usleep( (random() % (10+1)) * 1000 );

			sem_wait(writer);
            fprintf(f, "%d:\t U %d:\tservice finished\n", ++(*process_c), worker_number);
            sem_post(writer);

			sem_wait(customer_done);
			sem_post(barber_done);

		}
		//Critical section for second service
		else if( *second_queue > 0 )
		{
			int service = 2;

			sem_wait(mutex);
			(*second_queue)--;
			sem_post(mutex);

			sem_wait(writer);
			fprintf(f, "%d:\t U %d:\tserving a service of type %d\n", ++(*process_c), worker_number, service);
			sem_post(writer);

			sem_wait(customer_sem);
			sem_post(barber_sem);

			usleep( (random() % (10+1)) * 1000 );

			sem_wait(writer);
            fprintf(f, "%d:\t U %d:\tservice finished\n", ++(*process_c), worker_number);
            sem_post(writer);

			sem_wait(customer_done);
			sem_post(barber_done);
		}
		//Critical section for a third service
		else if( *third_queue > 0 )
		{
			int service = 3;

			sem_wait(mutex);
			(*third_queue)--;
			sem_post(mutex);

			sem_wait(writer);
			fprintf(f, "%d:\t U %d:\tserving a service of type %d\n", ++(*process_c), worker_number, service);
			sem_post(writer);

			sem_wait(customer_sem);
			sem_post(barber_sem);

			usleep( (random() % (10+1)) * 1000 );

			sem_wait(writer);
            fprintf(f, "%d:\t U %d:\tservice finished\n", ++(*process_c), worker_number);
            sem_post(writer);

			sem_wait(customer_done);
			sem_post(barber_done);
		}
		//Section if post is closed
		else if( *post_closed == 1 )
		{
			sem_wait(writer);			
			fprintf(f, "%d:\t U %d:\tgoing home\n", ++(*process_c), worker_number);
			sem_post(writer);
			exit(0);
		}
		//Section if there`s no customers in queue
		else
		{
			sem_post(mutex);
			sem_wait(writer);
			fprintf(f, "%d:\t U %d:\ttaking break\n", ++(*process_c), worker_number);
			sem_post(writer);

			usleep( (random() % (TU + 1)) * 1000 );

			sem_wait(writer);
			fprintf(f, "%d:\t U %d:\tbreak finished\n", ++(*process_c), worker_number);
			sem_post(writer);
		}
	}
}

/**
 * Main function of a program 
 * @param argc count of arguments
 * @param argv array of arguments
 * 
*/
int main( int argc, char *argv[] )
{

	args arguments;

	//Calling argument function for storing in args structure
	arg_parse( argc, argv, &arguments);

	//Calling mapping function
	map_resources();

	f = fopen( "proj2.out", "wb" );
	//Check if file is correctly setup
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

	//Arrays for pids of customers and workers
	pid_t *customer_pids = malloc( sizeof(pid_t) * arguments.NZ);
	for( int i = 0; i < arguments.NZ; i++)
		customer_pids[i] = 0;
	
	pid_t *worker_pids = malloc( sizeof(pid_t) * arguments.NU);
	for( int i = 0; i < arguments.NU; i++)
		worker_pids[i] = 0;

	//Numbers of each workers and customers
	int c_num = 0;
	int w_num = 0;

	//Number of process Main(0) others are not zero
	int process_number = 0;
	
	//Initializing semaphores, for Mutex and initializers are set to one, writer is set to two
	sem_init(mutex, 1,1);
	sem_init(init_C, 1, 1);
	sem_init(init_W, 1, 1);
	sem_init(customer_sem, 1, 0);
	sem_init(barber_sem, 1, 0);
	sem_init(customer_done, 1,0);
	sem_init(barber_done, 1, 0);
	sem_init(writer,1,2);

	//Main process starts creating Customers
	if( process_number == 0 )
	{
		for( int i = 0; i < arguments.NZ; ++i)
		{
			if((customer_pids[i] = fork()) < 0)
			{
				fprintf(f, "%d:\tVytvoření procesu Zákazníka se nezdařilo\n", ++(*process_c));
				arguments.NZ--;
			}
			//Every process has its own process number which can be from 1 to Maximum number of customers
			if( customer_pids[i] == 0)
			{
				c_num = i + 1;
				process_number = c_num;

				sem_wait(init_C);
				sem_wait(writer);
				fprintf(f, "%d:\t Z %d:\tstarted\n", ++(*process_c), c_num);
				sem_post(writer);
				sem_post(init_C);

				//every process has own seed
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
				fprintf(f, "%d:\tVytvoření procesu Úředníka se nezdařilo\n", ++(*process_c));
				arguments.NU--;
			}
			//Every process has its own process number which can be from maximum number of customers to max. of customers + max. of workers
			if( worker_pids[i] == 0)
			{
				w_num = i + 1;
				process_number = arguments.NZ + w_num;

				sem_wait(init_W);
				sem_wait(writer);
				fprintf(f, "%d:\t U %d:\tstarted\n", ++(*process_c), w_num);
				sem_post(writer);
				sem_post(init_W);

				//every process has own seed
				free(customer_pids);
				free(worker_pids);
				srand(time(NULL) / getpid());
				break;

			}
		}
	}

	//Main process starts counting the time till closing
	if( process_number == 0 )
	{
		srand(time(NULL));
		int sleep_time = random() % (arguments.F/2 + 1) + arguments.F/2;
		usleep(sleep_time * 1000);
		(*post_closed) = 1;
		fprintf(f, "%d:\t closing\n", ++(*process_c));
	}

	while( wait(NULL) > -1);

	//Thread which are not Main are getting into their functions
	if( process_number != 0 )
	{
		while(1)
		{
			//From interval <1,NZ+1>
			if ( process_number < arguments.NZ + 1 )
			{
				//Calls for customer function
				customer_function(f, c_num, arguments.TZ);

				//After customer function has ended, customer is going home and process is exiting
				sem_wait(writer);
				fprintf(f, "%d:\t Z %d:\tgoing home\n", ++(*process_c), c_num);
				sem_post(writer);

				exit(0);
			}
			//From interval <NZ, NU+NZ>
			else if( process_number > arguments.NZ && process_number < arguments.NU + arguments.NZ + 1 )
			{
				//Worker is exiting in the function
				worker_function(f , w_num, arguments.TU);
			}

		}
	}
	//Terminating after all customer and worker processes are exited
	free(customer_pids);
	free(worker_pids);
	terminate();
	return 0;
}
