@echo off

set current_dir=%~dp0
set sln_dir=%current_dir%

::ɾ��BuildĿ¼
rmdir /S /Q %sln_dir%\Build

::ɾ��DebugĿ¼
del %sln_dir%\Bin\Debug\*.exe
del %sln_dir%\Bin\Debug\*.dll
del %sln_dir%\Bin\Debug\*.ilk

::�ύ
::git commit -m "temp"

::���
cd %sln_dir%
::git archive -o %current_dir%\UI2017.zip HEAD
"C:\Program Files (x86)\WinRAR\RAR.exe" a -k -r -s -m1 -x.git -x.vs %sln_dir%UI2017.zip 


:END

pause