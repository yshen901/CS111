/*
    NAME: Po-Chun Yang
    EMAIL: a29831968@g.ucla.edu
    ID: 605297984
    SLIPDAYS: 0
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>		// for using string
#include <getopt.h>     // for getopt_long()
#include <time.h>		// for clock_gettime()
#include <pthread.h>	// for multithread implementation
#include <errno.h>

#define ONE_BILLION 1000000000L;

// initialize the long long counter to 0
long long counter = 0;

// flag for options
int iter_flag;
int thread_flag;
int opt_yield = 0;
int sync_flag;

// iter times, thread numbers
int iter_times = 1;
int thread_nums = 1;
int lock = 0;

// lock type 
int NONE = 0;
int MUTEX = 1;
int SPIN_LOCK = 2;
int COMPARE_AND_SWAP = 3;


// mutex, spin_lock
pthread_mutex_t mutex;
long long spinLock = 0;

// test name
char * test = "add-none";
void get_test_name() {
	if (opt_yield) {
		if (lock == MUTEX) {
			test = "add-yield-m";
		}else if (lock == SPIN_LOCK){
			test = "add-yield-s";
		}else if (lock == COMPARE_AND_SWAP) {
			test = "add-yield-c";
		}else {
			test = "add-yield-none";
		}
	}else {
		if (lock == MUTEX) {
			test = "add-m";
		}else if (lock == SPIN_LOCK){
			test = "add-s";
		}else if (lock == COMPARE_AND_SWAP) {
			test = "add-c";
		}
	}
}

// detroy mutex right before exit
void destroyMutex(){
    if(lock == MUTEX){
        pthread_mutex_destroy(&mutex);
    }
}

// race condition
void add (long long *pointer, long long value) {
	long long sum = *pointer + value;
	if (opt_yield) {sched_yield();}
	*pointer = sum;
}



// mutex, spin_lock, compare_and_swap, none
void add_multiple_way (int val) {
	for (int i = 0; i < iter_times; i++) {
		if (lock == MUTEX) {
			// mutex
			pthread_mutex_lock(&mutex);
			add(&counter, val);
			pthread_mutex_unlock(&mutex);

		}else if (lock == SPIN_LOCK) {
			// spin lock 
			while(__sync_lock_test_and_set(&spinLock, 1));
	        add(&counter, val);
	        __sync_lock_release(&spinLock);

		}else if (lock == COMPARE_AND_SWAP) {
			// compare and swap
			long long oldVal, newVal;
	        do{
	            oldVal = counter;
	            newVal = oldVal + val;
	            if (opt_yield) {sched_yield();}
	        } while(__sync_val_compare_and_swap(&counter, oldVal, newVal) != oldVal);

		}else {
			// none lock
			add(&counter, val);
		}
	}
}

void add_sub() {
	add_multiple_way(1);
	add_multiple_way(-1);
}





int main(int argc, char **argv) {

	// options declaration
	static struct option long_options[] = {
		{"iterations", required_argument, &iter_flag, 1},
		{"threads", required_argument, &thread_flag, 1},
		{"yield", no_argument, &opt_yield, 1},
		{"sync", required_argument, &sync_flag, 1},
		{0, 0, 0, 0}
	};

	int opt_index;
	int c;
	while (1) {
		c = getopt_long(argc, argv, "", long_options, &opt_index);
		if (c == -1) break;
		if (c != 0) {
			fprintf(stderr, "Error : Invalid argument.\n");
			exit(1);
		}
		if (opt_index == 0) {
			iter_times = atoi(optarg);
		}
		if (opt_index == 1) {
			thread_nums = atoi(optarg);
		}
		if (opt_index == 2) {
			opt_yield = 1;
		}
		if (opt_index == 3) {
			if (strlen(optarg) != 1) {
				fprintf(stderr, "Error : Invalid option. Sync argument must be one letter\n");
                exit(1);
			}
			switch(optarg[0]) {
				case 'm':	 // Mutex
					lock = MUTEX;
					break;
				case 's':
					lock = SPIN_LOCK; // spin lock
					break;
				case 'c':
					lock = COMPARE_AND_SWAP; // compare and swap
					break;
				default:
					fprintf(stderr, "Error: sync argument invalid.\n");
					exit(1);
			}
		}
	}
	// debug
	// printf("iter : %d, thread : %d, yield: %d, lock : %d\n", iter_times, thread_nums, opt_yield, lock);

	// timer start
	
	struct timespec start_time, end_time;
    
    if(clock_gettime(CLOCK_MONOTONIC, &start_time) < 0){
    	fprintf(stderr, "Error: Getting start time error.\n");
    	exit(1);
    }

    // mutex initialize
    if (lock == MUTEX) {
    	if (pthread_mutex_init(&mutex, NULL) != 0) { 
    		fprintf(stderr, "Error : mutex initialize failed.\n");
    		exit(1);
	    } 
    }

    // pthread implementation
    pthread_t tid[thread_nums];

    for (int i = 0; i < thread_nums; i++) {
    	if (pthread_create(&tid[i], NULL, (void*)&add_sub, &counter) < 0) {
    		fprintf(stderr, "Error : pthread : %d create error.\n", i);
    		exit(1);
    	}
    }
    for (int i = 0; i < thread_nums; i++) {
    	if (pthread_join(tid[i], NULL) < 0) {
    		fprintf(stderr, "Error : pthread : %d join error.\n", i);
    		exit(1);
    	}
    }

	
    // timer end
    if (clock_gettime(CLOCK_MONOTONIC, &end_time) < 0){
    	fprintf(stderr, "Error: Getting end time error.\n");
    	exit(1);
    }

    long long time_total = (end_time.tv_sec - start_time.tv_sec) * ONE_BILLION;
    time_total += (end_time.tv_nsec - start_time.tv_nsec);
	int operation_num = iter_times * thread_nums * 2;
	get_test_name();
	printf("%s,%d,%d,%d,%lld,%lld,%lld\n", test, thread_nums, iter_times, operation_num, time_total, time_total / operation_num, counter);
	atexit(destroyMutex);
	exit(0);
}
