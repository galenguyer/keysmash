#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>
#include <sys/socket.h>	// socket
#include <sys/types.h>	// Not strictly needed, but potentially required for BSD
#include <netinet/in.h>	// sockaddr_in
#include <pthread.h>

#include "keysmash.h"

const int PORT = 8080;
const int BACKLOG = 1024;

struct request {
	char method[8];
	char path[2048];
};

void request_init(struct request* req) {
}

void request_parse(struct request* req, const char* data) {
	// Make a copy of the request data for internal use
	char idata[65535];
	strcpy(idata, data);

	// Copy the HTTP method to the request struct
	char *ptr = strtok(idata, " ");
	strcpy(req->method, ptr);
	req->method[strlen(ptr)] = '\0';

	// Copy the HTTP path to the request struct
	ptr = strtok(NULL, " ");
	strcpy(req->path, ptr);
	req->path[strlen(ptr)] = '\0';
}

void* client_handler(void* client_fd_ptr) {
	// Convert the void pointer input to an int for the client file descriptor
	int client_fd = *(int*) client_fd_ptr;
	// Create the response and request buffers
	char *response, request[65535];
	response = "\0";
	// Read the input into the request buffer and print it
	int read_length = recv(client_fd, request, 65535, 0);
	request[read_length] = '\0';

	// Initialize a request struct and parse the request data into it
	struct request req;
	request_init(&req);
	request_parse(&req, request);

	// print the request data
	printf("request{method:'%s', path:'%s'}\n", req.method, req.path);

	// Check that we're getting a GET request to the index route, else return a 404
	if (strcmp("GET", req.method) == 0) {
		if(strcmp("/", req.path) == 0) {
			response = "HTTP/1.1 200 OK\r\n"
				"Server: c/1.0\r\n"
				"Content-Type: text/plain\r\n"
				"\r\n"
				":ok:\r\n";
		}
	}
	// Set a 404 if none of the methods match
	if (strlen(response) <= 1) {
		response = "HTTP/1.1 404 Not Found\r\n"
			"Server: c/1.0\r\n"
			"Content-Type: text/plain\r\n"
			"\r\n"
			"F\r\n";
	}
	// Send the response and close the socket
	send(client_fd, response, strlen(response), 0);
	close(client_fd);
	return NULL;
}

