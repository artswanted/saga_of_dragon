#include <discord_game_sdk/discord_game_sdk.h>
#include <discord/discord.h>
#include <string.h>

namespace discord
{

struct Application
{
    struct IDiscordCore* core;
    struct IDiscordActivityManager* activities;
    struct IDiscordRelationshipManager* relationships;
    struct IDiscordApplicationManager* application;
};

static Application g_kApp;
static IDiscordActivityEvents g_kActivitiesEvents;
static DiscordActivity g_kActivities;
static bool g_kIsInit = false;

static void UpdateActivityCallback(void* data, enum EDiscordResult result)
{
    //DISCORD_REQUIRE(result);
}

bool init(__int64 client_id)
{
    if (g_kIsInit)
        return false;

    memset(&g_kApp, 0, sizeof(g_kApp));
    memset(&g_kActivitiesEvents, 0, sizeof(g_kActivitiesEvents));
    memset(&g_kActivities, 0, sizeof(g_kActivities));

    DiscordCreateParams params;
    DiscordCreateParamsSetDefault(&params);
    params.client_id = client_id;
    params.flags = DiscordCreateFlags_NoRequireDiscord;
    params.event_data = &g_kApp;
    params.activity_events = &g_kActivitiesEvents;
    if (DiscordCreate(DISCORD_VERSION, &params, &g_kApp.core) != DiscordResult_Ok)
        return false;

    g_kApp.activities = g_kApp.core->get_activity_manager(g_kApp.core);
    g_kApp.application = g_kApp.core->get_application_manager(g_kApp.core);
    g_kIsInit = true;
    return true;
}

void activity_set_details(const char* details)
{
    if (g_kIsInit)
        strcpy_s(g_kActivities.details, details ? details : "");
}

void activity_set_state(const char* state)
{
    if (g_kIsInit)
        strcpy_s(g_kActivities.state, state ? state : "");
}

void activity_set_large_image(const char* image, const char* text)
{
    if (g_kIsInit)
    {
        strcpy_s(g_kActivities.assets.large_image, image ? image : "");
        strcpy_s(g_kActivities.assets.large_text, text ? text : "");
    }
}

void activity_set_small_image(const char* image, const char* text)
{
    if (g_kIsInit)
    {
        strcpy_s(g_kActivities.assets.small_image, image ? image : "");
        strcpy_s(g_kActivities.assets.small_text, text ? text : "");
    }
}

void update_activity()
{
    if (g_kIsInit)
        g_kApp.activities->update_activity(g_kApp.activities, &g_kActivities, &g_kApp, UpdateActivityCallback);
}

bool tick()
{
    if (!g_kIsInit)
        return false;
    return g_kApp.core->run_callbacks(g_kApp.core) != DiscordResult_Ok;
}

} // namespace discord
