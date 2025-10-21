#include <curl/curl.h>
#include <telegram/telegram.h>
#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        std::cout << "usage: " << argv[0] << " <token> <chat_id> <message>" << std::endl;
        return -1;
    }

    curl_global_init(CURL_GLOBAL_ALL);
    telegram::init(argv[1]);
    telegram::send_message(atoi(argv[2]), argv[3]);
    telegram::term();
    curl_global_cleanup();
    return 0;
}