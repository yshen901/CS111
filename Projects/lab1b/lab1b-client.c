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
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <zlib.h>

#define ETX '\003' // End of text (^C)
#define EOF2 '\004' // End of transmission (^D)
#define LF '\012'  // Line feed
#define CR '\015'  // Carriage return

#define READ 0
#define WRITE 1

#define SENT 0
#define RECEIVED 1

#define BUFF_SIZE 256

char crlf[2] = {CR, LF};

/* Variables to store argument flags and options */
int port_flag = 0;
int log_flag = 0;
int compress_flag = 0;

/* Variables to store socket, log, and compression information*/
int sock_fd, log_fd;
z_stream client_to_server;
z_stream server_to_client;

struct termios saved_attributes;

/* ------------------
    HELPER FUNCTIONS 
   ------------------ */
int safe_write(int fd, char * message, int length, char * target) {
  int write_status;
  if ((write_status = write(fd, message, length)) < 0) {
    fprintf(stderr, "ERROR writing to %s: %s%s", target, strerror(errno), crlf);
    exit(1);
  }
  return write_status;
}

int safe_read(int fd, char * buffer, int length, char * source) {
  int read_status;
  if ((read_status = read(fd, buffer, length)) < 0) {
    fprintf(stderr, "ERROR reading from %s: %s%s", source, strerror(errno), crlf);
    exit(1);
  }
  return read_status;
}

void err_mess(char* message) {
  fprintf(stderr, "%s: %s%s", message, strerror(errno), crlf);
  exit(1);
}

void log_to_file(int fd, int type, char* data, int data_size, char* num_bytes) {
  sprintf(num_bytes, "%d", data_size); // Converts read_size to a string

  if (type == SENT)
    safe_write(fd, "SENT ", 5, "logfile");
  else if (type == RECEIVED)
    safe_write(fd, "RECEIVED ", 9, "logfile");
  else
    return;
  
  safe_write(fd, num_bytes, strlen(num_bytes), "logfile");
  safe_write(fd, " bytes: ", 8, "logfile");
  safe_write(fd, data, data_size, "logfile");
  safe_write(fd, "\n", 1, "logfile");
}

void initialize_compression() {
  client_to_server.zalloc = Z_NULL;
  client_to_server.zfree = Z_NULL;
  client_to_server.opaque = Z_NULL;

  if (deflateInit(&client_to_server, Z_DEFAULT_COMPRESSION) != Z_OK)
    err_mess("ERROR initializing deflation");

  server_to_client.zalloc = Z_NULL;
  server_to_client.zfree = Z_NULL;
  server_to_client.opaque = Z_NULL;

  if (inflateInit(&server_to_client) != Z_OK)
    err_mess("ERROR initializing inflation");
}

/* --------------------
   INPUT MODE FUNCTIONS
   -------------------- */
void save_input_mode () {
  tcgetattr(STDIN_FILENO, &saved_attributes);
}

void reset_input_mode () {
  tcsetattr(STDIN_FILENO, TCSANOW, &saved_attributes);
}

void set_input_mode () {
  struct termios tattr;

  if (!isatty(STDIN_FILENO)) { // STDIN must be terminal
    fprintf(stderr, "STDIN is not a terminal.\n");
    exit(1);
  }

  save_input_mode();        // save current input settings

  tcgetattr(STDIN_FILENO, &tattr); // get and modify the current input mode
  tattr.c_iflag = ISTRIP;          
  tattr.c_oflag = 0;
  tattr.c_lflag = 0;
  tattr.c_cc[VTIME] = 0;           // disables timed read
  tattr.c_cc[VMIN] = 1;            // enables a counted read that requires at least 1 char
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &tattr); 
}

/* --------
   CLEAN UP
   --------*/
void clean_up() {
  if (log_flag == 1)
    close(log_fd);
  if (compress_flag == 1) {
    deflateEnd(&client_to_server);
    inflateEnd(&server_to_client);
  }
  close(sock_fd);
  reset_input_mode();
}

/* ---------
   MAIN CODE
   --------- */
