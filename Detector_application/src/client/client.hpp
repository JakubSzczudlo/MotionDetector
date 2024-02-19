#ifndef CLIENT_HPP
#define CLIENT_HPP

#include<cstdint>
#include<string>
#include <opencv2/opencv.hpp>

class Client
{
    private:
        virtual int32_t create_socket(void) = 0;
        virtual void close_socket(void) = 0;
        virtual int32_t send_data(const cv::Mat& data_img) = 0;
    protected:
        const uint16_t port_soc;
        const std::string ip_soc;
        Client(const uint16_t port, const std::string& ip): port_soc(port), ip_soc(ip) {};
        virtual ~Client() = default;
    public:
        Client(Client &other) = delete;
};

class SimpleSocketClient : public Client
{
    private:
        int client_descriptor;
        int32_t create_socket(void) final;
        void close_socket(void) final;
    public:
        SimpleSocketClient(const uint16_t port, const std::string& ip) : Client(port, ip) {create_socket();};
        ~SimpleSocketClient() override {close_socket();};
        SimpleSocketClient(SimpleSocketClient &other) = delete;
        int32_t send_data(const cv::Mat& data_img) override;
};

#endif // CLIENT_HPP