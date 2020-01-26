/*
  NAME : YUCI SHEN
  EMAIL: SHEN.YUCI11@GMAIL.COM
  UID  : 604836772
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <string.h>
#include <errno.h>

#define ETX '\003' // End of text (^C)
#define EOF2 '\004' // End of transmission (^D)
#define LF '\012'  // Line feed
#define CR '\015'  // Carriage return

#define READ_SIZE 256

#define READ 0
#define WRITE 1

char crlf[2] = {CR, LF};
int pid;
int term_to_shell[2];
int shell_to_term[2];

struct termios saved_attributes;

void save_input_mode () {
  tcgetattr(STDIN_FILENO, &saved_attributes);
}

void reset_input_mode () {
  tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode () {
  struct termios tattr;
  //char *name;

  // Makes sure the standard input is the terminal
  if (!isatty(STDIN_FILENO)) {
    fprintf(stderr, "STDIN is not a terminal.\n");
    exit(1);
  }

  // Save the terminal attributes
  save_input_mode();

  // Restores the attributes at exit
  atexit(reset_input_mode);

  // Get the current input mode
  tcgetattr(STDIN_FILENO, &tattr);
  // Set input mode
  tattr.c_iflag = ISTRIP;
  tattr.c_oflag = 0;
  tattr.c_lflag = 0;
  tattr.c_cc[VTIME] = 0; // disables timed read
  tattr.c_cc[VMIN] = 1;  // enables a counted read that requires at least 1 char

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr);
}

void shell_exit_status() {
  int status;
  waitpid(pid, &status, 0);
  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d%s", WTERMSIG(status), WEXITSTATUS(status), crlf);
}

void sigpipe_handler() {
  close(shell_to_term[READ]);
  close(term_to_shell[WRITE]);
  kill(pid, SIGINT);
  shell_exit_status();
  exit(0);
}

int main(int argc, char** argv) {
  char c;                 // stores chars
  char key_input[10];     // buffer for keyboard read
  char shell_input[256];  // buffer for shell read

  int read_status;     // stores read's return code
  int poll_status;     // stores poll's return code
  int shell_flag = 0;  // flag for shell option
  
  int option_index = 0;
  static struct option long_options[] = {
    {"shell", no_argument, 0, 's'},
    {0,0,0,0}
  };

  while((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
    switch(c) {
      case 's':
	shell_flag = 1;
	break;
      case '?':
      default:
        fprintf(stderr, "%s\n Please use these valid options: --shell\n", strerror(errno));
        exit(1);
    }
  }

  if (shell_flag) {
    // Make two pipes for terminal-to-shell and shell-to-terminal data flows
    if (pipe(term_to_shell) != 0 || pipe(shell_to_term) != 0) {
      fprintf(stderr, "Pipe failed: %s%s", strerror(errno), crlf);
      exit(1);
    }

    set_input_mode();
    signal(SIGPIPE, sigpipe_handler);

    // Fork to create a child process (the shell)
    pid = fork();
    if (pid < 0) {
      fprintf(stderr, "Fork failed: %s%s", strerror(errno), crlf);
      exit(1);
    }
    else if (pid != 0) { /* Parent Process */
      close(term_to_shell[READ]); // terminal-to-shell pipe, close the terminal's read side
      close(shell_to_term[WRITE]); // shell-to-terminal pipe, close the terminal's write side
      
      struct pollfd fds[2];
      fds[0].fd = STDIN_FILENO;
      fds[0].events = POLLIN | POLLHUP | POLLERR;
      fds[1].fd = shell_to_term[READ];
      fds[1].events = POLLIN | POLLHUP | POLLERR;

      while(1) {
	if ((poll_status = poll(fds, 2, 0)) < 0) {
	  fprintf(stderr, "Error polling: %s%s", strerror(errno), crlf);
	  exit(1);
	}

	// Read from keyboard
	if (fds[0].revents && fds[0].revents == POLLIN) {
	  if ((read_status = read(fds[0].fd, &key_input, 10)) < 0) {
	    fprintf(stderr, "Error reading from keybaord: %s%s", strerror(errno), crlf);
	    exit(1);
	  }
	  for (int i = 0; i < read_status; i++) {
	    c = key_input[i];
	    if (c == EOF2 ) { // ^D: close the write side
	      write(STDOUT_FILENO, "^D", 2);
	      close(term_to_shell[WRITE]);
	    }
	    else if (c == ETX) { // ^C: kill the process
	      write(STDOUT_FILENO, "^C", 2);
	      if (kill(pid, SIGINT) < 0)
		fprintf(stderr, "Kill failed.");
	    }
	    else if (c == CR || c == LF) {
	      c = LF;
	      write(STDOUT_FILENO, crlf, 2);
	      write(term_to_shell[WRITE], &c, 1);
	    }
	    else {
	      write(STDOUT_FILENO, &c, 1);
	      write(term_to_shell[WRITE], &c, 1);
	    }
	  }
	}
	else if (fds[0].revents && fds[0].revents == POLLERR) { // not processed
	  fprintf(stderr, "Error polling keyboard %s%s", strerror(errno), crlf);
	  exit(1);
	}
	else if (fds[0].revents && fds[0].revents == POLLHUP) { // not processed
	  fprintf(stderr, "Peer (shell) has closed its channel: %s%s", strerror(errno), crlf);
	  exit(1);
	}

	// Either read from shell, or directly exit the loop
	if (fds[1].revents && fds[1].revents == POLLIN) {
	  read_status = read(shell_to_term[READ], &shell_input, 256);
	  if (read_status < 0) {
	    fprintf(stderr, "Error reading from shell: %s%s", strerror(errno), crlf);
	  }
	  for (int i = 0; i < read_status; i++) {
	    c = shell_input[i];
	    if (c == EOF2) {
	      close(shell_to_term[READ]);
	      shell_exit_status();
	      exit(0);
	    }
	    else if (c == LF) {
	      write(STDOUT_FILENO, crlf, 2);
	    }
	    else {
	      write(STDOUT_FILENO, &c, 1);
	    }
	  }
	}
	else if (fds[1].revents && (fds[1].revents == POLLERR || fds[1].revents == POLLHUP)) {
	  close(shell_to_term[READ]);
	  shell_exit_status();
	  exit(0);
	}
      }
    }
    else { /* Child Process */
      // terminal-to-shell pipe
      close(term_to_shell[WRITE]); // close the shell's write side 
      dup2(term_to_shell[READ], STDIN_FILENO); // redirect the shell's read side
      close(term_to_shell[READ]);

      // shell-to-terminal pipe
      close(shell_to_term[READ]); // close the shell's read side
      dup2(shell_to_term[WRITE], STDOUT_FILENO); // redirect the shell's write side
      dup2(shell_to_term[WRITE], STDERR_FILENO);
      close(shell_to_term[READ]);

      // execute the shell (code after exec doesn't execute)
      char* exec_argv [2];
      char shell_name [] = "/bin/bash";
      exec_argv[0] = shell_name;
      exec_argv[1] = NULL;
      if (execvp(shell_name, exec_argv) < 0) {
	fprintf(stderr, "Shell execution failed: %s%s", strerror(errno), crlf);
	exit(1);
      }
    }

    shell_exit_status();
    exit(0);
  }
  	 
  set_input_mode();

  while(1) {
    /* status:  0 means ETX or no more bytes read
               -1 means read error
    */
    read_status = read(STDIN_FILENO, &key_input, 10);
    if (read_status < 0) {
      fprintf(stderr, "Error reading from keyboard: %s%s", strerror(errno), crlf);
      exit(1);
    }
    else if (read_status == 0) {
      exit(0);
    }
    
    for(int i = 0; i < read_status; i++) {
      c = key_input[i];
      if (c == EOF2) {
	write(STDOUT_FILENO, "^D", 2);
	exit(0);
      }
      else if (c == ETX) {
	write(STDOUT_FILENO, "^C", 2);
	exit(0);
      }
      else if (c == CR || c == LF) {
	c = CR;
	write(STDOUT_FILENO, &c, 1);
	c = LF;
	write(STDOUT_FILENO, &c, 1);
      }
      else {
	write(STDOUT_FILENO, &c, 1);
      }
    }
  }

  exit(0);
}
