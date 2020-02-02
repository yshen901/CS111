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
#include <netinet/in.h>
#include <sys/socket.h>
#include <zlib.h>

#define ETX '\003' // End of text (^C)
#define EOT '\004' // End of transmission (^D)
#define LF '\012'  // Line feed
#define CR '\015'  // Carriage return

#define READ 0
#define WRITE 1

#define BUFF_SIZE 256

char crlf[2] = {CR, LF};
int pid;
int serv_to_shell[2];
int shell_to_serv[2];
int sock_fd1;
int sock_fd2;

z_stream server_to_client;
z_stream client_to_server;

struct termios saved_attributes;

/* ------------------
    HELPER FUNCTIONS 
   ------------------ */
void err_mess(char* message) {
  fprintf(stderr, "%s: %s\n", message, strerror(errno));
  exit(1);
}

int safe_write(int fd, char * message, int length, char * target) {
  int write_status;
  if ((write_status = write(fd, message, length)) < 0) {
    fprintf(stderr, "ERROR writing to %s: %s\n", target, strerror(errno));
    exit(1);
  }
  return write_status;
}

int safe_read(int fd, char * buffer, int length, char * source) {
  int read_status;
  if ((read_status = read(fd, buffer, length)) < 0) {
    fprintf(stderr, "ERROR reading from %s: %s\n", source, strerror(errno));
    exit(1);
  }
  return read_status;
}

void initialize_compression() {
  client_to_server.zalloc = Z_NULL;
  client_to_server.zfree = Z_NULL;
  client_to_server.opaque = Z_NULL;

  if (inflateInit(&client_to_server) != Z_OK)
    err_mess("ERROR initializing inflation");
  
  server_to_client.zalloc = Z_NULL;
  server_to_client.zfree = Z_NULL;
  server_to_client.opaque = Z_NULL;

  if (deflateInit(&server_to_client, Z_DEFAULT_COMPRESSION) != Z_OK)
    err_mess("ERROR initializing inflation");
}

/* --------------------
   SHELL EXIT HANDLERS
   -------------------- */
void shell_exit_status() {
  int status;
  waitpid(pid, &status, 0);
  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
}

void sigpipe_handler() {
  close(sock_fd1);
  close(sock_fd2);
  close(serv_to_shell[WRITE]);
  close(shell_to_serv[READ]);
  kill(pid, SIGINT);
  shell_exit_status();
  exit(0);
}

/* --------
   CLEAN UP
   --------*/
void clean_up() {
  inflateEnd(&client_to_server);
  deflateEnd(&server_to_client);
}

/* ---------
   MAIN CODE
   --------- */
