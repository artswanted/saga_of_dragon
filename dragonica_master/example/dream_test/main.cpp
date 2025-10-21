#include <drem/sys/os/filesystem.h>

int main(int argc, char** argv)
{
    drem::rm_dir("Z:/games/projectd/dragonica_exe/sfreedom_dev/Shader/Generated", drem::FLAG_REMOVE_RECURSIVE);
    return 0;
}