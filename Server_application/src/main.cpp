
#include <opencv2/opencv.hpp>
#include "server.hpp"
#include "notify.hpp"


int main(int argc, char **argv)
{
	const int PORT = 8080;
	std::string IP{"192.168.0.38"};
	auto notify_handler = CurlWebNotifier("https://www.notifymydevice.com/push");
	auto server = SocketServer(PORT, IP, &notify_handler);
	server.accept_connection();

    return 0;
}
