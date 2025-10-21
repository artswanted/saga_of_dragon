#include "stdafx.h"
#include <discord/discord.h>
#include <PgScripting.h>
#include <lwWString.h>

static inline void discord_activity_set_details(lwWString details)
{
    char szMultiBuff[128]={0,};
    std::wstring &temp = (std::wstring &)details;
    WideCharToMultiByte(CP_UTF8, 0, temp.c_str(), temp.length(), szMultiBuff, sizeof(szMultiBuff), 0, 0);
    discord::activity_set_details(szMultiBuff);
}

static inline void discord_activity_set_state(lwWString state)
{
    char szMultiBuff[128]={0,};
    std::wstring &temp = (std::wstring &)state;
    WideCharToMultiByte(CP_UTF8, 0, temp.c_str(), temp.length(), szMultiBuff, sizeof(szMultiBuff), 0, 0);
    discord::activity_set_details(szMultiBuff);
}

LUA_REG_MODULE(Discord)
{
    def(pkState, "discord_activity_set_details", &discord_activity_set_details);
    def(pkState, "discord_activity_set_state", &discord_activity_set_state);
    def(pkState, "discord_activity_set_large_image", &discord::activity_set_large_image);
    def(pkState, "discord_activity_set_small_image", &discord::activity_set_small_image);
    def(pkState, "discord_update_activity", &discord::update_activity);
}