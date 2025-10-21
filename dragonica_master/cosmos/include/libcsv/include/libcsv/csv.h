#pragma once
#include <string.h>
#include <cstdlib>
#include <stdlib.h>

// expand if necessary
#define MAX_CELL_SIZE 512

enum CsvFlag
{
	ECSV_FLAG_NONE = 0,
	ECSV_FLAG_CASE_INSENSITIVE = 1
};

enum CsvRowConst
{
	CSV_HEADER_ROW = 0
};

class CsvParser
{
private:
	const char* buffer;
	size_t len;
	size_t seek;
	size_t colCount;
	size_t rowIdx;
	char* cellBuffer;

public:
	CsvParser() : buffer(NULL), len(0), seek(0), colCount(0), rowIdx(0)
	{
		cellBuffer = NULL;
	}

	~CsvParser()
	{
		if (cellBuffer)
			delete cellBuffer;
	}

	bool load_from_buffer(const char* buffer, size_t len);
	bool next_row();
	const char* col(size_t index);
	inline bool is_empty() { return !buffer || seek >= len; }
	inline size_t col_count() { return colCount; }
	inline size_t row_idx() { return rowIdx; }

	inline int col_int(size_t index) { return atoi(col(index)); }
	inline __int64 col_int64(size_t index) { return _atoi64(col(index)); }
	int col_idx(const char* title, CsvFlag flag=ECSV_FLAG_CASE_INSENSITIVE);
};