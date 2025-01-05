#include <stdio.h>

#include "socket.h"

int main() {
  socket_t* server = socket_create_client("127.0.0.1", "8080");

  bool was_closed = false;
  for (int i = 0; i < 10; i++) {
    printf("Sending: %d\n", i);
    socket_send(server, &i, sizeof(i), &was_closed);
  }

  socket_destroy(server);
}