#include <drem/sys/os/filesystem.h>
#include <Windows.h>

bool drem::rm_dir(const char* path, int flag)
{
    DWORD Attributes = GetFileAttributesA(path);
    if (Attributes == INVALID_FILE_ATTRIBUTES ||
        !(Attributes & FILE_ATTRIBUTE_DIRECTORY))
        return false;

    if ((flag & FLAG_REMOVE_RECURSIVE) == 0)
        return RemoveDirectoryA(path) == TRUE;

    char fullpath[MAX_PATH];
    if (strcpy_s(fullpath, path))
        return false;
    if (strcat_s(fullpath, "/*"))
        return false;

    WIN32_FIND_DATAA FindFileData;
    HANDLE hFind = FindFirstFileA(fullpath, &FindFileData);
    if (!hFind)
        return RemoveDirectoryA(path) == TRUE;

    do
    {
        if (strcmp(FindFileData.cFileName, ".") != 0 &&
            strcmp(FindFileData.cFileName, "..") != 0)
        {
            char filepath[MAX_PATH];
            if (strcpy_s(filepath, path))
                return false;
            if (strcat_s(filepath, "/"))
                return false;
            if (strcat_s(filepath, FindFileData.cFileName))
                return false;

            Attributes = GetFileAttributesA(filepath);
            if (Attributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if (!rm_dir(filepath, flag))
                    return false;
            }
            else
            {
                if (DeleteFileA(filepath) != TRUE)
                    return false;
            }
        }
    } while (FindNextFileA(hFind, &FindFileData));
    RemoveDirectoryA(path);
    return true;
}

bool drem::is_dir(const char* path)
{
    const DWORD ftyp = GetFileAttributesA(path);
    return ftyp == INVALID_FILE_ATTRIBUTES ? false :
        (ftyp & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
}

bool drem::mkdir(const char* path)
{
  return CreateDirectoryA(path, NULL) == TRUE;
}

bool drem::fs_each(const char* path, bool(*iterator)(WIN32_FIND_DATAA const&))
{
    WIN32_FIND_DATAA kFindData;
	HANDLE hFind = FindFirstFileA(path, &kFindData);
    if(INVALID_HANDLE_VALUE == hFind)
    {
        FindClose(hFind);
        return false;
    }

	do
	{
		if (!iterator(kFindData))
            break;
	} while(FindNextFileA(hFind, &kFindData));
    FindClose(hFind);
    return true;
}