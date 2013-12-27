#!/bin/sh

# Argument 1: Project Name
# Argument 2: Short Version (0.1.1)
# Argument 3: Long Version (0.1.1-20131214)

YEAR=`date +%Y`

echo '<?xml version="1.0" encoding="UTF-8"?>'
echo '<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">'
echo '<plist version="1.0">'
echo '<dict>'
echo '        <key>CFBundleDevelopmentRegion</key>'
echo '        <string>English</string>'
echo '        <key>CFBundleExecutable</key>'
echo "        <string>$1</string>"
echo '        <key>CFBundleGetInfoString</key>'
echo "        <string>$1 Version $2-$3</string>"
echo '        <key>CFBundleIconFile</key>'
echo '        <string>openraider.icns</string>'
echo '        <key>CFBundleIdentifier</key>'
echo '        <string>de.xythobuz.openraider</string>'
echo '        <key>CFBundleInfoDictionaryVersion</key>'
echo '        <string>6.0</string>'
echo '        <key>CFBundleLongVersionString</key>'
echo "        <string>$2-$3</string>"
echo '        <key>CFBundleName</key>'
echo "        <string>$1</string>"
echo '        <key>CFBundlePackageType</key>'
echo '        <string>APPL</string>'
echo '        <key>CFBundleShortVersionString</key>'
echo "        <string>$2-$3</string>"
echo '        <key>CFBundleSignature</key>'
echo '        <string>????</string>'
echo '        <key>CFBundleVersion</key>'
echo "        <string>$2-$3</string>"
echo '        <key>CSResourcesFileMapped</key>'
echo '        <true/>'
echo '        <key>NSHumanReadableCopyright</key>'
echo "        <string>Copyright 2001 - ${YEAR}</string>"
echo '</dict>'
echo '</plist>'
