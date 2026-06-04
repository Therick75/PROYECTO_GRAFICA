@echo off
cd /d "%~dp0"

echo Compilando proyecto...

set PATH=C:\msys64\ucrt64\bin;%PATH%

C:\msys64\ucrt64\bin\g++.exe src/main.cpp src/glad.c -o programa.exe -I include -L lib -lglfw3 -lgdi32 -lopengl32 -lwinmm -luser32

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Hubo un problema al compilar. Revisa el texto de arriba.
    pause
) else (
    echo [EXITO] Compilacion terminada. Abriendo programa...
    echo ==================================================
    programa.exe
    pause
)
