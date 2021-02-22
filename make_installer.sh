#!/bin/bash
# Crear un Instalador Offline con Qt Installer Framework using MinGW64 [Windows]
#
export DEST_DIR=./deployment/windows/packages/cu.pictadl.gui/data
export PACKAGE_DIR=./deployment/windows

DEPS="libwinpthread-1.dll libstdc++-6.dll libharfbuzz-0.dll \
libpng16-16.dll zlib1.dll libgraphite2.dll libfreetype-6.dll \
libglib-2.0-0.dll libbrotlidec.dll libbz2-1.dll libintl-8.dll \
libpcre-1.dll libdouble-conversion.dll libicuin67.dll libicuuc67.dll \
libpcre2-16-0.dll libzstd.dll libicudt67.dll libbrotlicommon.dll libiconv-2.dll"

echo "Cleaning ..."
rm -rf ./release/*
[[ -d ${DEST_DIR} ]] && rm -rf ${DEST_DIR}/*

echo "Building Picta-dl_GUI ..."
qmake Picta-dl_GUI.pro -spec win32-g++
make -j2

# echo "Create output directory if it does not exist"
# mkdir -p "${DEST_DIR}/tools"

echo -e "\nCopy the final executables and dependencies to: ${DEST_DIR}"
echo "Copying dependencies ..."
cp -f $MINGW_PREFIX/bin/libgcc_*-1.dll ${DEST_DIR}
for v in $DEPS ; do
    cp -f "$MINGW_PREFIX/bin/$v" ${DEST_DIR}; 
done

echo -e "Copying executables ...\n"
cp -f "./release/Picta-dl_GUI.exe" ${DEST_DIR}
cp -f "./Resources/picta-dl.exe.orig" "${PACKAGE_DIR}/packages/cu.pictadl.cli/data/picta-dl.exe"
cp -f "./Resources/ffmpeg.exe.orig" "${PACKAGE_DIR}/packages/cu.pictadl.ffmpeg/data/ffmpeg.exe"

echo -e "Deploying executable ...\n"
windeployqt --dir=${DEST_DIR} ./release/Picta-dl_GUI.exe

echo -e "Creating Offline/Online Installer Binary ...\n"
binarycreator -v -c ${PACKAGE_DIR}/config/config.xml -p ${PACKAGE_DIR}/packages PictaDownloaderGUI-Installer.exe

echo -e "Offline/Online Installer created!\n"

export GIT_MERGE_AUTOEDIT=no
echo "Change to GH-Pages branch"
git checkout gh-pages
git merge master

echo "Cleaning for create Repository..."
rm -rfd deployment/repository/

echo -e "Creating Repository for GUI ...\n"
-repogen --update -v -p deployment/windows/packages -i cu.pictadl.gui deployment/repository

echo -e "Restore file\n"
git restore deployment/repository/.gitkeep

echo -e "Updating GH-Pages ...\n"
git config --global user.name "oleksis"
git config --global user.email "oleksis.fraga@gmail.com"
git add -A
git commit -am "Deploy repository to gh-pages"
git push -f origin HEAD:gh-pages

echo "Picta-dl_GUI deployed!"
