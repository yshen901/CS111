#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include "SortedList.h"

// We now need one lock for each list
pthread_mutex_t* m_lock;
int* s_lock;

int num_iters;
int opt_yield;
int num_lists;
char opt_sync;
long long lock_time;
SortedList_t *head;
SortedListElement_t *elements;

/* HELPER FUNCTIONS */
void printCSV(int num_threads, long num_ops, long total_time, long avg_optime, long long avg_lock_time) {
  char name [50] = "list";
  if (opt_yield == 0)
    strcat(name, "-none");
  else if (opt_yield == 1)
    strcat(name, "-i");
  else if (opt_yield == 2)
    strcat(name, "-d");
  else if (opt_yield == 3)
    strcat(name, "-id");
  else if (opt_yield == 4)
    strcat(name, "-l");
  else if (opt_yield == 5)
    strcat(name, "-il");
  else if (opt_yield == 6)
    strcat(name, "-dl");
  else if (opt_yield == 7)
    strcat(name, "-idl");

  if (opt_sync == 'm')
    strcat(name, "-m");
  else if (opt_sync == 'c')
    strcat(name, "-c");
  else if (opt_sync == 's')
    strcat(name, "-s");
  else
    strcat(name, "-none");

  printf("%s,%d,%d,%d,%ld,%ld,%ld, %lld\n",
	 name, num_threads, num_iters, num_lists, num_ops, total_time, avg_optime, avg_lock_time);
}

void handle_segfault(int signum) {
  if (signum == SIGSEGV) {
    fprintf(stderr, "ERROR segmentation fault: %s\n", strerror(errno));
    exit(2); // As segfaults are due to inconsistencies
  }
}

int get_list(const char* key) {
  return key[0] % num_lists;
}

/* Protected System Calls  */
void safe_clock_gettime(struct timespec* time) {
  if(clock_gettime(CLOCK_MONOTONIC, time) != 0) {
    fprintf(stderr, "ERROR gettime failed: %s\n", strerror(errno));
    exit(1);
  }
}

/* Lock and Unlock Functions */

void lock(int list) {
  if (opt_sync == 's')
    while(__sync_lock_test_and_set(s_lock + list, 1));
  if (opt_sync == 'm')
    pthread_mutex_lock(m_lock + list);
}

void unlock(int list) {
  if (opt_sync =='s')
    __sync_lock_release(s_lock + list);
  if (opt_sync == 'm')
    pthread_mutex_unlock(m_lock + list);
}

/* THREAD FUNCTIONS */
void* thread_func(void *args) {
  int offset = *(int*)args;
  SortedListElement_t *current;
  struct timespec start_time, end_time;
  
  int length = 0;
  char key[64];
  int list = 0;

  // For list insertion: lock the list you are inserting into
  for (int i = offset; i < offset + num_iters; i++) {
    list = get_list(elements[i].key);

    safe_clock_gettime(&start_time);
    lock(list);
    safe_clock_gettime(&end_time);
    lock_time += 1000000000L * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;
    
    SortedList_insert(head + list, elements + i);
    unlock(list);
  }

  // For list length: lock all lists before checking
  for (list = 0; list < num_lists; list++) {
    safe_clock_gettime(&start_time);
    lock(list);
    safe_clock_gettime(&end_time);
    lock_time += 1000000000L * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;
  }
  for (list = 0; list < num_lists; list++)
    length += SortedList_length(head + list);
  if (length < num_iters) {
    fprintf(stderr, "ERROR items have not been inserted property and atomically\n");
    exit(2);
  }
  for (list = 0; list < num_lists; list++)
    unlock(list);

  // For deleting elements: lock the list you are searching in
  for (int i = offset; i < offset + num_iters; i++) {
    strcpy(key, elements[i].key);
    list = get_list(key);

    safe_clock_gettime(&start_time);
    lock(list);
    safe_clock_gettime(&end_time);
    lock_time += 1000000000L * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;
    
    if ((current = SortedList_lookup(head + list, key)) == NULL) {
      fprintf(stderr, "ERROR element could not be found\n");
      exit(2);
    }

    if (SortedList_delete(current) != 0) {
      fprintf(stderr, "ERROR element could not be deleted\n");
      exit(2);
    }

    unlock(list);
  }

  return NULL;
}

