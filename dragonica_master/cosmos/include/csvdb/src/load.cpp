#include "csvdb/load.h"
#include <Lohengrin/LogGroup.h>

class CsvLoadProfiler
{
	BM::vstring m_kFilename;
	const UINT64 m_kStartTime;
public:
	CsvLoadProfiler(BM::vstring filename) :
		m_kStartTime(BM::GetTime64()),
		m_kFilename(filename) { }

	~CsvLoadProfiler()
	{
		const UINT64 uiDiff = BM::DiffTime(m_kStartTime, BM::GetTime64());
		INFO_LOG(BM::LOG_LV7, "[Profiler] Load " << m_kFilename << " elapsed " << uiDiff);
	}
};

bool csvdb::load(BM::Stream::STREAM_DATA &buff, CsvParser &csv, BM::vstring const filename)
{
	CsvLoadProfiler profiler(filename);

	INFO_LOG(BM::LOG_LV7, __FL__ << "Load CSV: " << filename);
	if (!BM::FileToMem(filename, buff))
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << "Can't open csv: " << filename);
		return false;
	}

    if (buff.size() == 0)
    {
        VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << "Empty csv: " << filename);
        return false;
    }

	if (!csv.load_from_buffer(&buff.at(0), buff.size()))
	{
		VERIFY_INFO_LOG(false, BM::LOG_LV1, __FL__ << "Failed to parse csv: " << filename);
		return false;
	}

    return true;
}
