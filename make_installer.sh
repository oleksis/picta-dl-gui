#!/bin/bash
# Crear un Installador Offline con Qt Installer Framework [Windows]
#
export DEST_DIR=./deployment/windows/packages/cu.pictadl.gui/data
export PACKAGE_DIR=./deployment/windows

echo "Cleaning ..."
rm -rf ${DEST_DIR}/*

echo "Building Picta-dl_GUI ..."
qmake Picta-dl_GUI.pro -spec win32-g++
make -j2

# echo "Create output directory if it does not exist"
# mkdir -p "${DEST_DIR}/tools"

echo "Copying the final executables to: ${DEST_DIR}"
cp -f "./release/Picta-dl_GUI.exe" ${DEST_DIR}
cp -f "./Resources/picta-dl.exe" ${DEST_DIR}
cp -f "./Resources/ffmpeg.exe" ${DEST_DIR}

echo "Deploying executable ..."
windeployqt --dir=${DEST_DIR} ./release/Picta-dl_GUI.exe

echo "Creating Offline Installer Binary ..."
binarycreator -f -v -c ${PACKAGE_DIR}/config/config.xml -p ${PACKAGE_DIR}/packages PictaDownloaderGUI-Installer-Offline.exe

echo "Offline Installer created!"