int main() {
	/*
	 * Create the server socket
	 * The call for socket is as follows:
	 * int server_fd = socket(domain, type, protocol);
	 * domain refers to the address family. Some examples that could be used are
	 * 	AF_INET		used for IPv4
	 * 	AF_INET6	used for IPv6
	 * 	AF_UNIX		used for local sockets, similar to pipes
	 * type refers to the socket type, which could be
	 * 	SOCK_STREAM	used for TCP
	 * 	SOCK_DGRAM	used for UDP
	 * 	SOCK_RAW	used to create a raw socket
	 * protocol indicates a specific protocol for the domain and type of socket. 
	 * In this case, there's only one protocol possible, so 0 is used
	 */
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	// If socket create fails, denoted by returning -1, print an error and exit
	if (server_fd < 0) {
		perror("Could not create socket");
		exit(EXIT_FAILURE);
	}

	/*
	 * Set up the address for the socket to bind to
	 * sockaddr_in is a struct defined by the following:
	 *
	 * #include <netinet/in.h>
	 *
	 * struct sockaddr_in {
	 * 	short		sin_family;	// e.g. AF_INET
	 * 	unsigned short	sin_port;	// e.g. htons(3490)
	 * 	struct in_addr	sin_addr;	// see struct in_addr, below
	 * 	char		sin_zero[8];	// zero this if you want to
	 * };
	 *
	 * struct in_addr {
	 * 	unsigned long	s_addr;		// load with inet_aton()
	 * };
	 *
	 * sin_family refers to the address family used to set up the socket, in our case AF_INET
	 *
	 * sin_port is the port used to connect to the server. Normally it is manually assigned, but if set to 0
	 * the operating system will choose a random port
	 *
	 * sin_addr is a struct representing the listen address for the server. This is just the IP address to bind to.
	 * There's several ways to define it
	 * 	INADDR_ANY		binds to 0.0.0.0, or every availabile IPv4 address
	 * 	INADDR_LOOPBACK		binds to 127.0.0.1, the loopback IPv4 address
	 *	inet_addr("X.X.X.X") 	binds to X.X.X.X
	 * It can also be set using inet_aton(); as shown below
	 * inet_aton("X.X.X.X", &myaddr.sin_addr.s_addr);
	 *
	 * htonl and htons are function to convert the host address to network long address and host address to network short address
	 *
	 * #include <arpa/inet.h>
	 * * OR
	 * #include <netinet/in.h>
	 * uint32_t htonl(uint32_t hostlong);
	 * uint16_t htons(uint16_t hostshort);
	 */
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = htonl(INADDR_ANY);
	server_address.sin_port = htons(PORT);

	/*
	 * Bind the socket to the address we set up. Returns 0 on success or -1 on failure.
	 * Bind is defined by
	 * int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
	 * The first argument is the socket file descriptor created earlier.
	 * The second argument is a pointer to a sockaddr struct, but since we constructed a 
	 * sockaddr_in struct, it must be converted to the proper type.
	 * The third argument is the size of the address
	 */
	if (bind(server_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0) {
		perror("Could not bind to address or port");
		exit(EXIT_FAILURE);
	}

	/*
	 * listen() tells a server to listen to a socket and is defined as below:
	 * int listen(int sockfd, int backlog);
	 * It returns 0 on success and -1 on failure 
	 * The backlog argument specifies the number of connections that can be waiting before
	 * the system stops accepting new connection. If backlog is greater than the value in /proc/sys/net/ipv4/tcp_max_syn_backlog,
	 * it will be reduced to that value. 
	 */
	if (listen(server_fd, BACKLOG) < 0) {
		perror("Error listening to socket");
		exit(EXIT_FAILURE);
	}

	/*
	 * Accept the new connection. accept is defined as below:
	 * int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	 * The first argument is the original socket descriptor
	 * The second argument is the address struct that will be populated with the connecting client's address
	 * The third argument is the length of this struct, but cast to a socklen_t pointer
	 * addr and addrlen can be set to NULL. If addr is NULL, addrlen should be NULL as well.
	 * the accept() call will block until a client connects
	 */
	int client_fd, client_addr_len;
	struct sockaddr_in client_addr;
	client_addr_len = sizeof(client_addr);
	pthread_t thread_id;

	/*
	 * This continually spins off new threads for each request. Threading is quicker than forking 
	 * because the memeroy doesn't have to be copied, but for this kind of app the overhead is negligible.
	 * 
	 * accept is defined by the following signature
	 * 	int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
	 * sockfd is the server socket file descriptor. addr is a pointer to an empty client address and addrlen 
	 * is the length of that struct. accept returns the client socket file descriptor if successful, or -1 on failure
	 */
	while ((client_fd = accept(server_fd, (struct sockaddr*) &client_addr, (socklen_t*)&client_addr_len))) {
		if(client_fd < 0) {
			perror("Error accepting new connection");
			continue;
		}

		/*
		 * pthread_create is defined by the following method:
		 * 	#include <pthread.h>
		 * 	int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg);
		 *
		 * thread is a pointer to an empty pthread_t that allows future actions like join() on the thread.
		 * start_routine is a method to run in the thread and arg is an argument to pass into the thread
		 * pthread_create returns -1 on failure
		 * To compile and link it with the pthread library, use the -pthread flag with gcc
		 */	
		if(pthread_create(&thread_id, NULL, client_handler, (void*)&client_fd) < 0) {
			perror("Error creating handler thread");
		}
	}
	return 0;
}
