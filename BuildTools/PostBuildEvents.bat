REM @echo off

REM Postbuild call command: call ..\BuildTools\PostBuildEvents.bat $(ProjectName) $(Platform) $(Configuration)

REM $(ProjectName), Ex: CUDAShader
set ProjectName=%1
REM $(Plateform), Ex: x64
set Plateform=%2
REM $(Configuration), Ex: Debug
set Configuration=%3

if %ProjectName% == CUDAShader goto :CUDAShader
if %ProjectName% == FractalesJulia goto :FractalesJulia
if %ProjectName% == FractalesUI goto :FractalesUI
goto :end

:CUDAShader
	REM Dll generated => copy to other projects folders
	set Source=..\CUDAShader\%Plateform%\%Configuration%\CUDAShader.dll
	set Target_FractalesJulia=..\FractalesJulia\bin\%Configuration%\net6.0-windows
	set Target_FractalesUI=..\FractalesUI\%Plateform%\%Configuration%

	copy %Source% %Target_FractalesJulia%
	copy %Source% %Target_FractalesUI%

	goto end

:FractalesJulia
	REM Nothing to do
	goto :end

:FractalesUI
	REM Copy SFML dlls
	set Source=..\Libraries\SFML-2.5.1\bin\*.dll
	set Target=..\FractalesUI\%Plateform%\%Configuration%
	
	copy %Source% %Target%

	goto :end

:end