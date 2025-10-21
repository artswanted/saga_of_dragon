@echo off

SET ROOT=%~dp0
SET DATA_DIR=%ROOT%\data

echo Make directory data for patcher %DATA_DIR%
mkdir data

copy %ROOT%\..\Patcher.exe %DATA_DIR% /a

echo Make patcher.id by Patcher.exe
%DATA_DIR%\Patcher.exe -initmake=%DATA_DIR%\patcher.id

echo Make initial patch.id of version 0.0.1 by MPPack
%ROOT%\..\..\Pack\MPPack.exe -create_id %DATA_DIR% 0 0 1
echo Make start.id of version 0.0.1 by copy patch.id
copy %DATA_DIR%\patch.id %ROOT%\..\ /a


echo Make start.id of version 0.0.1 by copy patch.id
copy %DATA_DIR%\patch.id %DATA_DIR%\start.id /a

pause