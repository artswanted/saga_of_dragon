#pragma once

namespace telegram
{

enum ErrorCode
{
    TG_OK,
    TG_FAILED,
    TG_EMPTY_TOKEN,
    TG_FAILED_INIT_CURL,
    TG_CURL_ERROR,
};

void init(const char* token);
void term();
ErrorCode send_message(int chat_id, const char* message);

}