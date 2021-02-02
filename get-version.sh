#!/bin/bash
# Print the human-readable version string in version.h
# oleksis <oleksis.fraga@gmail.com>

cd "`dirname $0`"
sed -n 's/#define VERSION_STRING "\(\([[:digit:]]\{2\}\.\?\)\{3\}\)"/\1/p' version.h