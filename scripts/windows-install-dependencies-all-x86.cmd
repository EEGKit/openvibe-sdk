powershell.exe -NoProfile -ExecutionPolicy Bypass -file "windows-install-dependencies.ps1" -manifest_file .\windows-dependencies-x86.txt -dest_dir .\..\dependencies_x86
powershell.exe -NoProfile -ExecutionPolicy Bypass -file "windows-install-dependencies.ps1" -manifest_file .\tests-data.txt
pause
