#include <discord/discord.h>
#include <Windows.h>

int main(int argc, char** argv)
{
    discord::init(711882623558877185);
    discord::activity_set_details("dragonica-mercy.online");
    discord::activity_set_state("Port of The wind");
    discord::activity_set_large_image("logo", "Dragonica Mercy");
    discord::activity_set_small_image("woodie", "Dragonica Mercy");
    discord::update_activity();
    for (;;)
    {
        discord::tick();
        Sleep(16);
    }
    return 0;
}