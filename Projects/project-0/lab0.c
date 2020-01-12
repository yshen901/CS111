#include <fcntl.h>  // open, creat, dup 
#include <unistd.h> // close, dup, read, write, getopt
#include <getopt.h> // getopt
#include <stdio.h>  //
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc, char** argv) {
  int c;
  int option_index = 0;

  int fd0, fd1;

  struct option long_options[] = {
    {"input", required_argument, NULL, 'i'},
    {"output", required_argument, NULL, 'o'},
    {"segfault", no_argument, NULL, 's'},
    {"catch", no_argument, NULL, 'c'},
    {0, 0, 0, 0}
  };

  while(1) {
    c = getopt_long(argc, argv, "", long_options, &option_index);
    if (c == -1) break; //returns -1 if out of options

    switch(c) {
      case 0:
	printf("All options parsed");
	break;
      case 'i':
	printf("Option %s with argument %s\n", long_options[option_index].name, optarg);
	fd0 = open(optarg, O_RDONLY);
	if(fd0 >= 0){
	  close(0);
	  dup(fd0);
	  close(fd0);
	} else {
	  fprintf(stdout, "Failed with error code %s\n", strerror(errno));
	  _exit(2);
	}
	break;
      case 'o':
	printf("Option %s with argument %s\n", long_options[option_index].name, optarg);
	break;
      case 's':
        printf("Option %s", long_options[option_index].name);
	break;
      case 'c':
	printf("Option %s", long_options[option_index].name);
	break;
      case '?':
	printf("Foreign option...skipped\n");
	break;
      default:
	printf("Unknown option detected with code %c\n", c);
	break;
    }
  }

  return 0;
}
