#!/bin/bash

VERSION=$1

if [ "$VERSION" != "" ]; then
        echo "Creating RTCW COOP $VERSION"
        echo ""

        sh create_binpk3.sh
        sh create_windows.sh $VERSION
        sh create_linux.sh $VERSION
        sh create_osx.sh $VERSION

        echo ""
        echo "Done"

else
        echo "Usage ./$0 <version>"
fi
