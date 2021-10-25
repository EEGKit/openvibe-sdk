setlocal
setlocal enableextensions

set "OV_PATH_ROOT=%~dp0.."
set "PATH=%OV_PATH_ROOT%/bin;%PATH%"
set "TEMPLATE_DIR=sphinx	emplates"
if not exist %TEMPLATE_DIR% (md %TEMPLATE_DIR%)
call openvibe-plugin-inspector-sdk --box-doc-directory sphinx/templates
move sphinx\templates\index-boxes.rst sphinx\source\boxes
pushd sphinx
call make.bat html
popd
endlocal
if exist ..\doc (rmdir /s /q ..\doc)
mkdir ..\doc
move sphinx\build\html "..\doc\OpenViBE Manual"

