# GitHub Actions - Compilación Automática Multi-Plataforma

## 🚀 Cómo Funciona

Este workflow compila **automáticamente** DonCEy Kong Jr para **Linux, Windows y macOS** usando los runners de GitHub.

### ✅ Qué Se Genera

**Cliente**:
- 🐧 **Linux**: `DonCEy-Kong-Jr-Client-Linux.tar.gz`
- 🪟 **Windows**: `DonCEy-Kong-Jr-Client-Windows.zip`
- 🍎 **macOS**: `DonCEy-Kong-Jr-Client-macOS.dmg` + `.app`

**Servidor**:
- ☕ **Universal JAR**: `DonCEy-Kong-Jr-Server-Universal.zip` (funciona en cualquier OS con Java)
- 🍎 **macOS Bundle**: `DonCEy-Kong-Jr-Server-macOS.dmg` + `.app`

## 📝 Uso

### 1️⃣ Activar el Workflow

```bash
# Desde tu Mac, sube el workflow a GitHub
git add .github/workflows/build-all-platforms.yml
git commit -m "Add multi-platform build workflow"
git push origin main
```

**¡Eso es todo!** GitHub Actions detecta automáticamente el archivo y comienza a compilar.

### 2️⃣ Ver el Progreso

1. Ve a tu repo en GitHub: `https://github.com/NicoFJ09/DonCEy-Kong-JR`
2. Click en la pestaña **"Actions"**
3. Verás el workflow ejecutándose con 6 jobs en paralelo:
   - ✅ build-client-linux
   - ✅ build-client-windows
   - ✅ build-client-macos
   - ✅ build-server (universal)
   - ✅ build-server-macos
   - ✅ create-release (solo en releases)

### 3️⃣ Descargar los Ejecutables

**Opción A: Desde "Actions" (builds de desarrollo)**

1. Click en el workflow completado
2. Scroll down hasta **"Artifacts"**
3. Descarga los ZIPs/DMGs/tar.gz que necesites

**Opción B: Desde "Releases" (versiones oficiales)**

Cuando crees un release/tag, los ejecutables se adjuntan automáticamente:

```bash
# Crear una release
git tag v1.0.0
git push origin v1.0.0
```

Luego en GitHub:
1. Ve a **"Releases"**
2. Click en la release `v1.0.0`
3. Los ejecutables estarán adjuntos para descarga pública

## 🎯 Triggers (Cuándo Se Ejecuta)

El workflow se ejecuta automáticamente cuando:

- ✅ Haces `git push` a `main` o `develop`
- ✅ Creas un Pull Request
- ✅ Creas un Release/Tag
- ✅ **Manual**: Click en "Run workflow" en GitHub UI

## 🔧 Ejecución Manual

Si quieres compilar sin hacer push:

1. Ve a **Actions** en GitHub
2. Selecciona **"Build All Platforms"**
3. Click en **"Run workflow"** (botón a la derecha)
4. Selecciona la rama
5. Click **"Run workflow"** (botón verde)

## 📦 Estructura de Artifacts

```
Artifacts disponibles después de cada build:

client-linux/
└── DonCEy-Kong-Jr-Client-Linux.tar.gz
    ├── DonCEy-Kong-Jr-Client (binary)
    ├── assets/ (sprites, fonts, etc)
    └── launch-client.sh

client-windows/
└── DonCEy-Kong-Jr-Client-Windows.zip
    ├── DonCEy-Kong-Jr-Client.exe
    ├── assets/
    └── launch-client.bat

client-macos-dmg/
└── DonCEy-Kong-Jr-Client-macOS.dmg

client-macos-app/
└── DonCEy Kong Jr.app/

server-universal/
└── DonCEy-Kong-Jr-Server-Universal.zip
    ├── DonCEy-Kong-Jr-Server.jar
    ├── assets/
    ├── launch-server.sh (Linux/Mac)
    └── launch-server.bat (Windows)

server-macos-dmg/
└── DonCEy-Kong-Jr-Server-macOS.dmg

server-macos-app/
└── DonCEy Kong Jr Server.app/
```

## ⚙️ Configuración

### Cambiar Versión de Java

Edita `.github/workflows/build-all-platforms.yml`:

```yaml
- name: Setup Java
  uses: actions/setup-java@v4
  with:
    distribution: 'temurin'
    java-version: '17'  # Cambia aquí
```

### Cambiar Versión de Raylib

```yaml
- name: Install Raylib
  run: |
    git clone --depth 1 --branch 5.0  # Cambia la versión aquí
```

### Compilar Solo para Algunas Plataformas

Comenta los jobs que no necesites:

```yaml
jobs:
  # build-client-windows:  # Desactivado
  #   runs-on: windows-latest
  #   ...
  
  build-client-linux:  # Activo
    runs-on: ubuntu-latest
    ...
```

## 🐛 Troubleshooting

### "Workflow not found"
- Asegúrate de que `.github/workflows/build-all-platforms.yml` esté en tu repo
- Verifica que el archivo esté en la rama `main`

### "Build failed"
- Click en el job que falló
- Expande los logs para ver el error específico
- Errores comunes:
  - Dependencias faltantes → El workflow las instala automáticamente
  - Errores de compilación → Revisa el código localmente primero

### "Artifacts expired"
- Los artifacts duran 90 días por defecto
- Para distribución permanente, usa Releases en lugar de Artifacts

## 💰 Costos

**GitHub Actions es GRATIS para repositorios públicos** con límites generosos:
- 2000 minutos/mes de Linux
- Minutos ilimitados para repos públicos

Este workflow toma ~10-15 minutos por build completo.

## 🎉 Resultado

Ahora tienes **compilación automática multi-plataforma** sin necesitar:
- ❌ Una PC con Windows
- ❌ Una PC con Linux
- ❌ Instalar cross-compilers
- ❌ Configuraciones complejas

Solo:
- ✅ `git push`
- ✅ Esperar 10-15 minutos
- ✅ Descargar ejecutables para todas las plataformas

## 📚 Más Información

- [GitHub Actions Docs](https://docs.github.com/en/actions)
- [Workflow Syntax](https://docs.github.com/en/actions/using-workflows/workflow-syntax-for-github-actions)
- [Available Runners](https://docs.github.com/en/actions/using-github-hosted-runners/about-github-hosted-runners)
