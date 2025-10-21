#include "libupdate.h"
#include "libtorrent/session_settings.hpp"
#include "libtorrent/session.hpp"
#include "libtorrent/torrent_info.hpp"
#include "libtorrent/torrent_status.hpp"
#include "libtorrent/torrent_handle.hpp"
#include <memory>

namespace lt = libtorrent;

namespace libupdate
{

namespace torrent
{

void write_log(const char* msg)
{
}

static std::unique_ptr<lt::session> g_kSession;
static lt::torrent_handle g_kTorrentHandle;

bool init(const char* updatepath)
{
    if (!updatepath)
    {
        write_log("Updater path is empty!");
        return false;
    }

    if (g_kSession)
    {
        write_log("Already init!");
        return false;
    }

    lt::session_settings settings = lt::high_performance_seed();
    settings.user_agent = "Dragonica Mercy Updater v" LIBUPDATE_VERSION_STR;
    settings.anonymous_mode = false;
    settings.announce_to_all_trackers = true;
    settings.announce_to_all_tiers = true;

    g_kSession = std::make_unique<lt::session>();

    lt::add_torrent_params torrentParams;

    boost::system::error_code erc;
    torrentParams.save_path = ".";
    torrentParams.ti = boost::make_shared<lt::torrent_info>(updatepath, erc);
    torrentParams.flags |=
        lt::add_torrent_params::flag_paused
    ;

    if (erc)
    {
        g_kSession.reset();
        write_log(erc.message().c_str());
        return false;
    }

    g_kTorrentHandle = g_kSession->add_torrent(torrentParams, erc);
    if (erc)
    {
        g_kSession.reset();
        write_log(erc.message().c_str());
        return false;
    }
    return true;
}

bool pause()
{
    if (g_kSession)
    {
        g_kTorrentHandle.pause();
        return true;
    }
    return false;
}

bool resume()
{
    if (g_kSession)
    {
        g_kTorrentHandle.force_reannounce();
        g_kTorrentHandle.resume();
        return true;
    }
    return false;
}

bool force_reannounce()
{
    if (g_kSession)
    {
        g_kTorrentHandle.force_reannounce();
        return true;
    }
    return false;
}

bool deinit()
{
    if (g_kSession)
    {
        g_kSession.reset();
        return true;
    }
    return false;
}

state_t state()
{
    if (g_kSession)
    {
        return (state_t)g_kTorrentHandle.status().state;
    }
    return state_t::failed;
}

float progress()
{
    if (g_kSession)
    {
        return g_kTorrentHandle.status().progress;
    }
    return -1;
}

}

}