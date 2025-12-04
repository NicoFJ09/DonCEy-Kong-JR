@echo off
setlocal enabledelayedexpansion

echo ================================================
echo  Building DonCEy Kong Jr Client for Windows
echo ================================================

:: Get project root (assuming this script is in build-tools/)
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
set "CLIENT_DIR=%PROJECT_ROOT%\client"
set "DIST_DIR=%PROJECT_ROOT%\dist\windows\DonCEy-Kong-Jr-Client"

:: Check for gcc
where gcc >nul 2>nul
if errorlevel 1 (
    echo ERROR: gcc no esta instalado.
    echo.
    echo Para compilar en Windows, necesitas instalar un compilador de C:
    echo.
    echo Opcion 1: MinGW-w64 ^(Recomendado^)
    echo   Descarga desde: https://www.mingw-w64.org/downloads/
    echo   O usa MSYS2: https://www.msys2.org/
    echo   Con MSYS2: pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-raylib
    echo.
    echo Opcion 2: TDM-GCC
    echo   Descarga desde: https://jmeubank.github.io/tdm-gcc/
    echo.
    echo Despues de instalar, asegurate de que gcc este en PATH.
    echo.
    pause
    exit /b 1
)

:: Clean previous builds
echo ^>^> Cleaning previous builds...
if exist "%DIST_DIR%" rd /s /q "%DIST_DIR%"
mkdir "%DIST_DIR%"

:: Clean object files
echo ^>^> Cleaning object files...
if exist "%CLIENT_DIR%\build\obj" rd /s /q "%CLIENT_DIR%\build\obj"
mkdir "%CLIENT_DIR%\build\obj"

:: Build for Windows
echo ^>^> Compiling client...
cd /d "%CLIENT_DIR%"

:: Compile main and external files
echo   * Compiling source files...
gcc -c src/main.c -o build/obj/main.o -DPLATFORM_DESKTOP -D_WIN32 -std=c99 -Wall
if errorlevel 1 (
    echo ERROR: Failed to compile main.c
    pause
    exit /b 1
)

gcc -c src/external/cJSON.c -o build/obj/cJSON.o -std=c99 -Wall
if errorlevel 1 (
    echo ERROR: Failed to compile cJSON.c
    pause
    exit /b 1
)

:: Compile all game module files
echo   * Compiling game modules...
for %%f in (src\game\*.c) do (
    set "filename=%%~nf"
    echo     - !filename!.c
    gcc -c "%%f" -o "build\obj\!filename!.o" -DPLATFORM_DESKTOP -D_WIN32 -std=c99 -Wall
    if errorlevel 1 (
        echo ERROR: Failed to compile %%f
        pause
        exit /b 1
    )
)

:: Compile all game/screens module files
echo   * Compiling screen modules...
for %%f in (src\game\screens\*.c) do (
    set "filename=%%~nf"
    echo     - screens\!filename!.c
    gcc -c "%%f" -o "build\obj\!filename!.o" -DPLATFORM_DESKTOP -D_WIN32 -std=c99 -Wall
    if errorlevel 1 (
        echo ERROR: Failed to compile %%f
        pause
        exit /b 1
    )
)

:: Compile all network module files
echo   * Compiling network modules...
for %%f in (src\network\*.c) do (
    set "filename=%%~nf"
    echo     - network\!filename!.c
    gcc -c "%%f" -o "build\obj\!filename!.o" -DPLATFORM_DESKTOP -D_WIN32 -std=c99 -Wall
    if errorlevel 1 (
        echo ERROR: Failed to compile %%f
        pause
        exit /b 1
    )
)

:: Compile all rendering module files
echo   * Compiling rendering modules...
for %%f in (src\rendering\*.c) do (
    set "filename=%%~nf"
    echo     - rendering\!filename!.c
    gcc -c "%%f" -o "build\obj\!filename!.o" -DPLATFORM_DESKTOP -D_WIN32 -std=c99 -Wall
    if errorlevel 1 (
        echo ERROR: Failed to compile %%f
        pause
        exit /b 1
    )
)

