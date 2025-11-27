#!/bin/bash
set -e

echo "================================================"
echo " Building DonCEy Kong Jr Server for macOS"
echo "================================================"

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SERVER_DIR="$PROJECT_ROOT/server"
DIST_DIR="$PROJECT_ROOT/dist/macos"
APP_NAME="DonCEy Kong Jr Server"
BUNDLE_ID="com.doncey.kongjr.server"
VERSION="1.0.0"

# Clean previous builds
echo "→ Cleaning previous builds..."
rm -rf "$DIST_DIR/$APP_NAME.app"
mkdir -p "$DIST_DIR"

# Build server
echo "→ Building server..."
cd "$PROJECT_ROOT"
rm -rf server/bin
mkdir -p server/bin

javac -d server/bin \
    server/src/Main.java \
    server/src/network/*.java \
    server/src/gamestate/GameSession.java \
    server/src/ui/*.java \
    server/src/utils/*.java \
    server/src/game/*.java

# Create JAR
echo "→ Creating JAR..."
cd server/bin
cat > MANIFEST.MF << EOF
Manifest-Version: 1.0
Main-Class: server.src.Main
EOF

jar cfm DonCEy-Kong-Jr-Server.jar MANIFEST.MF $(find . -name '*.class')
cd ../..

# Create .app bundle structure
echo "→ Creating .app bundle..."
APP_BUNDLE="$DIST_DIR/$APP_NAME.app"
mkdir -p "$APP_BUNDLE/Contents/MacOS"
mkdir -p "$APP_BUNDLE/Contents/Resources/Java"

# Copy JAR
cp server/bin/DonCEy-Kong-Jr-Server.jar "$APP_BUNDLE/Contents/Resources/Java/"

# Copy assets for server (sprites needed for admin panel)
echo "→ Copying assets..."
cp -r "$PROJECT_ROOT/server/assets" "$APP_BUNDLE/Contents/Resources/"

# Create launcher script
echo "→ Creating launcher script..."
cat > "$APP_BUNDLE/Contents/MacOS/DonCEy-Kong-Jr-Server" << 'EOF'
#!/bin/bash

# Get the directory of this script
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../Resources" && pwd)"

# Check if Java is installed
if ! command -v java &> /dev/null; then
    osascript -e 'display dialog "Java no está instalado.\n\nPor favor instala Java 11 o superior desde:\nhttps://adoptium.net" buttons {"OK"} default button 1 with icon stop'
    exit 1
fi

# Check Java version
JAVA_VERSION=$(java -version 2>&1 | awk -F '"' '/version/ {print $2}' | awk -F. '{print $1}')
if [ "$JAVA_VERSION" -lt 11 ]; then
    osascript -e 'display dialog "Java 11 o superior es requerido.\n\nVersión actual: '"$JAVA_VERSION"'\n\nPor favor actualiza Java desde:\nhttps://adoptium.net" buttons {"OK"} default button 1 with icon stop'
    exit 1
fi

# Launch the server with dock icon
cd "$DIR"
java -Xdock:name="DonCEy Kong Jr Server" \
     -Xdock:icon="$DIR/AppIcon.icns" \
     -jar "$DIR/Java/DonCEy-Kong-Jr-Server.jar"

# Keep terminal open on error
if [ $? -ne 0 ]; then
    echo ""
    echo "Presiona Enter para cerrar..."
    read
fi
EOF

chmod +x "$APP_BUNDLE/Contents/MacOS/DonCEy-Kong-Jr-Server"

# Check if icon exists
if [ -f "$PROJECT_ROOT/client/assets/icons/macos/server.icns" ]; then
    cp "$PROJECT_ROOT/client/assets/icons/macos/server.icns" "$APP_BUNDLE/Contents/Resources/AppIcon.icns"
else
    echo "  ⚠ Warning: server.icns not found, app will use default icon"
fi

# Create Info.plist
echo "→ Creating Info.plist..."
cat > "$APP_BUNDLE/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleName</key>
    <string>$APP_NAME</string>
    <key>CFBundleDisplayName</key>
    <string>$APP_NAME</string>
    <key>CFBundleIdentifier</key>
    <string>$BUNDLE_ID</string>
    <key>CFBundleVersion</key>
    <string>$VERSION</string>
    <key>CFBundleShortVersionString</key>
    <string>$VERSION</string>
    <key>CFBundleExecutable</key>
    <string>DonCEy-Kong-Jr-Server</string>
    <key>CFBundleIconFile</key>
    <string>AppIcon</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.13</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>LSApplicationCategoryType</key>
    <string>public.app-category.games</string>
    <key>LSUIElement</key>
    <false/>
</dict>
</plist>
EOF

# Sign the app (ad-hoc signature)
echo "→ Signing app..."
codesign --force --deep --sign - "$APP_BUNDLE" 2>/dev/null || echo "  ⚠ Code signing skipped"

# Create DMG (optional)
if command -v hdiutil &> /dev/null; then
    echo "→ Creating DMG..."
    DMG_NAME="DonCEy-Kong-Jr-Server-macOS.dmg"
    rm -f "$DIST_DIR/$DMG_NAME"
    hdiutil create -volname "$APP_NAME" -srcfolder "$APP_BUNDLE" -ov -format UDZO "$DIST_DIR/$DMG_NAME"
    echo "  ✓ DMG created: $DMG_NAME"
fi

echo ""
echo "✓ macOS Server build complete!"
echo "  Location: $APP_BUNDLE"
if [ -f "$DIST_DIR/DonCEy-Kong-Jr-Server-macOS.dmg" ]; then
    echo "  DMG: $DIST_DIR/DonCEy-Kong-Jr-Server-macOS.dmg"
fi
echo ""
echo "NOTA: Se requiere Java 11+ instalado en el sistema."
echo ""
