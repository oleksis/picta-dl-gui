#!/bin/bash
# Crear un paquete para openSUSE/SUSE a partir de los funtes
#
# Install Dependencias
#   sudo zypper in rpm-build
#   sudo zypper in git
#


# No Errors
set -e

VERSION_FILE="./get-version.sh"

chmod +x "$VERSION_FILE"

# Basic vars
TMPDIR=$(mktemp -d)
VERSION=$($VERSION_FILE)
RELEASE=1
# Get tag from current branch
VERSION_TAG=$(git describe --abbrev=0 --tags)
PWD=$(pwd)
CWD="$PWD"
NAME="picta-dl-gui"
WORKSPACE=$TMPDIR
export WORKSPACE
echo "Creating Workspace for rpmbuild ..."
mkdir -p $WORKSPACE/{BUILD,RPMS,SOURCES,SPECS,SRPMS,tmp}
echo "Working in $WORKSPACE"

SRCDIR="$TMPDIR/$NAME-$VERSION"

# SPEC vars
MAINTAINER="Oleksis Fraga"
MAINT=$MAINTAINER
EMAIL="oleksis.fraga@gmail.com"
LIC="MIT"
HOMEPAGE="https://github.com/oleksis/picta-dl-gui"
DATE=$(date +"%a %b %d 20%y")

# VARS listing for dynamic substitution
VARS="VERSION RELEASE VERSION_TAG NAME MAINTAINER EMAIL LIC HOMEPAGE DATE"

echo "Cleaning ..."
rm -f *qmake.stash *.pro.user* object_script.* *_wrapper.sh
find $PWD \( -name "moc_*" -or -name "*.o" -or -name "qrc_*" \
    -or -name "ui_*.h" -or -name "Makefile*" -or -name "*.a" \) -exec rm {} \;
rm -f *.rpm

echo "Create source archive in SOURCES ..."
git archive --format=tar HEAD | gzip > $WORKSPACE/SOURCES/$VERSION_TAG.tar.gz

echo "Copy $NAME.spec to $WORKSPACE"
cp -f "./deployment/linux/openSUSE/$NAME.spec" $WORKSPACE/SPECS

echo "Change to $WORKSPACE"
cd "$WORKSPACE"

# Provision SPEC
for v in $(echo $VARS | xargs) ; do
    # Get the var content: Indirect Expansion
    CONT=${!v}
    # Note
    echo "Replace $v by \"$CONT\""

    find "SPECS/" -type f -exec \
        sed -i "s|\_\_$v\_\_|${CONT}|g" {} \;
done

# Make the openSUSE archive
echo "Building package ..."
rpmbuild --define "_topdir ${WORKSPACE}" -bb -vv "$WORKSPACE/SPECS/$NAME.spec"


ls -la ./RPMS/x86_64/*.rpm
echo "Copying RPM package ..."
cp ./RPMS/x86_64/${NAME}*.rpm $CWD 
echo "Deleting $TMPDIR ..."
rm -rdf "$WORKSPACE"
echo "Change to $CWD"
cd $CWD 
ls -la *.rpm
echo "Finished build the RPM package!"