int main(int argc, char** argv) {
  /* Variables to store infomation */
  char c;
  char comp_buffer[4 * BUFF_SIZE];
  char read_buffer[BUFF_SIZE];

  /* Variables to store reading and polling status*/
  int read_status;
  int poll_status;
  int zlib_status;

  /* Variables to store argument flags and options */
  int port_flag = 0;
  int compress_flag = 0;

  /* Variables for socket communication */
  int port_num;
  struct sockaddr_in serv_addr, cli_addr;
  
  int option_index = 0;
  static struct option long_options[] = {
    {"port", required_argument, 0, 'p'},
    {"compress", no_argument, 0, 'c'},
    {0,0,0,0}
  };
  
  while((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
    switch(c) {
      case 'p':
	port_flag = 1;
	port_num = atoi(optarg);
	break;
      case 'c':
	compress_flag = 1;
	initialize_compression();
	atexit(clean_up);
	break;
      case '?':
      default:
        fprintf(stderr, "%s: Please use --port={port#} or no argument\n", strerror(errno));
        exit(1);
    }
  }

  /* TRIGGER ERROR IF: 1) port option isn't triggered or 2) port number is reserved */
  if (port_flag == 0) {
    fprintf(stderr, "Port option is mandatory, please use --port={port number} to specify the port");
    exit(1);
  }
  else if (port_num < 1024) {
    fprintf(stderr, "Port number is reserved, please use a port number greater than 1024");
    exit(1);
  }
  
  /* PIPE CREATION */
  if (pipe(shell_to_serv) != 0 || pipe(serv_to_shell) != 0)
    err_mess("ERROR creating pipe");

  /* ERROR HANDLER */
  signal(SIGPIPE, sigpipe_handler);

  /* FORK THEN SETUP SHELL */
  pid = fork();
  if (pid < 0)
    err_mess("ERROR creating fork");
  else if (pid == 0) { //CHILD
    close(serv_to_shell[WRITE]); // Close unecessary pipes
    close(shell_to_serv[READ]);

    dup2(serv_to_shell[READ], STDIN_FILENO);   // Get input from the server
    dup2(shell_to_serv[WRITE], STDOUT_FILENO); // Send outputs to the server
    dup2(shell_to_serv[WRITE], STDERR_FILENO);  // Send errors to the server

    close(serv_to_shell[READ]); // Close the rest of the pipes
    close(shell_to_serv[WRITE]);

    char* exec_argv[2]; // Setup the shell
    char shell_name [] = "/bin/bash";
    exec_argv[0] = shell_name;
    exec_argv[1] = NULL; // Indicates no arguments
    if (execvp(shell_name, exec_argv) < 0)
      err_mess("ERROR creating shell");
  }
  else { // PARENT
    /* SETUP: SOCKET */
    sock_fd1 = socket(AF_INET, SOCK_STREAM, 0); // Create a socket
    if (sock_fd1 < 0)
      err_mess("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr)); // Setup server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port_num);

    if (bind(sock_fd1, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) // Binds the socketx
      err_mess("ERROR binding the socket");
  
    listen(sock_fd1, 5); // Listen for a client connection (max 5)
    socklen_t clilen = sizeof(cli_addr);
    sock_fd2 = accept(sock_fd1, (struct sockaddr*) &cli_addr, &clilen); // Waits for a connection
    if (sock_fd2 < 0)
      err_mess("ERROR accepting");

    close(serv_to_shell[READ]); // Close unecessary pipes
    close(shell_to_serv[WRITE]);

    /* POLLING: socket and shell */
    struct pollfd fds[] = {
      {sock_fd2, POLLIN|POLLHUP|POLLERR, 0},
      {shell_to_serv[READ], POLLIN|POLLHUP|POLLERR, 0}
    };

    int open = 1;
    while(open) {
      if ((poll_status = poll(fds, 2, 0)) < 0)
	err_mess("ERROR polling");
    
      if (fds[0].revents & POLLIN) { // Socket has input
	read_status = safe_read(fds[0].fd, read_buffer, BUFF_SIZE, "socket");
	if (read_status == 0) { // client exits (EOF)
	  close(serv_to_shell[WRITE]);
	  open = 0;
	}
	else {
	  if (compress_flag == 1) {
	    client_to_server.avail_in = read_status;
	    client_to_server.next_in = (unsigned char *) read_buffer;
	    client_to_server.avail_out = 4*BUFF_SIZE;
	    client_to_server.next_out = (unsigned char *) comp_buffer;

	    while (client_to_server.avail_in > 0) {
	      zlib_status = inflate(&client_to_server, Z_SYNC_FLUSH);
	      if (zlib_status != Z_OK)
		err_mess("ERROR inflating compressed client output");
	    }

	    for (int i = 0; (unsigned int) i < 4*BUFF_SIZE - client_to_server.avail_out; i++) {
	      c = comp_buffer[i];
	      if (c == EOT) // close the write end ... shell will exit (send EOT)
		close(serv_to_shell[WRITE]); 
	      else if (c == ETX) // kill the shell
		kill(pid, SIGINT);
	      else // (CRLF mapping alrerady done on client-side)
		safe_write(serv_to_shell[WRITE], &c, 1, "shell");
	    }
	  }
	  else {
	    for (int i = 0; i < read_status; i++) {
	      c = read_buffer[i];
	      if (c == EOT) // close the write end...shell will exit (send EOT)
		close(serv_to_shell[WRITE]);
	      else if (c == ETX) // kill the shell
		kill(pid, SIGINT);
	      else // (CRLF mapping already done on client-side)
		safe_write(serv_to_shell[WRITE], &c, 1, "shell");
	    }
	  }
	}
      }
      else if (fds[0].revents & POLLERR) {
	fprintf(stderr, "ERROR while polling socket: socket error\n");
	open = 0;
      }
      else if (fds[0].revents & POLLHUP) {
	fprintf(stderr, "ERROR while polling socket: socket hung up\n");
	open = 0;
      }

      if (fds[1].revents & POLLIN) { // Shell has input...send through socket
	read_status = safe_read(fds[1].fd, read_buffer, BUFF_SIZE, "shell");
	if (compress_flag == 1) {
	  server_to_client.avail_in = read_status;
	  server_to_client.next_in = (unsigned char*) read_buffer;
	  server_to_client.avail_out = BUFF_SIZE;
	  server_to_client.next_out = (unsigned char*) comp_buffer;

	  while(server_to_client.avail_in > 0) {
	    zlib_status = deflate(&server_to_client, Z_SYNC_FLUSH);
	    if (zlib_status != Z_OK) {
	      fprintf(stderr, "ERROR deflating shell output: %d", zlib_status);
	      exit(1);
	    }
	  }
	  safe_write(sock_fd2, comp_buffer, BUFF_SIZE - server_to_client.avail_out, "socket");
	}
	else {
	  safe_write(sock_fd2, read_buffer, read_status, "socket");
	}
      }
      else if (fds[1].revents & POLLHUP) { // Shell exits...break out of loop
	fprintf(stderr, "ERROR while polling shell: shell hung up\n");
	open = 0;
      }
      else if (fds[1].revents & POLLERR) {
	fprintf(stderr, "ERROR while polling shell: shell error\n");
	open = 0;
      }
    }
    close(sock_fd1);
    close(sock_fd2);
    close(shell_to_serv[READ]);
    close(serv_to_shell[WRITE]);
    shell_exit_status();
    exit(0);
  }
  exit(0);
}
