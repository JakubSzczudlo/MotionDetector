#include "server.hpp"
#include <thread>
#include <sys/errno.h>   /* defines ERESTART, EINTR */

extern int errno;

void SocketServer::accept_connection(void)
{
	int rqst;       /* socket accepting the request */
	socklen_t alen;       /* length of address structure */
	struct sockaddr_in client_addr;  /* client's address */

	/* loop forever - wait for connection requests and perform the service */
	alen = sizeof(client_addr);     /* length of address */
	for (;;) 
	{
		while ((rqst = accept(socket_handler, (struct sockaddr *)&client_addr, &alen)) < 0) 
		{
			/* we may break out of accept if the system call */
			/* was interrupted. In this case, loop back and */
			/* try again */
			if ((errno != ECHILD) && (errno != ERESTART) && (errno != EINTR)) 
			{
				perror("accept failed");
				break;
			}
		}
		std::thread session_thread(&SocketServer::run_session, this, std::ref(rqst));
		session_thread.detach();
	}
}

[[noreturn]] void SocketServer::run_session(int rqst)
{
	static uint8_t img_counter = 0;
	while(true)
	{
		std::string img_save_name{"test_img"};
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
		img_counter += 1;
		img_save_name += std::to_string(img_counter);
		img_save_name += ".jpg";
		cv::imwrite(img_save_name, get_img);
	}
}

SocketServer::SocketServer(uint16_t port_number, std::string const &IP, CurlWebNotifier* notify) 
	: port(port_number), ip_address(IP), notifier(notify)
{
	bool status = start_socket();
	if(!status)
	{
		exit(1);
	}
}

bool SocketServer::start_socket(void)
{
	struct sockaddr_in my_addr;    /* address of this service */
	int sockoptval = 1;
	/* get a tcp/ip socket */

	if ((socket_handler = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{
		perror("cannot create socket");
		return false;
	}

	/* set socket option*/
	setsockopt(socket_handler, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));

	/* set up our address */
	memset((char*)&my_addr, 0, sizeof(my_addr));  /* 0 out the structure */
	my_addr.sin_family = AF_INET;   /* address family */
	my_addr.sin_port = htons(port);
	my_addr.sin_addr.s_addr = inet_addr(ip_address.c_str());

	/* bind to the address to which the service will be offered */
	if (bind(socket_handler, (struct sockaddr *)&my_addr, sizeof(my_addr)) < 0) 
	{
		perror("bind failed");
		return false;
	}

	/* set up the socket for listening with a queue length of 5 */
	if (listen(socket_handler, 5) < 0) 
	{
		perror("listen failed");
		return false;
	}
	return true;
}
