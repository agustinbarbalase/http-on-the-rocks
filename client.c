#include <stdio.h>
#include <string.h>

#include "request.h"
#include "socket.h"

void create_request(request_t* request, char* request_str) {
  if (request->method == GET) {
    strcat(request_str, "GET ");
  }
  // if (request->method == POST) {
  //   strcpy(request_str, "POST ");
  // } else if (request->method == PUT) {
  //   strcpy(request_str, "PUT ");
  // } else if (request->method == DELETE) {
  //   strcpy(request_str, "DELETE ");
  else {
    fprintf(stderr, "Unsupported HTTP method\n");
    return;
  }

  strcat(request_str, request->route);
  strcat(request_str, " HTTP/1.1\r\nHost: 127.0.0.1\r\nConnection: close\r\n\r\n");
}

int main() {
  socket_t* server = socket_create_client("127.0.0.1", "8080");

  request_t* request = (request_t*) malloc(sizeof(request_t));
  request->method = GET;
  request->route = (char*) malloc(256 * sizeof(char));
  strcpy(request->route, "/");

  bool was_closed = false;
  char request_str[4096] = "";
  create_request(request, request_str);

  socket_send(server, request_str, sizeof(request_str) - 1, &was_closed);

  char response[4096];
  socket_recv(server, &response, sizeof(response) - 1, &was_closed);
  printf("Received:\n%s\n", response);

  free(request->route);
  free(request);

  socket_destroy(server);
}