#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <signal.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <ctype.h>
#include <mraa.h>
#include <getopt.h>

#define R0 100000
#define B 4275

int period;
char scale;
FILE* log_file;
int run_flag;
int start_flag;
char* std_input;

time_t last_timestamp;
mraa_aio_context thermometer;
mraa_gpio_context button;

void syscall_error(const char* error_message);
void log_str(const char* message);


/* READS THE TEMPERATURE */
float read_temp() {
  float temp = (float)mraa_aio_read(thermometer);
  float R = 1023.0/temp - 1.0;
  R *= R0;
  
  float temp_c = 1.0/(log(R/R0)/B + 1/298.15) - 273.15;
  if (scale == 'F')
    return (temp_c * 9)/5 + 32;
  else
    return temp_c;
}

void handle_command(char c, int* i) {
  switch(c) {
  case 'F':
    scale = 'F';
    *i += 7;
    log("SCALE=F\n");
    break;
  case 'C':
    scale = 'C';
    *i += 7;
    log("SCALE=C\n");
    break;
  case 'S':
    if(start_flag == 1) {
      start_flag = 0;
      *i += 4;
      log("STOP\n");
    }
    break;
  case 'G':
    if(start_flag == 0) {
      start_flag = 1;
      *i += 5;
      log("START\n");
    }
    break;
  }
  case 'O':
    log("OFF\n");
    button_func();
    break;
}

/* STDIN PROCESSOR */
void log_str(const char* message) {
  if (log_file)
    fprintf(log_file, "%s", mess);
  else
    printf("%s", mess);
}

void process_input() {
  char input_buff [4096];
  char log_buff[256];
  int input_len = read(STDIN_FILENO, input_buff, 4096);
  if (input_len == -1)
    syscall_error("ERROR reading input\n");

  //int end, valid;
  for (int i = 0; i < input_len; i++) {
    if(strcmp(input_buff+i, "SCALE=F" == 0)) {
      handle_command('F', &i);
    else if(strcmp(input_buff+i, "SCALE=C") == 0) 
      handle_command('C', &i);
    else if(strcmp(input_buff+i, "STOP") == 0)
      handle_command('S', &i);
    else if (strcmp(input_buff+i, "START") == 0)
      handle_command('G', &i);
    else if (strcmp(input_buff+i, "OFF") == 0)
      handle_command('O', &i);
    else if (strcmp(input_buff+i, "PERIOD=") == 0) {
      i += 7;
      if (isdigit(input_buff[i]))
	period = atoi(input_buff[i]);

      if (log_file)
	fprintf(log_file, "PERIOD=%c", input_buff[i]);
      else
	printf("PERIOD=%c", input_buff[i]);
      
      /*end = i;
      valid = 1;
      while (end < input_len && input_buff[end] != '\n') {
	if (!isdigit(input_buff[end]))
	  valid = 0;
      }*/
    }
    else if (strcmp(input_buff+i, "LOG") == 0) {
      while(input_buff[i] != '\n') {
	log(&input_buff[i]);
	i++;
      }
      log("\n");
      i++;
    }
  } 
  return;  
}

/* HELPER FUNCTIONS FOR PRINTING */
void print_time() {
  time_t now = time(NULL);
  struct tm *mytime = localtime(&now);

  char current_time[10];
  strftime(current_time, 10, "%H:%M:%S", mytime);
  printf("%s", current_time);
  if(log_file)
    fprintf(log_file, "%s", current_time);
}

void print_temp() {
  int temp = (int)(10*read_temp());

  printf(" %d.%d\n", temp/10, temp%10);
  if(log_file)
    fprintf(log_file, " %d.%d\n", temp/10, temp%10);
}

void print_timestamp() {
  if (start_flag == 0)
    return;
  
  time_t now = time(NULL);
  if (now - last_timestamp < period) {
    // printf("Currrent Time: %ld\nNext Time: %ld\n\n", (long)now, (long)last_timestamp);
    return;
  }

  print_time();
  print_temp();
  time(&last_timestamp);
}

/* FUNCTIONS FOR SYSTEM SHUTDOWN */
void shutdown() { 
  print_time();
  printf(" SHUTDOWN\n");
  if(log_file)
    fprintf(log_file, " SHUTDOWN\n");
  exit(0);
}

void close_mraa() {
  mraa_gpio_close(button);
  mraa_aio_close(thermometer);
}

void button_func() {
  run_flag = 0;
  start_flag = 0;
  shutdown();
}

/* HELPER FUNCTIONS FOR ERRORS */
void syscall_error(const char* err_mess) {
  fprintf(stderr, err_mess);
  exit(1);
}

void mraa_error(const char* err_mess) {
  fprintf(stderr, err_mess);
  mraa_deinit();
  exit(1);
}


/* MAIN FUNCTION */
int main (int argc, char** argv) {
  int c;
  int option_index;
  
  period = 1;
  scale = 'C';
  run_flag = 1;
  start_flag = 1;
  log_file = NULL;
  last_timestamp = time(NULL);
  
  static struct option long_options[] = {
    {"period", required_argument, 0, 'p'},
    {"scale", required_argument, 0, 'c'},
    {"log", required_argument, 0, 'l'},
    {0, 0, 0, 0}
  };

  while ((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
    switch(c) {
    case 'p':
      period = atoi(optarg);
      if (period <= 0)
	syscall_error("ERROR period cannot be negative\n");
      break;
    case 's':
      scale = optarg[0];
      if (strlen(optarg) != 0 && scale != 'C' && scale != 'F') 
	syscall_error("ERROR invalid scale, please input either C or F\n");
      break;
    case 'l':
      log_file = fopen(optarg, "w+");
      if (log_file == NULL)
	syscall_error("ERROR opening/creating logfile\n");
      break;
    default:
      syscall_error("ERROR invalid option detected, please use --period, --scale, --log\n");
    }
  }

  if ((thermometer = mraa_aio_init(1)) == NULL)
    mraa_error("ERROR initializing thermometer\n");

  
  if ((button = mraa_gpio_init(60)) == NULL)
    mraa_error("ERROR initializing button");
  mraa_gpio_dir(button, MRAA_GPIO_IN);
  mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &button_func, NULL);

  atexit(close_mraa);

  struct pollfd keyboard[] = {
    {STDIN_FILENO, POLLIN, 0}
  };

  if ((std_input = (char*) malloc(4096 * sizeof(char))) == NULL)
    syscall_error("ERROR allocating memory for stdio\n");

  int input = 0;
  while(run_flag) {
    print_timestamp();
    input = poll(keyboard, 1, 0);
    if (input) 
      process_input();
  }

  return(0);
}
