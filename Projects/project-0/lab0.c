/*
  NAME : YUCI SHEN
  EMAIL: SHEN.YUCI11@GMAIL.COM
  ID   : 604836772
*/

#include <fcntl.h>  // open, creat, dup 
#include <unistd.h> // close, dup, read, write, getopt
#include <getopt.h> // getopt
#include <stdio.h>  // fprintf
#include <stdlib.h> // _exit
#include <signal.h> // signal
#include <errno.h>  // errno
#include <string.h> // strerror

#define INPUT 'i'
#define OUTPUT 'o'
#define SEGFAULT 's'
#define CATCH 'c'

void handle_error(int signum) {
  fprintf(stderr, "SEGFAULT caught: signum %d (%s)\n", signum, strerror(signum));
  exit(4);
}

int main(int argc, char** argv) {

  int c;
  int option_index = 0;
  int segfault = 0;
  int fd0, fd1;
  
  struct option long_options[] = {
    {"input", required_argument, &c, INPUT}, // use #define INPUT 'i' to set an alias
    {"output", required_argument, &c, OUTPUT},
    {"segfault", no_argument, &c, SEGFAULT},
    {"catch", no_argument, &c, CATCH},
    {0, 0, 0, 0}
  };

  // handling options and redirection
  while(getopt_long(argc, argv, "", long_options, &option_index) != -1) {
    switch(c) {
      case 0: // What is this used for in the example?
	break;
      case INPUT:
	fd0 = open(optarg, O_RDONLY);
	if(fd0 >= 0){
	  close(0);
	  dup(fd0);
	  close(fd0);
	} else {
	  fprintf(stderr, "Error opening input file: %s\n", strerror(errno));
	  exit(2);
	}
	break;
      case OUTPUT:
	//fd1 = creat(optarg, 0666); // 0666 specifies r+w permissions for user, group, and other
	fd1 = open(optarg, O_RDWR | O_CREAT, 0666); // Same as above, but not having O_RDWR will cause a bad file-descriptor error
	if (fd1 >= 0) {
	  dup2(fd1, 1); // NOTE: Same as close -> dup -> close
	} else {
	  fprintf(stderr, "Error opening/creating output file: %s\n", strerror(errno)); // prints to specific file-descriptor
	  exit(3);
	}
	break;
      case SEGFAULT:
	segfault = 1;
	break;
      case CATCH:
	signal(SIGSEGV, handle_error);
	break;
      case '?':
      default:
	printf("Unknown option detected, please use the following supported options: ");
	printf("--input=filename, --output=filename, --segfault, and --catch\n");
	exit(1);
    }
  }

  // trigger segfault
  if (segfault == 1) {
    char* p = NULL;
    *p = 'A';
  }

  // reading/writing the information
  int temp;
  char* buff;
  while(1) {
    temp = read(0, &buff, sizeof(char));
    
    if (temp == 0) break;
    if (temp == -1) {
      fprintf(stderr, "Error reading from file: %s\n", strerror(errno));
      exit(-1);
    }

    temp = write(1, &buff, sizeof(char));
    if (temp == -1) {
      fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
      exit(-1);
    }
  }

  exit(0);
}
