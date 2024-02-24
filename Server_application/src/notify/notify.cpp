#include "notify.hpp"
CurlWebNotifier::CurlWebNotifier(std::string const& url_address) : Notifier(url_address)
{
    curl_handler = curl_easy_init();
    if (curl_handler) 
    {
        curl_easy_setopt(curl_handler, CURLOPT_URL, url.c_str());
    }
    api_key = read_api_key_from_file();
}

std::string CurlWebNotifier::read_api_key_from_file(void)
{
    std::ifstream file("../inc/apikey.txt", std::ios::binary);
    std::string line;
    if (file.is_open()) 
    {
        if (std::getline(file, line)) { // Read one line from the file
            std::cout << "Api key read from file" << std::endl;
        } else 
        {
            std::cerr << "Failed to read api key from file!" << std::endl;
        }
    } else 
    {
        std::cerr << "Failed to open file!" << std::endl;
    }
    return line;
}

void CurlWebNotifier::post_data(const std::string& title, const std::string& message) 
{
    std::string postData = "ApiKey=" + api_key + "&PushTitle=" + title + "&PushText=" + message;
    curl_easy_setopt(curl_handler, CURLOPT_POSTFIELDS, postData.c_str());
    curl_easy_setopt(curl_handler, CURLOPT_POSTFIELDSIZE, postData.length());
    if (CURLcode res = curl_easy_perform(curl_handler); res != CURLE_OK)
    {
        std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
    }
}