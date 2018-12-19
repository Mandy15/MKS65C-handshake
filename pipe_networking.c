#include "pipe_networking.h"


/*=========================
  server_handshake
  args: int * to_client

  Performs the client side pipe 3 way handshake.
  Sets *to_client to the file descriptor to the downstream pipe.

  returns the file descriptor for the upstream pipe.
  =========================*/
int server_handshake(int *to_client) {
  int fd;
  if(mkfifo("wkp", 0644) == -1){
    printf("mkfifo Error: %s\n", strerror(errno));
    exit(0);
  }
  printf("WKP server FIFO created...\n");

  printf("Server waiting for client...\n");
  fd = open("wkp", O_RDONLY);
  char buf[HANDSHAKE_BUFFER_SIZE];
  read(fd, buf, HANDSHAKE_BUFFER_SIZE);
  printf("Server has received message from client...\n");
  printf("\"%s\"\n", buf);

  *to_client = open(buf, O_WRONLY);
  remove("wkp");
  printf("Server has removed WKP and connected to client...\n");

  write(*to_client, ACK, HANDSHAKE_BUFFER_SIZE);
  printf("Server has sent initial acknowledgement message to client...\n");

  read(fd, buf, HANDSHAKE_BUFFER_SIZE);
  printf("Server has received response from client...\n");
  printf("\"%s\"\n", buf);

  close(*to_client);
  free(buf);
  close(fd);
  printf("Server has closed all connections to client...\n");
  printf("Server is waiting for new client...\n");

  return fd;
}


/*=========================
  client_handshake
  args: int * to_server

  Performs the client side pipe 3 way handshake.
  Sets *to_server to the file descriptor for the upstream pipe.

  returns the file descriptor for the downstream pipe.
  =========================*/
int client_handshake(int *to_server) {
  int fd;
  if(mkfifo("private", 0644) == -1){
    printf("mkfifo Error: %s\n", strerror(errno));
    exit(0);
  }
  printf("Private client FIFO created...\n");

  *to_server = open("private", O_WRONLY);
  write(*to_server, "client", BUFFER_SIZE);
  printf("Client has connected to server and sent private FIFO name...\n");

  fd = open("private", O_RDONLY);
  char buf[HANDSHAKE_BUFFER_SIZE];
  read(fd, buf, HANDSHAKE_BUFFER_SIZE);
  printf("Client has received message from server and removed its private FIFO...\n");
  printf("\"%s\"\n", buf);
  remove("private");

  write(*to_server, ACK, HANDSHAKE_BUFFER_SIZE);
  printf("Client has sent response to server...\n");

  close(*to_server);
  free(buf);
  close(fd);
  printf("Client has exited...\n");

  return fd;
}
