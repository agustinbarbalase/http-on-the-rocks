#include <stdio.h>
#include <string.h>

#include "socket.h"

int main() {
  socket_t* server = socket_create_server("8080");
  socket_t* new_connection = socket_accept(server);

  if (new_connection == NULL) {
    fprintf(stderr, "Failed to accept new connection\n");
    socket_destroy(server);
    return 1;
  }

  printf("New connection\n");

  char request[4096] = "";
  bool was_closed = false;
  while (!was_closed) {
    socket_recv(new_connection, request, strlen(request), &was_closed);
    if (was_closed) break;
    printf("Received: \n%s\n", request);
  }

  const char* response = "HTTP/1.1 200 OK\r\n"
                         "Content-Type: text/plain\r\n"
                         "Content-Length: 13\r\n"
                         "\r\n"
                         "Hello, world!";

  socket_send(new_connection, response, strlen(response), &was_closed);

  socket_destroy(new_connection);
  socket_destroy(server);
}