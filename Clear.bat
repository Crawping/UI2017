@echo off

set current_dir=%~dp0
set sln_dir=%current_dir%

::ɾ��BuildĿ¼
::rmdir /S /Q %sln_dir%\Build

::ɾ��DebugĿ¼
::del %sln_dir%\Bin\Debug\*.exe
::del %sln_dir%\Bin\Debug\*.dll
::del %sln_dir%\Bin\Debug\*.ilk

::���
cd %sln_dir%
git archive -o %current_dir%\UI2017.zip HEAD


:END

pause