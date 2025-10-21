#pragma once
#include <BM/Common.h>
#include <BM/filesupport.h>
#include <BM/Stream.h>
#include <libcsv/csv.h>

namespace csvdb
{

bool load(BM::Stream::STREAM_DATA &buff, CsvParser &csv, BM::vstring const filename);

}