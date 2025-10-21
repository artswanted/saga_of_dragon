#pragma once

#ifndef LIBUPDATE_EXPORT
#   define LIBUPDATE_EXPORT __declspec( dllexport )
#endif

#define LIBUPDATE_VERSION 100
#define LIBUPDATE_VERSION_STR "1.0.0"

namespace libupdate
{

namespace torrent
{

// Copy from libtorrent/torrent_status.hpp
enum state_t
{
    failed = -1, // Custom error code
#ifndef TORRENT_NO_DEPRECATE
	// The torrent is in the queue for being checked. But there
	// currently is another torrent that are being checked.
	// This torrent will wait for its turn.
	queued_for_checking,
#else
	// internal
	unused_enum_for_backwards_compatibility,
#endif

	// The torrent has not started its download yet, and is
	// currently checking existing files.
	checking_files,

	// The torrent is trying to download metadata from peers.
	// This assumes the metadata_transfer extension is in use.
	downloading_metadata,

	// The torrent is being downloaded. This is the state
	// most torrents will be in most of the time. The progress
	// meter will tell how much of the files that has been
	// downloaded.
	downloading,

	// In this state the torrent has finished downloading but
	// still doesn't have the entire torrent. i.e. some pieces
	// are filtered and won't get downloaded.
	finished,

	// In this state the torrent has finished downloading and
	// is a pure seeder.
	seeding,

	// If the torrent was started in full allocation mode, this
	// indicates that the (disk) storage for the torrent is
	// allocated.
	allocating,

	// The torrent is currently checking the fastresume data and
	// comparing it to the files on disk. This is typically
	// completed in a fraction of a second, but if you add a
	// large number of torrents at once, they will queue up.
	checking_resume_data
};

/**
 * @brief Initializate libupdate
 * Create a libtorrent session
 * @param updatepath .torrent file path
 * @return true if success
 */
LIBUPDATE_EXPORT bool init(const char* updatepath);

/**
 * @brief Pause update download
 *
 * @return true if success
 */
LIBUPDATE_EXPORT bool pause();

/**
 * @brief Resume update download and reannounce trackers
 *
 * @return true if success
 */
LIBUPDATE_EXPORT bool resume();

/**
 * @brief Froce reannounce trackers
 *
 * @return true if success
 */
LIBUPDATE_EXPORT bool force_reannounce();

/**
 * @brief Return a current torrent state
 * @return Same with libtorrent
 */
LIBUPDATE_EXPORT state_t state();

/**
 * @brief Download Progress
 * @return -1 if error, else value 0..1
 */
LIBUPDATE_EXPORT float progress();

/**
 * @brief De initializate torrent
 * Stop all torrents, reset session
 * @return true if success
 */
LIBUPDATE_EXPORT bool deinit();

}

}