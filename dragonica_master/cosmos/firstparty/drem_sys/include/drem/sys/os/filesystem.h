#pragma once
#include <Windows.h>

namespace drem
{

enum
{
    FLAG_EMPTY = 0,
    FLAG_REMOVE_RECURSIVE = 1,
};

bool rm_dir(const char* path, int flag = FLAG_EMPTY);
bool is_dir(const char* path);
bool mkdir(const char* path);
bool fs_each(const char* path, bool(*iterator)(WIN32_FIND_DATAA const&));

} // namespace drem
