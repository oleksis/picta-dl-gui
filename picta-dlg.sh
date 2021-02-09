#!/bin/bash
appname=`basename $0 | sed s,\.sh$,,`

dirname=`dirname $0`
tmp="${dirname#?}"

if [ "${dirname%$tmp}" != "/" ]; then
dirname=$PWD/$dirname
fi
## Distributing Qt libraries ##
# LD_LIBRARY_PATH=$dirname
# export LD_LIBRARY_PATH
$dirname/$appname "$@"
