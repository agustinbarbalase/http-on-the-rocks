#define _POSIX_C_SOURCE 200112L

#include "socket.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BACKLOG 20

typedef struct socket {
  int fd;
  bool is_closed;
} socket_t;

socket_t* socket_create(int fd) {
  socket_t* new_socket = (socket_t*) malloc(sizeof(socket_t));
  if (!new_socket) return NULL;

  new_socket->fd = fd;
  new_socket->is_closed = false;

  return new_socket;
}

struct addrinfo* resolve_service(const char* hostname, const char* servname, bool is_passive) {
  struct addrinfo hints, *result;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = is_passive ? AI_PASSIVE : 0;

  int status = getaddrinfo(NULL, servname, &hints, &result);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return NULL;
  }

  return result;
}

int resolve_file_descriptor(struct addrinfo* result,
                            bool(callback)(int fd, struct addrinfo* curr)) {
  int fd;
  struct addrinfo* curr;

  for (curr = result; curr != NULL; curr = curr->ai_next) {
    fd = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
    if (fd == -1) {
      continue;
    }

    if (callback(fd, curr)) {
      break;
    }
  }

  return fd;
}

bool resolve_callback_fd_server(int fd, struct addrinfo* curr) {
  int status = bind(fd, curr->ai_addr, curr->ai_addrlen);
  if (status == -1) {
    return false;
  }

  status = listen(fd, BACKLOG);
  if (status == -1) {
    return true;
  }

  return false;
}

socket_t* socket_create_server(const char* servname) {
  struct addrinfo* result = resolve_service(NULL, servname, true);
  int fd = resolve_file_descriptor(result, resolve_callback_fd_server);

  freeaddrinfo(result);

  return socket_create(fd);
}

bool resolve_callback_fd_client(int fd, struct addrinfo* curr) {
  int status = connect(fd, curr->ai_addr, curr->ai_addrlen);
  if (status == -1) {
    return true;
  }

  return false;
}

socket_t* socket_create_client(const char* hostname, const char* servname) {
  struct addrinfo* result = resolve_service(hostname, servname, false);
  int fd = resolve_file_descriptor(result, resolve_callback_fd_client);

  freeaddrinfo(result);

  return socket_create(fd);
}

socket_t* socket_accept(socket_t* skt) {
  int peer_addr = accept(skt->fd, NULL, NULL);
  if (peer_addr == -1) return NULL;
  return socket_create(peer_addr);
}

void socket_recv(socket_t* skt, void* data, size_t length, bool* was_closed) {
  size_t bytes_recv = 0;
  *was_closed = false;

  do {
    ssize_t curr_bytes_recv = recv(skt->fd, (char*) data + bytes_recv, length - bytes_recv, 0);
    if (curr_bytes_recv == -1) {
      perror("Error: socket recv");
      *was_closed = true;
      return;
    } else if (curr_bytes_recv == 0) {
      *was_closed = true;
      return;
    }

    bytes_recv += (size_t) curr_bytes_recv;

  } while (bytes_recv < length);
}

void socket_send(socket_t* skt, const void* data, size_t length, bool* was_closed) {
  size_t bytes_send = 0;
  *was_closed = false;

  do {
    ssize_t curr_bytes_send = send(skt->fd, (char*) data + bytes_send, length - bytes_send, 0);
    if (curr_bytes_send == -1) {
      perror("Error: socket recv");
      *was_closed = true;
      skt->is_closed = true;
      return;
    } else if (curr_bytes_send == 0) {
      *was_closed = true;
      skt->is_closed = true;
      return;
    }

    bytes_send += (size_t) curr_bytes_send;

  } while (bytes_send < length);
}

void socket_destroy(socket_t* skt) {
  if (!skt) return;

  if (!skt->is_closed) {
    skt->is_closed = true;
    shutdown(skt->fd, SHUT_RDWR);
  }

  free(skt);
}