int main(int argc, char** argv) {
  /* Variables to store infomation */
  char c;
  char read_buffer[BUFF_SIZE];
  char comp_buffer[4*BUFF_SIZE];
  char num_bytes[11]; // Max bytes needed to represent MAXINT

  /* Variables to store reading and polling status*/
  int zlib_status;
  int read_status;
  int poll_status;

  /* Variables for socket communication */
  int port_num;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  
  int option_index = 0;
  static struct option long_options[] = {
    {"log", required_argument, 0, 'l'},
    {"port", required_argument, 0, 'p'},
    {"compress", no_argument, 0, 'c'},
    {0,0,0,0}
  };

  while((c = getopt_long(argc, argv, "", long_options, &option_index)) != -1) {
    switch(c) {
      case 'l':
	log_flag = 1;
	log_fd = open(optarg, O_TRUNC | O_RDWR | O_CREAT, 0666); // If file exists, clear all initial data
	if (log_fd < 0) {
	  fprintf(stderr, "ERROR opening/creating logfile: %s%s", strerror(errno), crlf);
	  exit(1);
	}
	break;
      case 'p':
	port_flag = 1;
	port_num = atoi(optarg);
	break;
      case 'c':
	compress_flag = 1;
	initialize_compression();
	break;
      case '?':
      default:
        fprintf(stderr, "ERROR processing arguments, please use --port={portno} is required, --compress and --log={logfile} are optional%s", crlf);
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
  
  set_input_mode();
  atexit(clean_up);

  /* SETUP: SOCKET */
  sock_fd = socket(AF_INET, SOCK_STREAM, 0); // Create a socket
  if (sock_fd < 0)
    err_mess("ERROR opening socket");
  
  server = gethostbyname("localhost"); // Get server information (hostent)
  if (server == NULL)
    err_mess("ERROR getting host name");

  bzero((char *) &serv_addr, sizeof(serv_addr)); // Setup servaddr_in using the hostent
  serv_addr.sin_family = AF_INET;
  bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(port_num);

  if (connect(sock_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) // Establish connection
    err_mess("ERROR connecting to socket");

  /* POLLING: keyboard and socket */
  struct pollfd fds[] = {
    {STDIN_FILENO, POLLIN|POLLHUP|POLLERR, 0},
    {sock_fd, POLLIN|POLLHUP|POLLERR, 0}
  };

  while(1) {
    if ((poll_status = poll(fds, 2, 0)) < 0)
      err_mess("ERROR polling");

    /* Polls from the keyboard */
    if (fds[0].revents & POLLIN) {
      read_status = safe_read(fds[0].fd, read_buffer, BUFF_SIZE, "keyboard");
      for (int i = 0; i < read_status; i++) {
	c = read_buffer[i];
	if (c == CR || c == LF) // Terminal echo
	  safe_write(STDOUT_FILENO, crlf, 2, "terminal");
	else
	  safe_write(STDOUT_FILENO, &c, 1, "terminal");

	if (c == CR || c == LF) // CRLF mapping for shell
	  c = LF;
	
	if (compress_flag == 1) { // Compressed write to socket
	  client_to_server.avail_in = 1;
	  client_to_server.next_in = (unsigned char *) &c;
	  client_to_server.avail_out = BUFF_SIZE;
	  client_to_server.next_out = (unsigned char *) comp_buffer;

	  while (client_to_server.avail_in > 0) {
	    zlib_status = deflate(&client_to_server, Z_SYNC_FLUSH);
	    if (zlib_status != Z_OK)
	      err_mess("ERROR deflating keyboard data");
	  }
	  
	  safe_write(sock_fd, comp_buffer, BUFF_SIZE - client_to_server.avail_out, "socket");
	  if (log_flag == 1)
	    log_to_file(log_fd, SENT, comp_buffer, BUFF_SIZE - client_to_server.avail_out, num_bytes);
	}
	else { // Uncompressed write to socket
	  safe_write(sock_fd, &c, 1, "socket");
	  if (log_flag == 1)
	    log_to_file(log_fd, SENT, &c, 1, num_bytes);
	}
      }
    }
    else if (fds[0].revents & POLLHUP || fds[0].revents & POLLERR) {
      fprintf(stderr, "ERROR polling keyboard%s", crlf);
      exit(1);
    }

    /* Polls from the socket */
    if (fds[1].revents & POLLIN) {
      read_status = safe_read(fds[1].fd, read_buffer, BUFF_SIZE, "socket");

      /* Exits if the read is 0, which only happens if the server socket closes */
      if (read_status == 0)
	exit(0);

      if (log_flag == 1) // Logs received data
        log_to_file(log_fd, RECEIVED, read_buffer, read_status, num_bytes);

      if (compress_flag == 1) { // Decompresses before processing
	server_to_client.avail_in = read_status;
	server_to_client.next_in = (unsigned char*) read_buffer;
	server_to_client.avail_out = 4*BUFF_SIZE;
	server_to_client.next_out = (unsigned char*) comp_buffer;

	while (server_to_client.avail_in > 0) {
	  zlib_status = inflate(&server_to_client, Z_SYNC_FLUSH);
	  if (zlib_status != Z_OK)
	    err_mess("ERROR inflating compressed shell output");
	}
	
	for (int i = 0; (unsigned int) i < 4*BUFF_SIZE - server_to_client.avail_out; i++) {
	  c = comp_buffer[i];
	  if (c == LF || c == CR)
	    safe_write(STDOUT_FILENO, crlf, 2, "terminal");
	  else
	    safe_write(STDOUT_FILENO, &c, 1, "terminal");
	}
      }
      else { // Immediately processes
	for (int i = 0; i < read_status; i++) {
	  c = read_buffer[i];
	  if (c == LF || c == CR)
	    safe_write(STDOUT_FILENO, crlf, 2, "terminal");
	  else
	    safe_write(STDOUT_FILENO, &c, 1, "terminal");
	}
      }
    }
    else if (fds[1].revents & POLLERR) {
      fprintf(stderr, "ERROR while polling socket%s", crlf);
      exit(1);
    }
    else if (fds[1].revents & POLLHUP) {
      fprintf(stderr, "ERROR socket connection hung up%s", crlf);
      exit(1);
    }
  }
  
  exit(0);
}
