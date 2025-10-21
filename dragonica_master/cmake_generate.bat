mkdir build/vs2022/
cd build/vs2022/
cmake ../../ -D CMAKE_BUILD_TYPE=RelWithDebInfo -G"Visual Studio 17 2022" -A Win32
