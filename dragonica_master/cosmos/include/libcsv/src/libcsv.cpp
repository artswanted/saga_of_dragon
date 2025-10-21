#include "csv.h"
#include <string.h>
#include <algorithm>
#include <string>
#include <cctype>

#define COL_SEPARATOR ','
#define STRING_QUOTE '"'

static inline bool is_row_separator(char sym)
{
	return sym == '\n' || sym == '\r';
}

bool CsvParser::load_from_buffer(const char *buffer, size_t len)
{
	if (!buffer)
		return false;

	this->buffer = buffer;
	this->len = len;
	colCount = 0;
	seek = 0;

	bool insideStringQuote = false;
	for (size_t i = 0; i < len; i++)
	{
		if (is_row_separator(buffer[i]))
		{
			colCount += 1;
			break;
		}

		if (!insideStringQuote && buffer[i] == COL_SEPARATOR)
			colCount += 1;

		if (buffer[i] == STRING_QUOTE)
			insideStringQuote = !insideStringQuote;
	}

	cellBuffer = new char[colCount * MAX_CELL_SIZE];
	return true;
}

static size_t skip_quoted_text(const char* begin, size_t len)
{
	size_t i = 1/*skip first quote*/;
	for(; begin[i] != STRING_QUOTE && i < len; i++) { }
	return i + 1/*skip last quote*/;
}

bool CsvParser::next_row()
{
	if (is_empty())
		return false;

	++rowIdx;
	size_t i = seek;
	for(; i < len && !is_row_separator(buffer[i]); i++)
	{
		if (buffer[i] == STRING_QUOTE)
		{
			i++;
			for(; i < len && buffer[i] != STRING_QUOTE; i++) {}
		}
	}

	while(i < len && is_row_separator(buffer[i]))
		++i;

	seek = i;
	return seek < len;
}

const char* CsvParser::col(size_t index)
{
	if (index >= colCount)
		return NULL;

	size_t i = seek;
	size_t currentRowIndex = 0;
	for(; i < len && !is_row_separator(buffer[i]); i++)
	{
		if (currentRowIndex == index)
		{
			if (buffer[i] == STRING_QUOTE)
			{
				size_t j = 0;
				i++;
				for(; i < len && buffer[i] != STRING_QUOTE; i++, j++)
					cellBuffer[j] = buffer[i];
				cellBuffer[j] = 0;
				return cellBuffer;
			}

			size_t j = 0;
			for(; i < len && !is_row_separator(buffer[i]) && buffer[i] != COL_SEPARATOR; i++, j++)
				cellBuffer[j] = buffer[i];
			cellBuffer[j] = 0;
			return cellBuffer;
		}

		if (buffer[i] == COL_SEPARATOR)
			currentRowIndex++;

		if (buffer[i] == STRING_QUOTE)
		{
			i++;
			for(; i < len && buffer[i] != STRING_QUOTE; i++) {}
		}
	}
	return NULL; /*End of file*/
}

int CsvParser::col_idx(const char* title, CsvFlag flag)
{
	std::string sTitle(title);
	if (flag == ECSV_FLAG_CASE_INSENSITIVE)
		std::transform(sTitle.begin(), sTitle.end(), sTitle.begin(), tolower);
	for(size_t i = 0; i < colCount; ++i)
	{
		std::string sTitile2 = col(i);
		if (flag == ECSV_FLAG_CASE_INSENSITIVE)
			std::transform(sTitile2.begin(), sTitile2.end(), sTitile2.begin(), tolower);
		if (sTitle == sTitile2)
			return i;
	}
	return -1;
}

#undef COL_SEPARATOR
#undef STRING_QUOTE