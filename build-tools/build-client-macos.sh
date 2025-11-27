#!/bin/bash
set -e

echo "================================================"
echo " Building DonCEy Kong Jr Client for macOS"
echo "================================================"

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
CLIENT_DIR="$PROJECT_ROOT/client"
DIST_DIR="$PROJECT_ROOT/dist/macos"
APP_NAME="DonCEy Kong Jr"
BUNDLE_ID="com.doncey.kongjr.client"
VERSION="1.0.0"

# Clean previous builds
echo "→ Cleaning previous builds..."
rm -rf "$DIST_DIR/$APP_NAME.app"
mkdir -p "$DIST_DIR"

# Build client
echo "→ Building client binary..."
cd "$CLIENT_DIR"
make clean
make

# Create .app bundle structure
echo "→ Creating .app bundle..."
APP_BUNDLE="$DIST_DIR/$APP_NAME.app"
mkdir -p "$APP_BUNDLE/Contents/MacOS"
mkdir -p "$APP_BUNDLE/Contents/Resources"

# Create launcher script that changes to Resources directory
cat > "$APP_BUNDLE/Contents/MacOS/DonCEy-Kong-Jr" << 'EOF'
#!/bin/bash
# Get the directory of this script
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/../Resources" && pwd)"
# Change to Resources directory so assets are found
cd "$DIR"
# Execute the actual binary
exec "$DIR/../MacOS/DonCEy-Kong-Jr-bin" "$@"
EOF
chmod +x "$APP_BUNDLE/Contents/MacOS/DonCEy-Kong-Jr"

# Copy actual binary with different name
cp "$CLIENT_DIR/build/client" "$APP_BUNDLE/Contents/MacOS/DonCEy-Kong-Jr-bin"
chmod +x "$APP_BUNDLE/Contents/MacOS/DonCEy-Kong-Jr-bin"

# Copy assets
echo "→ Copying assets..."
cp -r "$CLIENT_DIR/assets" "$APP_BUNDLE/Contents/Resources/"

# Check if icon exists, if not use logo as fallback
if [ -f "$CLIENT_DIR/assets/icons/macos/client.icns" ]; then
    cp "$CLIENT_DIR/assets/icons/macos/client.icns" "$APP_BUNDLE/Contents/Resources/AppIcon.icns"
else
    echo "  ⚠ Warning: client.icns not found, app will use default icon"
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
    <string>DonCEy-Kong-Jr</string>
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
</dict>
</plist>
EOF

# Sign the app (ad-hoc signature for local use)
echo "→ Signing app..."
codesign --force --deep --sign - "$APP_BUNDLE" 2>/dev/null || echo "  ⚠ Code signing skipped (not critical)"

# Create DMG (optional)
if command -v hdiutil &> /dev/null; then
    echo "→ Creating DMG..."
    DMG_NAME="DonCEy-Kong-Jr-Client-macOS.dmg"
    rm -f "$DIST_DIR/$DMG_NAME"
    hdiutil create -volname "$APP_NAME" -srcfolder "$APP_BUNDLE" -ov -format UDZO "$DIST_DIR/$DMG_NAME"
    echo "  ✓ DMG created: $DMG_NAME"
fi

echo ""
echo "✓ macOS Client build complete!"
echo "  Location: $APP_BUNDLE"
if [ -f "$DIST_DIR/DonCEy-Kong-Jr-Client-macOS.dmg" ]; then
    echo "  DMG: $DIST_DIR/DonCEy-Kong-Jr-Client-macOS.dmg"
fi
echo ""
