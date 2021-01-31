#!/bin/bash
#
# Install Dependencias
#   sudo apt-get install build-essential dh-make devscripts fakeroot
#
# Optional
#   sudo apt-get install pbuilder

 
# Basic vars

TMPDIR=`mktemp -d`
VERSION=`./get-version.sh`
PWD=`pwd`
NAME="picta-dl-gui"
CWD=$(pwd)
SRCDIR="$TMPDIR/$NAME-$VERSION"

# dhmake and pkg vars
MAINTAINER="Oleksis Fraga Menendez"
EMAIL="oleksis.fraga@gmail.com"
LIC="mit"
HOMEPAGE="https://github.com/oleksis/picta-dl-gui"
VCS="https://github.com/oleksis/picta-dl-gui.git"
DEPS="libc6, libnotify-bin, qt5-default (>=5.7.1)"
RECOM="ffmpeg (>=3.0.0)"
DATE=`date +"%a, %d %b 20%y %H:%M:%S %z"`

# var listing for dynamic substitution
VARS="VERSION NAME MAINTAINER EMAIL HOMEPAGE VCS DEPS RECOM DATE"

mkdir "$SRCDIR"
cp -r "$PWD"/* "$SRCDIR"/

cd "$SRCDIR"

# cleanup
rm -rdf debug/
rm -rdf release/
rm -f *.pro.user* object_script.* *_wrapper.sh
find $PWD \( -name "moc_*" -or -name "*.o" -or -name "qrc_*" -or -name "Makefile*" -or -name "*.a" \) -exec rm {} \;

rm -f *.deb

mv deployment/linux/debian .
rm -rdf deployment

# make it happen
dh_make  -c $LIC -e $EMAIL -s -p $NAME -y --createorig

# provision the debian folder
for v in `echo $VARS | xargs` ; do
    # get the var content
    CONTp=${!v}
    
    #escape possible / in the files
    CONT=`echo ${CONTp//\//\\\\/}`

    # note
    echo "replace $v by \"$CONT\""

    find "debian/" -type f -exec \
        sed -i s/"\_$v\_"/"${CONT}"/g {} \;
done

# make the debian archive
dpkg-buildpackage -us -uc 
#debuild -us -uc -b

cd ..
pwd
ls -la *.deb

cp picta-dl-gui*.deb $CWD 
rm -rdf "$TMPDIR"

cd $CWD 
pwd
ls -la *.deb
