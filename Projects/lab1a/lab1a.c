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

void err_mess(char* message) {
  fprintf(stderr, "%s: %s%s", message, strerror(errno), crlf);
  exit(1);
}

void shell_exit_status() {
  int status;
  waitpid(pid, &status, 0);
  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d%s", WTERMSIG(status), WEXITSTATUS(status), crlf);
}

void sigpipe_handler() {
  close(term_to_shell[WRITE]);
  close(shell_to_term[READ]);
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

  set_input_mode(); //set input mode before anything else

  while((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
    switch(c) {
      case 's':
	shell_flag = 1;
	break;
      case '?':
      default:
        fprintf(stderr, "%s: Please use --shell or no argument\n", strerror(errno));
        exit(1);
    }
  }

  if (shell_flag) {
    // Make two pipes for terminal-to-shell and shell-to-terminal data flows
    if (pipe(shell_to_term) != 0 || pipe(term_to_shell) != 0) 
      err_mess("Pipe creation failed");

    signal(SIGPIPE, sigpipe_handler);

    // Fork to create a child process (the shell)
    pid = fork();
    if (pid < 0)
      err_mess("Fork failed");
    else if (pid != 0) { /* Parent Process */
      if (close(term_to_shell[READ]) < 0) // terminal-to-shell pipe, close the terminal's read side
	err_mess("Failed to close term_to_shell[READ]");
      if (close(shell_to_term[WRITE]) < 0) // shell-to-terminal pipe, close the terminal's write side
	err_mess("Failed to close shell_to_term[WRITE]");
      
      struct pollfd fds[] = {
	{STDIN_FILENO, POLLIN|POLLHUP|POLLERR, 0},
	{shell_to_term[READ], POLLIN|POLLHUP|POLLERR, 0}
      };

      while(1) {
	if ((poll_status = poll(fds, 2, 0)) < 0) 
	  err_mess("Error polling");

	// Read from keyboard
	if (fds[0].revents && fds[0].revents == POLLIN) {
	  if ((read_status = read(fds[0].fd, &key_input, 10)) < 0) 
	    err_mess("Error reading from keybaord");
	  for (int i = 0; i < read_status; i++) {
	    c = key_input[i];
	    if (c == EOF2 ) { // ^D: close the write side
	      if (write(STDOUT_FILENO, "^D", 2) < 0)
		err_mess("Failed to write ^D to STDOUT");
	      if (close(term_to_shell[WRITE]) < 0)
		err_mess("Failed to close term_to_shell[WRITE]");
	    }
	    else if (c == ETX) { // ^C: kill the process
	      if (write(STDOUT_FILENO, "^C", 2) < 0)
		err_mess("Failed to write ^C to STDOUT");
	      if (kill(pid, SIGINT) < 0)
		fprintf(stderr, "Kill failed.");
	    }
	    else if (c == CR || c == LF) {
	      c = LF;
	      if (write(STDOUT_FILENO, crlf, 2) < 0)
		err_mess("Failed to write crlf to STDOUT");
	      if (write(term_to_shell[WRITE], &c, 1) < 0)
		err_mess("Failed to write to term_to_shell[WRITE]");
	    }
	    else {
	      if (write(STDOUT_FILENO, &c, 1) < 0)
		err_mess("Failed to write to STDOUT");
	      if (write(term_to_shell[WRITE], &c, 1) < 0)
		err_mess("Failed to write to term_to_shell[WRITE]");
	    }
	  }
	}
	else if (fds[0].revents && fds[0].revents == POLLERR) // not processed
	  err_mess("Error polling keyboard");
	else if (fds[0].revents && fds[0].revents == POLLHUP) // not processed
	  err_mess("Peer (shell) has closed its channel");

	// Either read from shell, or directly exit the loop
	if (fds[1].revents && fds[1].revents == POLLIN) {
	  read_status = read(shell_to_term[READ], &shell_input, 256);
	  if (read_status < 0)
	    err_mess("Error reading from shell");
	  for (int i = 0; i < read_status; i++) {
	    c = shell_input[i];
	    if (c == EOF2) {
	      if(close(shell_to_term[READ]) < 0)
		err_mess("Failed to close shell_to_term[READ]");
	      shell_exit_status();
	      exit(0);
	    }
	    else if (c == LF) {
	      if (write(STDOUT_FILENO, crlf, 2) < 0)
		err_mess("Failed to write crlf to STDOUT");
	    }
	    else {
	      if (write(STDOUT_FILENO, &c, 1) < 0)
		err_mess("Failed to write to STDOUT");
	    }
	  }
	}
	else if (fds[1].revents && (fds[1].revents == POLLERR || fds[1].revents == POLLHUP)) {
	  if (close(shell_to_term[READ]) < 0)
	    err_mess("Failed to close shell_to_term[READ]");
	  shell_exit_status();
	  exit(0);
	}
      }

      shell_exit_status();
      exit(0);
    }
    else { /* Child Process */
      // terminal-to-shell pipe
      if (close(term_to_shell[WRITE]) < 0) // close the shell's write side
	err_mess("Failed to close term_to_shell[WRITE]");
      if (dup2(term_to_shell[READ], STDIN_FILENO) < 0) // redirect to the shell's read side
	err_mess("Failed to redirect term_to_shell[READ] to STDIN_FILENO");
      if (close(term_to_shell[READ]))
	err_mess("Failed to close term_to_shell[READ]");

      // shell-to-terminal pipe
      if(close(shell_to_term[READ]) < 0) // close the shell's read side
	err_mess("Failed to close shell_to_term[READ]");
      if (dup2(shell_to_term[WRITE], STDOUT_FILENO) < 0) // redirect to the shell's write side
	err_mess("Failed to redirect STDOUT to shell_to_term[WRITE]");
      if (dup2(shell_to_term[WRITE], STDERR_FILENO) < 0) 
        err_mess("Failed to redirect STDERR to shell_to_term[WRITE]");
      if (close(shell_to_term[WRITE]) < 0) 
        err_mess("Failed to close shell_to_term[WRITE]");

      // execute the shell (code after exec doesn't execute)
      char* exec_argv [2];
      char shell_name [] = "/bin/bash";
      exec_argv[0] = shell_name;
      exec_argv[1] = NULL;
      if (execvp(shell_name, exec_argv) < 0)
	err_mess("Shell execution failed");
    }

    shell_exit_status();
    exit(0);
  }

  while(1) {
    /* status:  0 means ETX or no more bytes read
               -1 means read error
    */
    read_status = read(STDIN_FILENO, &key_input, 10);
    if (read_status < 0)
      err_mess("Error reading from keyboard");
    else if (read_status == 0) {
      exit(0);
    }
    
    for(int i = 0; i < read_status; i++) {
      c = key_input[i];
      if (c == EOF2) {
	if (write(STDOUT_FILENO, "^D", 2) < 0)
	  err_mess("Failed to write ^D to STDOUT");
	exit(0);
      }
      else if (c == ETX) {
	if (write(STDOUT_FILENO, "^C", 2) < 0)
	  err_mess("Failed to write ^C to STDOUT");
	exit(0);
      }
      else if (c == CR || c == LF) {
	if (write(STDOUT_FILENO, crlf, 2) < 0)
	  err_mess("Failed to write crlf to STDOUT");
      }
      else {
	if (write(STDOUT_FILENO, &c, 1) < 0)
	  err_mess("Failed to write to STDOUT");
      }
    }
  }

  exit(0);
}
