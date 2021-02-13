#
# Spec file for package picta-dl-gui
#
# Copyright © 2020 Picta Downloader GUI Team <oleksis.fraga@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
# IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
# CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
# SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#

%define VERSION_SRC 0.12.17

Name:           picta-dl-gui
Version:        2021.02.13
Release:        1
Summary:        Picta Downloader GUI permite descargar contenidos audiovisuales desde Picta.
License:        MIT
Group:          Productivity/Networking/Utilities
Source:         https://github.com/oleksis/picta-dl-gui/archive/v%{VERSION_SRC}.tar.gz
URL:            https://github.com/oleksis/picta-dl-gui
Packager:       Oleksis Fraga <oleksis.fraga@gmail.com>

BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: libqt5-qtbase-devel
BuildRequires: pkgconfig(Qt5Core)
BuildRequires: pkgconfig(Qt5Gui)
BuildRequires: pkgconfig(Qt5Widgets)
Recommends:    ffmpeg

%description
Picta Downloader GUI es una aplicación que le permite descargar contenidos audiovisuales desde Picta.

%prep
%setup -c v%{VERSION_SRC}

%build
qmake-qt5 -config release Picta-dl_GUI.pro -spec linux-g++-64
make -j2

%install
INSTALL_ROOT=%{buildroot} make install

%clean
rm -rf %{buildroot}

%files
%defattr(0755,root,root)
%{_bindir}/Picta-dl_GUI
%defattr(0755,root,root)

%defattr(0644,root,root)
%{_datadir}/applications/picta-dl-gui.desktop
%{_datadir}/icons/hicolor/scalable/apps/picta-dl-gui.svg
%defattr(0644,root,root)

%changelog
* Fri Feb 12 2021 Oleksis Fraga <oleksis.fraga@gmail.com> - 0.12.17
- Initial release