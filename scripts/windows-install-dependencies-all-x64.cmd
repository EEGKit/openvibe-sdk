powershell.exe -NoProfile -ExecutionPolicy Bypass -file "windows-install-dependencies.ps1" -manifest_file .\windows-dependencies-x64.txt -dest_dir .\..\dependencies_x64
powershell.exe -NoProfile -ExecutionPolicy Bypass -file "windows-install-dependencies.ps1" -manifest_file .\tests-data.txt
pause