:: Compile all utils module files
echo   * Compiling utils modules...
for %%f in (src\utils\*.c) do (
    set "filename=%%~nf"
    echo     - utils\!filename!.c
    gcc -c "%%f" -o "build\obj\!filename!.o" -DPLATFORM_DESKTOP -D_WIN32 -std=c99 -Wall
    if errorlevel 1 (
        echo ERROR: Failed to compile %%f
        pause
        exit /b 1
    )
)

:: Link executable
echo   * Linking executable...
gcc build\obj\*.o -o "%DIST_DIR%\DonCEy-Kong-Jr.exe" ^
    -lraylib -lopengl32 -lgdi32 -lwinmm -lws2_32 ^
    -static-libgcc -mwindows

if errorlevel 1 (
    echo ERROR: Linking failed
    echo.
    echo Asegurate de tener Raylib instalado:
    echo   Con MSYS2: pacman -S mingw-w64-x86_64-raylib
    echo   O descarga desde: https://www.raylib.com
    pause
    exit /b 1
)

:: Copy assets
echo ^>^> Copying assets...
xcopy /E /I /Y assets "%DIST_DIR%\assets"

:: Create README
echo ^>^> Creating README...
(
echo ============================================
echo  DonCEy Kong Jr - Cliente Windows
echo ============================================
echo.
echo REQUISITOS:
echo -----------
echo - Windows 7 o superior ^(64-bit^)
echo - OpenGL 3.3+ compatible ^(drivers de video actualizados^)
echo.
echo INSTALACION:
echo -----------
echo No requiere instalacion. Simplemente extrae todos los archivos
echo a una carpeta y ejecuta el launcher.
echo.
echo COMO EJECUTAR:
echo --------------
echo Doble-click en: DonCEy-Kong-Jr.exe
echo.
echo ESTRUCTURA:
echo -----------
echo DonCEy-Kong-Jr.exe --^> Ejecutable principal
echo assets/            --^> Recursos del juego ^(sprites, fuentes, etc.^)
echo README.txt         --^> Este archivo
echo.
echo SOLUCION DE PROBLEMAS:
echo ----------------------
echo * Error "OpenGL 3.3 not supported":
echo   --^> Actualiza los drivers de tu tarjeta grafica
echo   --^> Visita el sitio web del fabricante ^(NVIDIA, AMD, Intel^)
echo.
echo * Error "VCRUNTIME140.dll not found":
echo   --^> Instala Visual C++ Redistributable:
echo   --^> https://aka.ms/vs/17/release/vc_redist.x64.exe
echo.
echo * Pantalla negra o crash al iniciar:
echo   --^> Verifica que tu GPU soporte OpenGL 3.3+
echo   --^> Actualiza DirectX: https://www.microsoft.com/download/directx
echo.
echo * No encuentra assets:
echo   --^> Asegurate de que la carpeta assets/ esta en el mismo directorio
echo   --^> No muevas el .exe fuera de su carpeta
echo.
echo * Firewall bloquea conexiones:
echo   --^> Windows puede pedir permiso la primera vez
echo   --^> Click en "Permitir acceso" para conexiones de red
echo.
echo CONTROLES:
echo ----------
echo Flechas: Movimiento
echo Espacio: Saltar
echo ESC: Menu
echo.
echo CONECTAR AL SERVIDOR:
echo ---------------------
echo 1. Inicia el juego
echo 2. Ingresa la IP del servidor
echo 3. Si es local: 127.0.0.1
echo 4. Si es red local: IP de la PC servidor ^(ej: 192.168.1.X^)
echo 5. Puerto por defecto: 5000
echo.
echo SOPORTE:
echo --------
echo Para reportar bugs o solicitar ayuda, contacta al equipo de desarrollo.
echo.
echo ================================================
echo © 2024 DonCEy Kong Jr Team
echo ================================================
) > "%DIST_DIR%\README.txt"

echo.
echo ✓ Windows Client build complete!
echo   Location: %DIST_DIR%
echo.
echo Para probar:
echo   cd "%DIST_DIR%" ^&^& DonCEy-Kong-Jr.exe
echo.

endlocal
