@echo off
:: Close existing game to prevent "Return 5" errors
taskkill /f /im main.exe 2>nul

:: Compile the code
echo Compiling...
gcc src/main.c -o main.exe -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm

:: Check if compilation succeeded
if %errorlevel% == 0 (
    echo Build successful! Running game...
    main.exe
) else (
    echo Build failed!
)
pause
