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

int period;
char scale;
int log_fd;
int run_flag;

mraa_aio_context thermometer;
mraa_gpio_context button;


float read_temp() {

}

/*
  Processes the folowing options:
    SCALE=F, SCALE=C, PERIOD=seconds, STOP, START, LOG <line of text>, and OFF
*/
void process_input(const char* input) {
  

}

void syscall_error(const char* err_mess) {
  fprintf(stderr, err_mess);
  exit(1);
}

void mraa_error(const char* err_mess) {
  fprintf(stderr, err_mess);
  mraa_deinit();
  return EXIT_FAILURE;
}

int main (int argc, char** argv) {
  char c;
  period = 1;
  scale = 'F';
  run_flag = 1;
  
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
  mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &shutdown);

  struct pollfd poll;
  poll.fd = STDIN_FILENO;
  poll.events = POLLIN;

  char* std_input;
  if ((std_input = (char*) malloc(256 * sizeof(char))) == NULL)
    syscall_error("ERROR allocating memory for stdio\n");

  while(run_flag) {
    

  }
}
