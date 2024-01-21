#include "client.hpp"

#include <stdio.h>
#include <stdlib.h>	/* needed for os x*/
#include <string.h>	/* for strlen */
#include <netdb.h>      /* for gethostbyname() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> /* for printing an internet address in a user-friendly way */
#include <unistd.h>

#include <string>
#include <iostream>
#define MAX_SIZE 512U


void SimpleSocketClient::close_socket(void)
{
    close(client_descriptor); 
}

int32_t SimpleSocketClient::create_socket(void)
{
    struct sockaddr_in serv_addr; 
    
    if ((client_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
  
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port_soc); 
  
    // Convert IPv4 and IPv6 addresses from text to binary 
    // form 
    if (inet_pton(AF_INET, ip_soc.c_str(), &serv_addr.sin_addr) <= 0) 
	{ 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
    
    if ((connect(client_descriptor, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0) 
	{ 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    return 0; 
}

int32_t SimpleSocketClient::send_data(const cv::Mat& data_img)
{
    std::vector<uchar> send_buffer;
    cv::imencode(".jpg", data_img, send_buffer);
    size_t send_buffer_size = send_buffer.size();
    std::cout << send_buffer_size << std::endl;

    send(client_descriptor, static_cast<void*>(&send_buffer_size), sizeof(size_t), 0); 
    send(client_descriptor, send_buffer.data(), send_buffer_size, 0); 
    printf("Image sent\n"); 

    std::string reply_buffer;
    reply_buffer.reserve(MAX_SIZE);
    ssize_t valread = read(client_descriptor, (void*)reply_buffer.c_str(), MAX_SIZE); 
    std::cout << reply_buffer;
    return 0;
}
