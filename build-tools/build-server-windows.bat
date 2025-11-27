@echo off
setlocal enabledelayedexpansion

echo ================================================
echo  Building DonCEy Kong Jr Server for Windows
echo ================================================

:: Get project root (assuming this script is in build-tools/)
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%.."
set "SERVER_DIR=%PROJECT_ROOT%\server"
set "DIST_DIR=%PROJECT_ROOT%\dist\windows\DonCEy-Kong-Jr-Server"

:: Clean previous builds
echo ^>^> Cleaning previous builds...
if exist "%DIST_DIR%" rd /s /q "%DIST_DIR%"
mkdir "%DIST_DIR%"

:: Build server
echo ^>^> Building server...
cd /d "%PROJECT_ROOT%"
if exist server\bin rd /s /q server\bin
mkdir server\bin

javac -d server\bin ^
    server\src\Main.java ^
    server\src\network\*.java ^
    server\src\gamestate\*.java ^
    server\src\ui\*.java ^
    server\src\utils\*.java ^
    server\src\game\*.java

if errorlevel 1 (
    echo ERROR: Compilation failed
    pause
    exit /b 1
)

:: Create JAR
echo ^>^> Creating JAR...
cd server\bin
echo Manifest-Version: 1.0> MANIFEST.MF
echo Main-Class: server.src.Main>> MANIFEST.MF

:: List all .class files recursively
dir /s /b *.class > classes.txt
jar cfm DonCEy-Kong-Jr-Server.jar MANIFEST.MF @classes.txt
del classes.txt

if errorlevel 1 (
    echo ERROR: JAR creation failed
    pause
    exit /b 1
)

cd ..\..

:: Copy JAR to distribution
echo ^>^> Copying JAR...
copy server\bin\DonCEy-Kong-Jr-Server.jar "%DIST_DIR%\"

:: Copy assets
echo ^>^> Copying assets...
xcopy /E /I /Y server\assets "%DIST_DIR%\assets"

:: Create launch batch script
echo ^>^> Creating launch script...
(
echo @echo off
echo setlocal
echo.
echo :: Check if Java is installed
echo java -version ^>nul 2^>^&1
echo if errorlevel 1 ^(
echo     echo ERROR: Java no esta instalado.
echo     echo.
echo     echo Por favor instala Java 11 o superior desde:
echo     echo https://adoptium.net
echo     echo.
echo     pause
echo     exit /b 1
echo ^)
echo.
echo :: Get directory of this script
echo set "DIR=%%~dp0"
echo cd /d "%%DIR%%"
echo.
echo :: Launch the server
echo echo Iniciando DonCEy Kong Jr Server...
echo echo Presiona Ctrl+C para detener el servidor
echo echo.
echo java -jar DonCEy-Kong-Jr-Server.jar
echo.
echo :: Keep window open on error
echo if errorlevel 1 ^(
echo     echo.
echo     echo El servidor termino con errores.
echo     pause
echo ^)
) > "%DIST_DIR%\Launch-Server.bat"

:: Create README
echo ^>^> Creating README...
(
echo ============================================
echo  DonCEy Kong Jr - Servidor Windows
echo ============================================
echo.
echo REQUISITOS:
echo -----------
echo - Windows 7 o superior ^(64-bit^)
echo - Java 11 o superior ^(JRE^)
echo.
echo INSTALACION DE JAVA:
echo --------------------
echo Descarga e instala Java desde:
echo https://adoptium.net
echo.
echo Asegurate de seleccionar "Add to PATH" durante la instalacion.
echo.
echo COMO EJECUTAR:
echo --------------
echo Doble-click en: Launch-Server.bat
echo.
echo ESTRUCTURA:
echo -----------
echo DonCEy-Kong-Jr-Server.jar  --^> Servidor ejecutable
echo Launch-Server.bat          --^> Script de lanzamiento
echo README.txt                 --^> Este archivo
echo.
echo CONFIGURACION:
echo --------------
echo El servidor inicia automaticamente en el puerto 5000.
echo.
echo Para cambiar el puerto, edita src/utils/Config.java y recompila.
echo.
echo INTERFAZ DE ADMINISTRACION:
echo ---------------------------
echo El servidor incluye una interfaz grafica ^(GUI^) para:
echo * Ver jugadores conectados
echo * Seleccionar jugador a observar
echo * Generar frutas en el mapa
echo * Generar enemigos ^(cocodrilos^)
echo * Ver eventos en tiempo real
echo.
echo USO:
echo ----
echo 1. Ejecuta el servidor ^(Launch-Server.bat^)
echo 2. Se abrira la interfaz grafica
echo 3. Los clientes pueden conectarse al puerto 5000
echo 4. Selecciona un jugador en el panel de administracion
echo 5. Usa los botones para generar frutas/enemigos
echo.
echo SOLUCION DE PROBLEMAS:
echo ----------------------
echo * Error "Java no esta instalado":
echo   --^> Instala Java 11+ ^(ver seccion INSTALACION DE JAVA^)
echo   --^> Verifica que Java esta en PATH: java -version
echo.
echo * Error "Puerto 5000 en uso":
echo   --^> Otro proceso esta usando el puerto
echo   --^> Usa netstat -ano ^| findstr :5000 para encontrar el proceso
echo   --^> Detén el proceso conflictivo desde el Administrador de tareas
echo.
echo * Error "Could not find or load main class Main":
echo   --^> El JAR esta corrupto, vuelve a compilar
echo.
echo * Interfaz no aparece:
echo   --^> Verifica que no hay errores en la ventana de consola
echo   --^> Verifica que Java GUI esta habilitado ^(no en modo headless^)
echo.
echo * Clientes no se conectan:
echo   --^> Verifica firewall de Windows
echo   --^> Permite el puerto 5000: Panel de Control ^> Firewall ^> Reglas
echo   --^> Verifica que el servidor esta escuchando: netstat -an ^| findstr :5000
echo.
echo * Firewall bloquea conexiones:
echo   --^> Windows puede pedir permiso la primera vez
echo   --^> Click en "Permitir acceso"
echo.
echo LOGS:
echo -----
echo Los mensajes del servidor se muestran en la ventana de consola.
echo Busca lineas que comiencen con [SERVER], [CLIENT], etc.
echo.
echo APAGAR EL SERVIDOR:
echo -------------------
echo * Desde la interfaz: Cierra la ventana
echo * Desde consola: Presiona Ctrl+C
echo * El servidor enviara mensaje de apagado a todos los clientes
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
echo ✓ Windows Server build complete!
echo   Location: %DIST_DIR%
echo.
echo To test:
echo   cd "%DIST_DIR%" ^&^& Launch-Server.bat
echo.

endlocal
