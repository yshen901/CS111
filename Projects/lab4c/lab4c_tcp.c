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
#include <sys/socket.h>
#include <ctype.h>
#include <netdb.h>
#include <netinet/in.h>

#define R0 100000
#define B 4275

int period;
char scale;
FILE* log_file;
int run_flag;
int start_flag;
char* std_input;
char* id;
int port = -1;

char* host;
struct hostent* server;
int sock_fd;
struct sockaddr_in serv_addr;

time_t last_timestamp = 0;
mraa_aio_context thermometer;
mraa_gpio_context button;

void syscall_error(const char* error_message);
void log_str(const char* message, int print_server);
void button_func();

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
    log_str("SCALE=F\n", 0);
    break;
  case 'C':
    scale = 'C';
    *i += 7;
    log_str("SCALE=C\n", 0);
    break;
  case 'S':
    if(start_flag == 1) {
      start_flag = 0;
      *i += 4;
    }
    log_str("STOP\n", 0);
    break;
  case 'G':
    if(start_flag == 0) {
      start_flag = 1;
      *i += 5; 
    }
    log_str("START\n", 0);
    break;
  case 'O':
    log_str("OFF\n", 0);
    button_func();
    break;
  default:
    syscall_error("ERROR wrong command\n");
  }	
}

/* STDIN PROCESSOR */
void log_str(const char* mess, int print_server) {
  if (print_server == 1)
    write(sock_fd, mess, strlen(mess));
  if (log_file)
    fprintf(log_file, "%s", mess);
  printf("%s", mess);
}

void process_input() {
  char input_buff [4096];
  char log_buff[256];
  int input_len = read(sock_fd, input_buff, 4096);
  if (input_len == -1)
    syscall_error("ERROR reading input\n");
  input_buff[input_len] = '\0'; //AVOIDS OVERFLOW ISSUE 
  //int end, valid;
  int i;
  int j;
  for (i = 0; i < input_len; i++) { 
    if(strncmp(input_buff+i, "SCALE=F", 7) == 0) 
      handle_command('F', &i);
    else if(strncmp(input_buff+i, "SCALE=C", 7) == 0) 
      handle_command('C', &i);
    else if(strncmp(input_buff+i, "STOP", 4) == 0)
      handle_command('S', &i);
    else if (strncmp(input_buff+i, "START", 5) == 0){
      //printf("STARTED\n");
      handle_command('G', &i);

    }
    else if (strncmp(input_buff+i, "OFF", 3) == 0)
      handle_command('O', &i);
    else if (strncmp(input_buff+i, "PERIOD=",7) == 0) {
      i += 7;
      if (isdigit(input_buff[i]))
	period = atoi(&input_buff[i]);

      char buff[64];
      sprintf(buff, "PERIOD=%c\n", input_buff[i]);
      log_str(buff, 0);
      
      /*end = i;
      valid = 1;
      while (end < input_len && input_buff[end] != '\n') {
	if (!isdigit(input_buff[end]))
	  valid = 0;
      }*/
    }
    else if (strncmp(input_buff+i, "LOG", 3) == 0) {
      j = 0;
      while(input_buff[i] != '\n') {
	log_buff[j] = input_buff[i];
	i++;
        j++;
      }
      log_buff[j] = '\n';
      log_buff[j+1] = '\0';
      log_str(log_buff, 0);
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
  log_str(current_time, 1);
}

void print_temp() {
  time_t now = time(NULL);
  struct tm *mytime = localtime(&now);
  char current_time[10];
  strftime(current_time, 10, "%H:%M:%S", mytime);

  int temp = (int)(10*read_temp());

  char buff [64];
  sprintf(buff, "%s %d.%d\n", current_time, temp/10, temp%10);
  log_str(buff, 1);
}

void print_timestamp() {
  if (start_flag == 0)
    return;
  
  struct timeval now;
  if(gettimeofday(&now, 0) < 0)
    syscall_error("ERROR getting current time\n");
  
  if (now.tv_sec < last_timestamp) {
    //printf("Currrent Time: %ld\n", (long)now.tv_sec);
    return;
  }

  //print_time();
  print_temp();
  last_timestamp = now.tv_sec + period;
}

/* FUNCTIONS FOR SYSTEM SHUTDOWN*/
void shutdown_func() { 
  time_t now = time(NULL);
  struct tm *mytime = localtime(&now);
  char current_time[10];
  strftime(current_time, 10, "%H:%M:%S", mytime);

  char buff [64];
  sprintf(buff, "%s SHUTDOWN\n", current_time);

  log_str(buff, 1);
  exit(0);
}

void close_mraa() {
  // mraa_gpio_close(button);
  mraa_aio_close(thermometer);
}


void button_func() {
  run_flag = 0;
  start_flag = 0;
  shutdown_func();
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
  scale = 'F';
  run_flag = 1;
  start_flag = 1;
  log_file = NULL;
  last_timestamp = time(NULL);
  
  static struct option long_options[] = {
    {"period", required_argument, 0, 'p'},
    {"scale", required_argument, 0, 's'},
    {"log", required_argument, 0, 'l'},
    {"id", required_argument, 0, 'i'},
    {"host", required_argument, 0, 'h'},
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
    case 'i':
      id = optarg;
      break;
    case 'h':
      host = optarg;
      break;
    default:
      syscall_error("ERROR invalid option detected, please use --period, --scale, --log\n");
    }
  }

  if (optind < argc) {
    port = atoi(argv[optind]);
    if (port <= 0)
      syscall_error("ERROR invalid port number, must be greater than 0\n");
  }

  if (log_file == NULL)
    syscall_error("ERROR log argument is missing\n");

  if (port == -1 || host == NULL || id == NULL)
    syscall_error("ERROR port, id, and host arguments are missing\n");

  if (strlen(host) == 0)
    syscall_error("ERROR host length must be greater than 0\n");

  if (strlen(id) != 9)
    syscall_error("ERROR id must be exactly 9 digits\n");

  /*if ((button = mraa_gpio_init(60)) == NULL)
    mraa_error("ERROR initializing button");
  mraa_gpio_dir(button, MRAA_GPIO_IN);
  mraa_gpio_isr(button, MRAA_GPIO_EDGE_RISING, &button_func, NULL);*/
  
  if ((thermometer = mraa_aio_init(1)) == NULL)
    mraa_error("ERROR initializing thermometer\n");  
  atexit(close_mraa);

  if ((server = gethostbyname(host)) == NULL)
    syscall_error("ERROR finding host\n");
  
  if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    syscall_error("ERROR creating socket\n");

  bzero((char*) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(port);

  if (connect(sock_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
    syscall_error("ERROR connecting to remote host\n");

  char mess[64];
  sprintf(mess, "ID=%s\n", id);
  log_str(mess, 1);
  
  struct pollfd sock_poll;
  sock_poll.fd = sock_fd;
  sock_poll.events = POLLIN;

  if ((std_input = (char*) malloc(4096 * sizeof(char))) == NULL)
    syscall_error("ERROR allocating memory for stdio\n");

  int input = 0;
  while(run_flag) {
    print_timestamp();
    input = poll(&sock_poll, 1, 0);
    if (input) {
      process_input();
    }
  }

  return(0);
}
