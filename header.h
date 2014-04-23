#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>

#define ReportError(msg)       {perror(msg); exit(-1);}

// Predefined variables
long random(), M= 2147483647;
void srandom(unsigned);
void IntroduceError(char*, double);
char *GenerateData(int);
int Server(int port);
int Client(char* client_address, int port);
double floor(double);
char linebuffer[100];

// Added variables
int sd;
int run_server;
int next_node_socket;
int server_socket;

char node_name;

// Added functions
void start_network();
char* get_user_input();
char* create_frame(char dest, char* msg);
