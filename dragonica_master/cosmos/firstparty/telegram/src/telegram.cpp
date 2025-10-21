#include "telegram/telegram.h"
#include <string>
#include <assert.h> // TODO: Custom assert for work in production mode
#include <curl/curl.h>
#include <stdio.h>
#include <iostream>

#define TG_API_URL "https://api.telegram.org/"

namespace telegram
{

static std::string g_kToken;

template<typename T>
struct auto_ptr
{
private:
    T* _data;
    void(*_deleter)(T*);

public:
    auto_ptr(T *data, void(*deleter)(T*)) :
        _data(data),
        _deleter(deleter)
        {}
    T* operator*() { return _data; }
    ~auto_ptr()
    {
        if (_deleter && _data)
            _deleter(_data);
    }
};

void init(const char* token)
{
    assert(token);
    g_kToken = token;
}

void term()
{
    g_kToken.clear();
}

ErrorCode send_message(int chat_id, const char* message)
{
    auto_ptr<CURL> curl(curl_easy_init(), curl_easy_cleanup);
    assert(*curl);
    if (!*curl)
        return TG_FAILED_INIT_CURL;

    if (g_kToken.size() == 0)
        return TG_EMPTY_TOKEN;

    char url[512];
    char body[2048];
    ::sprintf_s(url, sizeof(url), TG_API_URL "bot%s/sendMessage", g_kToken.c_str());
    ::sprintf_s(body, sizeof(body), "{\"chat_id\": \"%i\", \"text\": \"%s\", \"disable_notification\": true}", chat_id, message);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(*curl, CURLOPT_URL, url);
    curl_easy_setopt(*curl, CURLOPT_POST, 1);
    curl_easy_setopt(*curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(*curl, CURLOPT_POSTFIELDS, body);

    CURLcode res = curl_easy_perform(*curl);

    return res == CURLE_OK ? TG_OK : TG_CURL_ERROR;
}

}
#undef TG_API_URL