int main(int argc, char ** argv) {
  char c;
  int option_index = 0;
  
  int t_flag, i_flag;
  opt_sync = 'n';
  opt_yield = 0;
  num_iters = 1;

  int num_threads = 1;
  num_lists = 1;
  lock_time = 0;
  pthread_t *tid;
  
  struct timespec start_time, end_time;
  
  static struct option long_options[] = {
    {"threads", required_argument, 0, 't'},
    {"iterations", required_argument, 0, 'i'},
    {"sync", required_argument, 0, 's'},
    {"yield", required_argument, 0, 'y'},
    {"lists", required_argument, 0, 'l'},
    {0,0,0,0}
  };

  while((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
    switch(c) {
    case 't':
      t_flag = 1;
      num_threads = atoi(optarg);
      break;
    case 'i':
      i_flag = 1;
      num_iters = atoi(optarg);
      break;
    case 'l':
      num_lists = atoi(optarg);
      printf("Lists\n");
      break;
    case 'y':
      for (int i = 0; i < (int)strlen(optarg); i++) {
	if (optarg[i] == 'i')
	  opt_yield |= INSERT_YIELD;
	else if (optarg[i] == 'd')
	  opt_yield |= DELETE_YIELD;
	else if (optarg[i] == 'l')
	  opt_yield |= LOOKUP_YIELD;
	else {
	  fprintf(stderr, "ERROR unrecognized yield option, please use i/d/l\n");
	  exit(1);
	}
      }
      break;
    case 's':
      opt_sync = optarg[0]; // mutex, spin, or CAS locks
      break;
    case '?':
    default:
      fprintf(stderr, "ERROR processing arguments, please use --threads={num_threads} or --iterations={num_iterations}\n");
      exit(1);
    }
  }

  // Checks for missing required arguments
  if (t_flag != 1 || i_flag != 1) {
    fprintf(stderr, "ERROR missing arguments, --threads and --iterations are mandatory arguments\n");
    exit(1);
  }

  // Checks for invalid synchronization arguments
  if (opt_sync != 'n' && opt_sync != 'm' && opt_sync != 's') {
    fprintf(stderr, "ERROR invalid synchronization options, please use s or m\n");
    exit(1);
  }
  
  // Setup segfault handler to deal with corruptions
  signal(SIGSEGV, handle_segfault);

  // Initialize the heads of several circular doubly linked lists
  head = malloc(sizeof(SortedList_t) * num_lists);
  for (int i = 0; i < num_lists; i++) {
    head[i].next = head + i;
    head[i].prev = head + i;
    head[i].key = NULL;
  }

  // Initialize the rest of the elements and their keys
  int num_eles = num_threads * num_iters;
  char alpha[26] = "abcdefghijklmnopqrstuvwxyz";
  
  elements = (SortedListElement_t*)malloc(sizeof(SortedListElement_t) * num_eles);
  if (elements == NULL) {
    fprintf(stderr, "ERROR memory allocation failed (eles): %s\n", strerror(errno));
    exit(1);
  }

  char** keys = malloc(sizeof(char*) * num_eles);
  if (keys == NULL) {
    fprintf(stderr, "ERROR memory allocation failed (keys): %s\n", strerror(errno));
    exit(1);
  }

  srand((unsigned int) time(NULL));
  for (int i = 0; i < num_eles; i++) {
    keys[i] = malloc(sizeof(char) * 64);
    if (keys[i] == NULL) {
      fprintf(stderr, "ERROR memory allocation failed (key): %s\n", strerror(errno));
      exit(1);
    }
    for (int j = 0; j < 63; j++) // creates a random combination of 63 letters
      keys[i][j] = alpha[rand() % 26];
    keys[i][63] = '\0';
    (elements + i)->key = keys[i];
  }

  // Set aside memory for thread ids
  tid = malloc(num_threads * sizeof(pthread_t));
  if (tid == NULL) {
    fprintf(stderr, "ERROR memory allocation failed (tid): %s\n", strerror(errno));
    exit(1);
  }

  // Setup synchronization options
  if (opt_sync == 'm') {
    m_lock = malloc(sizeof(pthread_mutex_t) * num_lists);
    for (int i = 0; i < num_lists; i++) {
      if (pthread_mutex_init(m_lock + i, NULL) != 0) {
	fprintf(stderr, "ERROR failed to initialize mutex\n");
	exit(1);
      }
    }
  }

  if (opt_sync == 's') {
    s_lock = malloc(sizeof(int) * num_lists);
    for (int i = 0; i < num_lists; i++)
      s_lock[i] = 0;
  }

  // Get start-time
  if(clock_gettime(CLOCK_MONOTONIC, &start_time) != 0) {
    fprintf(stderr, "ERROR gettime failed: %s\n", strerror(errno));
    exit(1);
  }

  // Run the threads with the given options
  int offsets[num_threads];
  for (int i = 0; i < num_threads; i++) {
    offsets[i] = i * num_iters;
    if(pthread_create(&(tid[i]), NULL, &thread_func, (void*)&offsets[i]) != 0) {
      fprintf(stderr, "ERROR failed to create thread: %s\n", strerror(errno));
      exit(1);
    }
  }

  for (int i = 0; i < num_threads; i++) {
    if(pthread_join(tid[i], NULL) != 0) {
      fprintf(stderr, "ERROR failed to join thread: %s\n", strerror(errno));
      exit(1);
    }
  }

  // Get end-time
  if(clock_gettime(CLOCK_MONOTONIC, &end_time) != 0) {
    fprintf(stderr, "ERROR gettime failed: %s\n", strerror(errno));
    exit(1);
  }

  // Cleanup synchronization options
  if (opt_sync == 'm') {
    for (int i = 0; i < num_lists; i++) {
      if (pthread_mutex_destroy(m_lock + i) != 0) {
	printf("ERROR failed to initialize mutex\n");
	exit(1);
      }
    }
    free(m_lock);
  }
  if (opt_sync == 's')
    free(s_lock);

  // Check the list length to see if it is zero
  if (SortedList_length(head) != 0) {
    fprintf(stderr, "ERROR list length is not zero\n");
    exit(2);
  }

  // Calculate the relevant values and print a CSV row
  long long num_ops = num_threads * num_iters * 3;
  long total_time = 1000000000L * (end_time.tv_sec - start_time.tv_sec) + end_time.tv_nsec - start_time.tv_nsec;
  long avg_optime = total_time / num_ops;
  long long avg_lock_time = lock_time / num_ops;
  printCSV(num_threads, num_ops, total_time, avg_optime, avg_lock_time);
  
  // Free relevant values and exit
  free(elements);
  for (int i = 1; i < num_eles; i++)
    free(keys[i]);
  free(keys);
  free(head);
  free(tid);
  exit(0);
}
