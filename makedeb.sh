#!/bin/bash
# Crear un paquete para Debian/Ubuntu a partir de los funtes
#
# Install Dependencias
#   sudo apt-get install build-essential dh-make devscripts fakeroot pbuilder


# No Errors
set -e

VERSION_FILE="./get-version.sh"

chmod +x "$VERSION_FILE"

# Basic vars
TMPDIR=$(mktemp -d)
VERSION=$($VERSION_FILE)
PWD=$(pwd)
CWD="$PWD"
NAME="picta-dl-gui"
SRCDIR="$TMPDIR/$NAME-$VERSION"

# dhmake and pkg vars
MAINTAINER="Oleksis Fraga"
MAINT=$MAINTAINER
EMAIL="oleksis.fraga@gmail.com"
LIC="mit"
HOMEPAGE="https://github.com/oleksis/picta-dl-gui"
VCS="https://github.com/oleksis/picta-dl-gui.git"
DEPS="qtbase5-dev"
RECOM="ffmpeg (>=3.0.0)"
DATE=$(date +"%a, %d %b 20%y %H:%M:%S %z")

# var listing for dynamic substitution
VARS="VERSION NAME MAINTAINER EMAIL HOMEPAGE VCS DEPS RECOM DATE"

# cleanup
echo "Cleaning ..."
rm -f *qmake.stash *.pro.user* object_script.* *_wrapper.sh
find $PWD \( -name "moc_*" -or -name "*.o" -or -name "qrc_*" \
    -or -name "ui_*.h" -or -name "Makefile*" -or -name "*.a" \) -exec rm {} \;
rm -f *.deb


mkdir "$SRCDIR"
echo "Copying source code to $SRCDIR ..."
cp -r "$CWD"/* "$SRCDIR"/
echo "Change to $SRCDIR"
cd "$SRCDIR"

echo "Copy debian folder"
mv deployment/linux/debian .
rm -rdf deployment
rm -rdf debug/
rm -rdf release/

# With Erros
set +e
# make it happen
echo "Running dh_make ..."
dh_make  -c $LIC -e $EMAIL -s -p $NAME -y --createorig

# provision the debian folder
for v in $(echo $VARS | xargs) ; do
    # Get the var content: Indirect Expansion
    CONT=${!v}
    # Note
    echo "Replace $v by \"$CONT\""

    find "debian/" -type f -exec \
        sed -i "s|\_\_$v\_\_|${CONT}|g" {} \;
done

# No Errors
set -e

# make the debian archive
echo "Building package ..."
dpkg-buildpackage -us -uc 
#debuild -us -uc -b

cd ..
echo "Change to: $(pwd)"
ls -la *.deb
echo "Copying Debian package ..."
cp picta-dl-gui*.deb $CWD 
echo "Deleting $TMPDIR"
rm -rdf "$TMPDIR"
echo "Change to: $CWD"
cd $CWD 
ls -la *.deb
echo "Finished build the Debian package!"
