@echo off
setlocal
chcp 65001 >NUL
cd /d "%~dp0"

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VS_PATH="
if exist "%VSWHERE%" for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VS_PATH=%%i"
if defined VS_PATH call "%VS_PATH%\VC\Auxiliary\Build\vcvars64.bat" >NUL

where cl >NUL 2>NUL
if %errorlevel%==0 goto msvc

where g++ >NUL 2>NUL
if %errorlevel%==0 goto mingw
goto no_compiler

:msvc
cl /std:c++17 /EHsc /W4 /utf-8 /MT main.cpp Entity.cpp Exceptions.cpp ConsoleUtils.cpp Dish.cpp User.cpp Payment.cpp Order.cpp StringUtils.cpp DeliverySystemApp.cpp /Fe:FoodDelivery.exe
if errorlevel 1 goto failed
goto success

:mingw
g++ -std=c++17 -Wall -Wextra -pedantic -finput-charset=UTF-8 -fexec-charset=UTF-8 main.cpp Entity.cpp Exceptions.cpp ConsoleUtils.cpp Dish.cpp User.cpp Payment.cpp Order.cpp StringUtils.cpp DeliverySystemApp.cpp -o FoodDelivery.exe
if errorlevel 1 goto failed
goto success

:success
echo Программа успешно собрана: FoodDelivery.exe
pause
exit /b 0

:no_compiler
echo Не найден компилятор MSVC или g++.
pause
exit /b 1

:failed
echo Ошибка компиляции.
pause
exit /b 1
