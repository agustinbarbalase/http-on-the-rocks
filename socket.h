#include <stdbool.h>
#include <stdlib.h>

typedef struct socket socket_t;

socket_t* socket_create(int fd);

socket_t* socket_create_server(const char* servname);

socket_t* socket_create_client(const char* hostname, const char* servname);

socket_t* socket_accept(socket_t* skt);

void socket_recv(socket_t* skt, void* data, size_t length, bool* was_closed);

void socket_send(socket_t* skt, void* data, size_t length, bool* was_closed);

void socket_destroy(socket_t* skt);