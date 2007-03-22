@echo off

if not exist "win32-init_env_command.cmd" (
	echo You have to configure your `win32-init_env_command` file.
	echo `win32-init_env_command-skeleton` is a good file to start with...
	goto terminate
)

call "win32-init_env_command.cmd"

set saved_directory=%CD%

mkdir ..\local-tmp > NULL
for /d %%s in (cmake\*.*) do (
	mkdir ..\local-tmp\%%s > NULL
	cd ..\local-tmp\%%s
	cmake %saved_directory%/%%s -G"NMake Makefiles"
	nmake
	cd %saved_directory%
)

:terminate

pause
