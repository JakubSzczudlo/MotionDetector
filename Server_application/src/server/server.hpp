#ifndef SERVER_HPP
#define SERVER_HPP

#include <stdio.h>
#include <stdlib.h>	/* needed for os x */
#include <string.h>	/* for memset */
#include <sys/socket.h>
#include <arpa/inet.h>	/* for printing an internet address in a user-friendly way */
#include <netinet/in.h>
#include <sys/wait.h>    /* defines WNOHANG, for wait() */
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>

#include "notify.hpp"

#ifndef ERESTART
#define ERESTART EINTR
#endif

class SocketServer
{
    private:
        const uint16_t port;
        const std::string ip_address;
        int socket_handler;
        void run_session(int rqst);
        CurlWebNotifier* notifier;
    public:
        SocketServer(uint16_t port_number, std::string const & IP, CurlWebNotifier* notify);
        bool start_socket(void);
        void accept_connection(void);


};

#endif // SERVER_HPP