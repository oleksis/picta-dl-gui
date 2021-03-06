# Qt5 in MSYS2 (MINGW64)

## Install Qt5 
```bash
$ pacman -Sy base-devel git vim mingw-w64-x86_64-toolchain mingw-w64-x86_64-qt5-static mingw-w64-x86_64-clang
```

## Install Qt Installer Framework
```bash
pacman -S mingw-w64-x86_64-qt-installer-framework
```

### Create Installer
```pwsh
binarycreator.exe -f -v -c config/config.xml -p packages PictaDownloaderGUI-Installer.exe
```

## Install PyQt5 (MINGW64)
```bash
$ pacman -Sy mingw-w64-x86_64-python-pyqt5
```

# Install CMake (MINGW64)
$ pacman -Sy mingw-w64-x86_64-cmake

## Buil with CMake (MINGW64)
$ mkdir -p build
$ cd build
$ cmake -G "Unix Makefiles" ..
$ cmake --build . --target all

# openSUSE

## WSL
```bash 
$ wget https://download.opensuse.org/repositories/Virtualization:/WSL/openSUSE_Leap_15.2/openSUSE-Leap-15.2-x64-Build26.65.appx
```

## Disable Repository Official
```bash
$ sudo zypper lr
$ sudo zypper mr -d 1  # 5,6,9,10
$ sudo zypper mr -d 2
```

## Add Repository
```bash
$ sudo zypper addrepo https://repos.uclv.edu.cu/opensuse/15.2/repo/oss/ uclv_oss
$ sudo zypper addrepo https://repos.uclv.edu.cu/opensuse/15.2/repo/non-oss/ uclv_non-oss
```

## Update Repository
```bash
$ sudo zypper ref
```

## Install Qt5 in openSUSE
```bash
$ sudo zypper install --type pattern devel_basis  # Choose from above solutions by number or cancel
$ sudo zypper ps -s
$ sudo zypper --non-interactive install libqt5-qtbase-devel
```

## Check qmake in openSUSE
```bash
$ qmake-qt5 --version
QMake version 3.1
Using Qt version 5.12.6 in /usr/lib64
```

## Compile Project
```bash
$ qmake-qt5 -config release Picta-dl_GUI.pro -spec linux-g++-64
$ make clean
$ make -j2
```

## Add DISPLAY environment variable for VcXsrv
```bash
$ echo "" >> .bashrc && echo 'export DISPLAY="`grep nameserver /etc/resolv.conf | sed 's/nameserver //'`:0"' >> .bashrc
$ echo 'export XDG_RUNTIME_DIR=~/' >> .bashrc
$ echo 'export RUNLEVEL=3'  >> .bashrc
```

## Optional 

### Update GCC in openSUSE
```bash
$ sudo zypper --non-interactive up gcc
```

### Check gcc in openSUSE
```bash
$ gcc --version
```

### Update GNU Binutils
```bash
sudo zypper --non-interactive up binutils
```

### Install MC
```bash
sudo zypper in mc
```

### Install KDE for GUI Apps
```bash
$ sudo zypper install --type pattern kde
```

## Artistic Style 
```bash
astyle --project --recursive *.cpp,*.h
```

# Debian

## Clang
```bash
sudo apt-get install clang-8 clang-8-doc llvm-8-doc clang-format-8 clang-tidy-8 clang-tools-8
```

### Clang-Format
```bash
find . -regex '.*\.\(cpp\|hpp\|c\|h\)' -exec clang-format-8 -style=file -i {} \;
```

### Clang Update Alternatives

```bash
sudo update-alternatives \
  --install /usr/bin/clang                clang        /usr/bin/clang-8     10 \
  --slave   /usr/bin/clang++              clang++      /usr/bin/clang++-8 \
  --slave   /usr/bin/clang-format         clang-format /usr/bin/clang-format-8  \
  --slave   /usr/bin/clang-tidy           clang-tidy   /usr/bin/clang-tidy-8  \
  --slave   /usr/bin/clang-tidy-diff.py   clang-tidy-diff.py /usr/bin/clang-tidy-diff-8.py
```

### Check Clang Version

```bash
clang++ --version
```

## Install CMake

```bash
sudo apt-get install cmake cmake-doc
```

### Check CMake Version

```bash
cmake --version
```

### Build with CMake

```bash
mkdir -p build
cd build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
make
```

### Install with CMake

```bash
cd build
sudo cmake -DBUILD_TYPE=Release -P cmake_install.cmake
```

### Run Picta Downloader GUI in Dark Mode

```bash
picta-dlg -d
```

### Use Clang-Tidy (Modernize use nullptr)

```bash
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..

run-clang-tidy-8.py -header-filter='.*' -checks='-*,modernize-use-nullptr' -fix
```


## Links:
* https://gist.github.com/oleksis/459ec563b7d11a2a1856cec6c35d286e