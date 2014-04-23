/*
 * The following code will put the standard input and the socket descriptor
 * in the select function to be inspected for reading.  When the select
 * function returns, the descriptors are inspected to find out which one
 * is ready.   This code can be modified to handle files and sockets which
 * become ready for writing.
 */

#include "header.h"

int main(int argc, char **argv)
{
    if(argc != 5)
    {
        printf("usage: ./tokennet [node_name] [node_port] [next_node] [next_port]\n");
        return 0;
    }

    node_name = argv[1][0];
    int node_port = atoi(argv[2]);
    char *next_address = argv[3];
    int next_port = atoi(argv[4]);

    printf("Node: %c\nNext address: %s\n", node_name, next_address);

    next_node_socket = 0;
    server_socket = 0;
    run_server = 1;

    // Connect to both client and server sockets
    while(1)
    {
        // Try and connect to next node
        if(next_node_socket <= 0)
        {
            printf("Attempting to connect to server...\n");
            next_node_socket = Client(next_address, next_port);
            if(next_node_socket > 0)
            {
                printf("Connected: %d\n", next_node_socket);
            }
            else
            {
                printf("Failed to connect: %d\n", next_node_socket);
            }
        }

        // Wait for client to connect to our server
        if(server_socket <= 0)
        {
            printf("Waiting for client to connect...\n");
            server_socket = Server(node_port);
            printf("Client connected: %d\n", server_socket);
        }

        printf("Client: %d\t\tServer: %d\n", next_node_socket, server_socket);
        sleep(2);
        if((next_node_socket > 0) && (server_socket > 0))
        {
            break;
        }
        // Wait 2 seconds before repeating
        //sleep(2);
    }

    printf("\n\nSocket connectoins completed!\nClient: %d\t\tServer: %d\n", next_node_socket, server_socket);

    printf("Starting network...\n");
    //sleep(10);
    start_network();
    printf("Complete!\n");
    return 0;
}

/* assume sd is the data channel socket & STDIN_FILENO is the
 * standard input file descriptor (it is 0)
 */
void start_network()
{
    char* in_buf;
    char* frame;
    int len = 0;
    in_buf = malloc(sizeof(char) * 88);
    frame = malloc(sizeof(char) * 88);

    // Make server socket non-blocking
    fcntl(server_socket, F_SETFL, O_NONBLOCK);

    while(1)
    {
        // Get message from user to send
        frame = get_user_input();
        printf("Sending frame: %s", frame);
        send(next_node_socket, frame, strlen(frame), NULL);

        // Check for data in the recv buffer
        len = recv(server_socket, in_buf, 88, NULL);
        if(len > 0)
        {
            printf("RECV FRAME: %d\n", len);
            printf("\tsrc addr: %c\n", in_buf[5]);
            printf("\tdst addr: %c\n", in_buf[4]);
            printf("\tmsg: %s\n", in_buf + 6);
        }
        else
        {
            printf("NO DATA: %d", len);
        }

        // clear the recv buffer
        memset(in_buf, 0, 88);
    }
}

char* get_user_input()
{
    // Declare variables for user input
    char dest[24];
    char msg[81];
    printf("Enter destinaton: ");
    fflush(stdout);
    fgets(dest, 24, stdin);
    printf("Enter Message:\n");
    fflush(stdout);
    fgets(msg, 81, stdin);

    return create_frame(dest, msg);
}

char* create_frame(char dest, char* msg)
{
    char* frame;
    frame = malloc(sizeof(char) * 88);

    // format frame header
    frame[0] = 0x16; // SYN
    frame[1] = 0x16; // SYN
    frame[2] = 0x10; // DLE
    frame[3] = 0x02; // STX

    // set frame addresses
    frame[4] = dest;
    frame[5] = node_name;

    // copy message into frame
    int msg_size = strlen(msg);
    strncpy(frame + 6, msg, msg_size);

    // set end frame
    frame[msg_size + 6] = 0x10;
    frame[msg_size + 7] = 0x03;

    return frame;
}

int Server(int port)
{
    struct sockaddr_in myaddr, otheraddr;

    int s, fd, otherlength;

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) ReportError ("socket");

    bzero(&myaddr, sizeof(myaddr));
    myaddr.sin_family  = AF_INET;
    myaddr.sin_port = htons(port);
    myaddr.sin_addr.s_addr = inet_addr("0.0.0.0");

    bind(s, &myaddr, sizeof(myaddr));

    listen(s, 1);

    otherlength = sizeof(otheraddr);
    fd = accept(s, &otheraddr, &otherlength);

    fprintf(stdout, "Connected");

    return(fd);
}

int Client(char* client_address, int port)
{
    int s;
    int n;

    struct sockaddr_in otheraddr;

    pid_t pid;

    bzero(&otheraddr, sizeof(otheraddr));

    otheraddr.sin_family = AF_INET;
    otheraddr.sin_port = htons(port);
    otheraddr.sin_addr.s_addr = inet_addr(client_address);

    s = socket(AF_INET, SOCK_STREAM, 0);
    n = connect(s, &otheraddr, sizeof(otheraddr));

    if ( n < 0)
        return(n);
    else
        return(s);
}
