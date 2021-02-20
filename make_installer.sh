#!/bin/bash
# Crear un Installador Offline con Qt Installer Framework using MinGW64 [Windows]
#
export DEST_DIR=./deployment/windows/packages/cu.pictadl.gui/data
export PACKAGE_DIR=./deployment/windows

echo "Cleaning ..."
rm -rf ./release/*
rm -rf ${DEST_DIR}/*

echo "Building Picta-dl_GUI ..."
qmake Picta-dl_GUI.pro -spec win32-g++
make -j2

# echo "Create output directory if it does not exist"
# mkdir -p "${DEST_DIR}/tools"

echo "Copy the final executables and dependencies to: ${DEST_DIR}"
echo "Copying dependencies ..."
cp -f "./release/Picta-dl_GUI.exe" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libwinpthread-1.dll" ${DEST_DIR}
cp -f $MINGW_PREFIX/bin/libgcc_*-1.dll ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libstdc++-6.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libharfbuzz-0.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libpng16-16.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/zlib1.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libgraphite2.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libfreetype-6.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libglib-2.0-0.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libbrotlidec.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libbz2-1.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libintl-8.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libpcre-1.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libdouble-conversion.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libicuin67.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libicuuc67.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libpcre2-16-0.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libzstd.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libicudt67.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libbrotlicommon.dll" ${DEST_DIR}
cp -f "$MINGW_PREFIX/bin/libiconv-2.dll" ${DEST_DIR}
echo "Copying executables ..."
cp -f "./Resources/picta-dl.exe" ${DEST_DIR}
cp -f "./Resources/ffmpeg.exe" ${DEST_DIR}

echo "Deploying executable ..."
windeployqt --dir=${DEST_DIR} ./release/Picta-dl_GUI.exe

echo "Creating Offline Installer Binary ..."
binarycreator -f -v -c ${PACKAGE_DIR}/config/config.xml -p ${PACKAGE_DIR}/packages PictaDownloaderGUI-Installer-Offline.exe

echo "Offline Installer created!"
