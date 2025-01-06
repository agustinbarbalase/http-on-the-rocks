typedef enum { GET } method_t;

typedef struct request {
  method_t method;
  char* route;
} request_t;
