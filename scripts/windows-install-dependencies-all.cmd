powershell.exe -NoProfile -ExecutionPolicy Bypass -file "windows-install-dependencies-generic.ps1" -manifest_file .\windows-dependencies.txt
powershell.exe -NoProfile -ExecutionPolicy Bypass -file "windows-install-dependencies-generic.ps1" -manifest_file .\tests-data.txt
pause