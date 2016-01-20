REM @echo off

SETLOCAL

SET "CWD=%~dp0"

if exist "%CWD%\..\..\..\dist-ninja-Release"\ (
	SET "OV_PATH_ROOT=%CWD%\..\..\..\dist-ninja-Release"
) else (
	SET "OV_PATH_ROOT=%CWD%\..\..\..\dist"
)

SET "OV_PATH_BIN=%OV_PATH_ROOT%\bin"
SET "OV_PATH_LIB=%OV_PATH_ROOT%\bin"
SET "OV_PATH_DATA=%OV_PATH_ROOT%\share\openvibe"
SET "MENSIA_PATH_DATA=%OV_PATH_ROOT%\share\mensia"
SET PATH=%OV_PATH_LIB%;%PATH%

call "%OV_PATH_BIN%\openvibe-set-env.cmd"

pushd src
start electron .
ENDLOCAL

popd
