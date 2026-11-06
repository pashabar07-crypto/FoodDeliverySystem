@echo off
chcp 65001 > nul
cd /d "%~dp0"
if not exist FoodDelivery.exe (
    echo Сначала запустите build.bat для сборки программы.
    pause
    exit /b 1
)
FoodDelivery.exe
pause
