#!/bin/bash
# @brief Make version number
# @author Wolfram Rösler <wolfram@roesler-ac.de>
# @date 2017-02-12

# Check command line
if [ $# = 2 -a "$1" = "-o" ];then

    # Called with -o: Create a .o file with the version number
    bash $0 | ${CMAKE_CXX_COMPILER:-c++} -x c++ -c - -o "$2"
    exit

elif [ $# != 0 ];then

    # Illegal command line
    echo "Generate version number"
    echo "Usage: $0 [ -o version.o ]"
    exit 1
fi

# Get the git repository version number
REPO=$(git describe --tags)

# Get the build time stamp
WHEN=$(date +"%Y-%m-%d %H:%M:%S")

# Get the user name
WHO="$USER"

# Get the machine name
WHERE=$(hostname)

# Get the OS version
WHAT=$(uname -sr)

# Put it all together
VERSION="built $WHEN by $WHO on $WHERE with $WHAT\n$REPO"

# Output the version number to the build log
echo "Building version $VERSION" >&2

# Create the version.cpp file
# NOTE: No includes to speed up compilation (remember that this file is
# re-created and compiled whenever a program is linked)
cat <<!
// This file was created by $0 on $(date)
// Do not edit - do not add to git
char const *Version() {
    return "@(#)$VERSION" + 4;
}
!