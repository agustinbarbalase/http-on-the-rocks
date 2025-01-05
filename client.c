#include <stdio.h>
#include <string.h>

#include "socket.h"

int main() {
  socket_t* server = socket_create_client("127.0.0.1", "8080");

  bool was_closed = false;
  const char request[4096] = "GET / HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n";
  socket_send(server, request, sizeof(request) - 1, &was_closed);

  char response[4096];
  socket_recv(server, &response, sizeof(response) - 1, &was_closed);
  printf("Received:\n%s\n", response);

  socket_destroy(server);
}