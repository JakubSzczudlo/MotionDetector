#ifndef NOTIFY_HPP
#define NOTIFY_HPP


#include <iostream>
#include <curl/curl.h>
#include <string>
#include <fstream>

class Notifier {
protected:
    const std::string url;
public:
    explicit Notifier(std::string const& url_address) : url(url_address) {};
    virtual ~Notifier() = default;
    virtual void post_data(const std::string& title, const std::string& message) = 0;
};

class CurlWebNotifier : public Notifier
{
private:
    const std::string api_key_filename;
    std::string api_key;
    std::string read_api_key_from_file(void);
    CURL* curl_handler;
public:
    explicit CurlWebNotifier(std::string const& url_address);
    CurlWebNotifier(CurlWebNotifier& other) = delete;
    void post_data(const std::string& title, const std::string& message) override;
    ~CurlWebNotifier() override {curl_easy_cleanup(curl_handler);};
};


#endif // NOTIFY_HPP