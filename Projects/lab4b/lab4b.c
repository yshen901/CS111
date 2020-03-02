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
int log_fd;
int run_flag;
int start_flag;
char* std_input;

time_t last_timestamp = 0;
mraa_aio_context thermometer;
mraa_gpio_context button;


/* READS THE TEMPERATURE */
float read_temp() {
  float temp = (float)mraa_aio_read(thermometer);
  float R = 1023.0/temp - 1.0;
  R *= R0;
  
  float temp_c = 1.0/(log(R/R0)/Bx + 1/298.15) - 273.15;
  if (scale == 'F')
    return (temp_c * 9)/5 + 32;
  else
    return temp_c;
}

/* STDIN PROCESSOR */
void process_input(const char* input) {
  printf(input);
}

/* HELPER FUNCTIONS FOR PRINTING */
void print_time() {
  struct tm *mytime = localtime(&now);
  int temp = (int)(10*read_temp());

  char current_time[9];
  strftime(current_time, 9, "%H:%M:%S", mytime);
  printf("%s", current_time);
  if(log_fd > 0)
    fprintf(log_fd, "%s", current_time);
}

void print_temp() {
  int temp = (int)(10*read_temp());

  printf(" %d.%d\n", temp/10, temp%10);
  if(log_fd > 0)
    fprintf(log_fd, " %d.%d\n", temp/10, temp%10);
}

void print_timestamp() {
  if (start_flag == 0)
    return;
  if (time(NULL) - last_timestamp < period)
    return;

  print_time();
  print_temp();
}

/* FUNCTIONS FOR SYSTEM SHUTDOWN */
void shutdown() {
  print_time();
  printf(" SHUTDOWN\n");
  if(log_fd > 0)
    fprintf(log_fd, " SHUTDOWN\n");
}

void close_mraa() {
  mraa_aio_close(button);
  mraa_gpio_close(thermometer);
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
  char c;
  int option_index;
  
  period = 1;
  scale = 'F';
  run_flag = 1;
  start_flag = 1;
  
  static struct option long_options[] = {
    {"period", required_argument, 0, 'p'},
    {"scale", required_argument, 0, 'c'},
    {"log", required_argument, 0, 'l'},
    {0, 0, 0, 0}
  };

  while ((c = gettopt_long(argc, argv, "", long_options, &option_index)) != -1) {
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
      log_fd = open(optarg, O_RDWR | O_APPEND | O_CREAT, 0666);
      if (log_fd <= 0 )
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

  int input;
  while(run_flag) {
    print_timestamp();
    if ((input = poll(keyboard, 1, 0)) < 0) 
      syscall_error("ERROR polling the keyboard\n");
    process_input();
  }
}
