#!/bin/bash

VER="$1"

if [ "$VER" != "" ]; then

        echo ""
        echo "Creating osx version"
        echo ""

        mkdir -p "../rtcwcoop_"$VER"_osx/main/"

        cp -R Readme.txt Credits.txt knownbugs.txt rtcwcoop.app VERSION.txt "../rtcwcoop_"$VER"_osx"
        cp main/bin.pk3 main/sp_pak_coop1.pk3 main/cgamei386.dylib main/uii386.dylib main/qagamei386.dylib main/put_your_pk3_files_here.txt "../rtcwcoop_"$VER"_osx/main/"
        cd ..
        zip -r "rtcwcoop_"$VER"_osx.zip" "rtcwcoop_"$VER"_osx/"
        cd -
else
        echo ""
        echo "Usage: ./$0 <version>"
        echo ""
fi
