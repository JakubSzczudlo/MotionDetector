/*
	echoserver: TCP/IP sockets example
	get a connect: keep reading data from the socket, echoing
	back the received data.

	usage:	echoserver [-d] [-p port]
*/

#include <stdio.h>
#include <stdlib.h>	/* needed for os x */
#include <string.h>	/* for memset */
#include <sys/socket.h>
#include <arpa/inet.h>	/* for printing an internet address in a user-friendly way */
#include <netinet/in.h>
#include <sys/errno.h>   /* defines ERESTART, EINTR */
#include <sys/wait.h>    /* defines WNOHANG, for wait() */
#include <unistd.h>

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <vector>

#ifndef ERESTART
#define ERESTART EINTR
#endif

#define PORT 8080

extern int errno;

void serve(int port);	/* main server function */

int main(int argc, char **argv)
{

	int port = PORT;
	serve(port);
    return 0;
}

/* serve: set up the service */

void serve(int port)
{
	int svc;        /* listening socket providing service */
	int rqst;       /* socket accepting the request */
	socklen_t alen;       /* length of address structure */
	struct sockaddr_in my_addr;    /* address of this service */
	struct sockaddr_in client_addr;  /* client's address */
	int sockoptval = 1;
	char buffer[1024] = { 0 }; 

	/* get a tcp/ip socket */
	/*   AF_INET is the Internet address (protocol) family  */
	/*   with SOCK_STREAM we ask for a sequenced, reliable, two-way */
	/*   conenction based on byte streams.  With IP, this means that */
	/*   TCP will be used */

	if ((svc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("cannot create socket");
		exit(1);
	}

	/* we use setsockopt to set SO_REUSEADDR. This allows us */
	/* to reuse the port immediately as soon as the service exits. */
	/* Some operating systems will not allow immediate reuse */
	/* on the chance that some packets may still be en route */
	/* to the port. */

	setsockopt(svc, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

	/* set up our address */
	/* htons converts a short integer into the network representation */
	/* htonl converts a long integer into the network representation */
	/* INADDR_ANY is the special IP address 0.0.0.0 which binds the */
	/* transport endpoint to all IP addresses on the machine. */

	memset((char*)&my_addr, 0, sizeof(my_addr));  /* 0 out the structure */
	my_addr.sin_family = AF_INET;   /* address family */
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = inet_addr("192.168.0.38");

	/* bind to the address to which the service will be offered */
	if (bind(svc, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) {
		perror("bind failed");
		exit(1);
	}

	/* set up the socket for listening with a queue length of 5 */
	if (listen(svc, 5) < 0) {
		perror("listen failed");
		exit(1);
	}

	printf("server started, listening on port %d\n", port);

	/* loop forever - wait for connection requests and perform the service */
	alen = sizeof(client_addr);     /* length of address */
	for (;;) {
		while ((rqst = accept(svc, (struct sockaddr *)&client_addr, &alen)) < 0) 
		{
			/* we may break out of accept if the system call */
			/* was interrupted. In this case, loop back and */
			/* try again */
			if ((errno != ECHILD) && (errno != ERESTART) && (errno != EINTR)) {
				perror("accept failed");
				exit(1);
			}
		}
		size_t img_size;
		read(rqst, &img_size, sizeof(img_size));
		std::vector<uchar> img_buffer(img_size);
		std:: cout << img_size << std::endl;
		size_t valread = 0; 
		while(valread < img_size)
		{
			valread += read(rqst, img_buffer.data()+valread, img_size);
		}
		std::cout << valread << std::endl;
		cv::Mat get_img = cv::imdecode(img_buffer, cv::IMREAD_GRAYSCALE);

		cv::imwrite("img.jpg", get_img);

		printf("received a connection from: %s port %d\n",
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        	shutdown(rqst, 2);    /* close the connection */
	}
}
