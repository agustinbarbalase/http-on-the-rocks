#include <stdio.h>

#include "socket.h"

int main() {
  socket_t* server = socket_create_server("8080");
  socket_t* new_connection = socket_accept(server);

  int i = 0;
  bool was_closed = false;
  while (!was_closed) {
    socket_recv(new_connection, &i, sizeof(i), &was_closed);
    if (was_closed) break;
    printf("Recived: %d\n", i);
  }

  socket_destroy(new_connection);
  socket_destroy(server);
}