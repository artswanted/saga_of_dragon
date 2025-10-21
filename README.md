# Open source "Saga of the Dragon"

This is the free source code of the private server "Saga of the Dragon".

## Project struct

In order for everything to work as the author intended, your project structure should look like this

```
.
├── database
├── dragonica_exe
│   ├── ToolBin
│   ├── archangel_run
│   └── sfreedom_dev
└── dragonica_master
```

### Description 

| Folder | Description|
| ------------- |-------------|
| ToolBin | Built Dragonica tools |
| archangel_run | Server files |
| sfreedom_dev | Client files |
| dragonica_master | Source code |
| database | database Backup |

## Installation

### Client, server and source code downloads

1. **To begin**, we need to install [git](https://git-scm.com/downloads) in order to download files.
2. **Once git has been successfully installed**, *navigate to the folder* where you want to download files.
3. *Right-click* on an empty space inside this folder and select the option `Git Bash Here`.
   - If you are using Windows 11, click on `Show More option` first.
4. **Copy and paste** the following commands into the opened window:
    ```sh
    git clone https://github.com/artswanted/saga_of_dragon.git
    cd saga_of_dragon
    git submodule update --init --recursive --progress
    ```
5. *Wait for the download* to complete.

## Update

1. **To begin**, we need to install [git](https://git-scm.com/downloads) in order to download files.
2. **Once git has been successfully installed**, *navigate to the folder* where you want to download files.
3. *Right-click* on an empty space inside `project_d` folder and select the option `Git Bash Here`.
   - If you are using Windows 11, click on `Show More option` first.
4. **Copy and paste** the following commands into the opened window:
    ```sh
    git pull
    git submodule update --init --recursive --progress
    ```
5. *Wait for the download* to complete.

### Setting up the server environment

1. **To begin**, you need to download and install [SQL Server Express](https://www.microsoft.com/en-us/sql-server/sql-server-downloads).
2. **Next**, download and install [Python 3.11](https://www.python.org/downloads/). We will need it for server configuration. Set check box `Add python.exe to PATH`, that very important!
3. Install [SQL Server Management Studio](https://learn.microsoft.com/en-us/sql/ssms/download-sql-server-management-studio-ssms?view=sql-server-ver16#download-ssms)
4. Launch SQL Server Management Studio.

5. You should see a window that looks like this:
   > ![Auth](./images/tutorial/sql01.png)

6. Click the "Connect" button.
7. After successfully connecting, we need to restore the databases.
8. MSSQL configuration is complete!
9. Now, we need to configure the server.
10. Open the folder `dragonica_Exe\archangel_run`.
11. *Right-click* on an empty space inside this folder and select the option `Git Bash Here`.
   - If you are using Windows 11, click on `Show More option` first.
12. Copy and paste the following commands into the opened window, after replacing `SERVER_NAME\\SQLEXPRESS` with your actual SQL Server name, which you can find in the "Connect" window.
Use `\\` instead of `\`!
    ```sh
    cd Tools
    python.exe db_cfg_gen.py --id sa --pw 123456 --addr SERVER_NAME\\SQLEXPRESS
    ```
1.  The basic server configuration is complete!
2.  Next, run MMC.bat, SMC.bat, and ServerControl_Release.exe.
3.  The login credentials for accessing the server admin panel are:
    Username: 1
    Password: 1
4.  After starting all servers in the admin panel, you can log in using the client located in `dragonica_Exe\sfreedom_dev`.
5.  To start the client, use `dragonica_local.cmd`.
6.  The login credentials for the game are:
    Username: 1
    Password: 1

### Setting up the environment for working with the source code

1. Download and install [Visual Studio 2022 Community](https://visualstudio.microsoft.com/vs/).
2. Install Desktop development with C++ component from Workloads tab
3. Install the following individual components:
   - C++ CMake tools for Windows
   - MSVC v143 2022 C++ x64/x86 build tools (Latest)
   - C++ ATL for latest v143 build tools (x86 & x64)
   - C++ MFC for latest v143 build tools (x86 & x64)
   - Windows 10 SDK 10.0.19041.0
   - Microsoft Foundation Classes for C++
4. Open Visual Studio 2022.
5. Select "Open Folder" from the start page.
6. Open the folder `dragonica_Exe\dragonica_master`.
7. Wait for the CMake project configuration to complete.

   ## Features

`Dev` is under development.

### Game

| Feature                              | Open |    Dev   |
| ------------------------------------ | :--: |  :-----: |
| Costume system                       |  ✅  |    ✅   |
| Mutators                             |  ✅  |    ✅   |
| Borderless window                    |  ✅  |    ✅   |
| Discord rich services                |  ✅  |    ✅   |
| Daily reward                         |  ✅  |    ✅   |
| Multi-Language support (RU/EN/FR)    |  ✅  |    ✅   |
| Escort mode mission                  |  ✅  |    ✅   |
| Freeze player instead of ban         |  ✅  |    ✅   |
| Unfreeze player by quest             |  ✅  |    ✅   |
| Fix client crash after duel          |  ✅  |    ✅   |
| BSQ reward for lose                  |  ✅  |    ✅   |
| User spam to chat exploit fix        |  ✅  |    ✅   |
| Empty username exploit fix           |  ✅  |    ✅   |
| Create drakan by special item        |  ✅  |    ✅   |
| Display exp percent in quest         |  ✅  |    ✅   |
| Save trade chat on server reload     |  ✅  |    ✅   |
| Weapon Kill track                    |  ✅  |    ✅   |
| Inventory second page                |  ✅  |    ✅   |
| Inventory indicator in UI            |  ✅  |    ✅   |
| Night mode                           |  ✅  |    ✅   |
| World PVP                            |  ❌  |    ✅   |
| Easy Anti Cheat                      |  ❌  |    ✅   |

### Developer

| Feature                              | Open |   Dev   |
| ------------------------------------ | :--: | :-----: |
| UI hot reload                        |  ✅  |    ✅   |
| SMC server crash notify to telegram  |  ✅  |    ✅   |
| CMake build system                   |  ✅  |    ✅   |
| CSV table instead of MSSQL           |  ✅  |    ✅   |
| Source converted to UTF-8            |  ✅  |    ✅   |
| GM API for python                    |  ✅  |    ✅   |
| Item Hot reload                      |  ✅  |    ✅   |
| Debug info for items                 |  ✅  |    ✅   |
| Skill info debug                     |  ✅  |    ✅   |
| Free camera                          |  ✅  |    ✅   |
| Game pause                           |  ✅  |    ✅   |
| Gitlab CI                            |  ❌  |    ✅   |
| Gamebryo Source code                 |  ❌  |    ✅   |
| New GM Tool Source code              |  ❌  |    ✅   |
| Patch maker tools                    |  ❌  |    ✅   |
