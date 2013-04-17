#!/bin/bash

VER="$1"

if [ "$VER" != "" ]; then

        echo ""
        echo "Creating Linux version"
        echo ""

        mkdir -p "../rtcwcoop_"$VER"_linux/main/"

        cp Readme.txt Credits.txt knownbugs.txt *.i386 VERSION "../rtcwcoop_"$VER"_linux"
        cp main/bin.pk3 main/sp_pak_coop1.pk3 main/cgamei386.so main/uii386.so main/qagamei386.so main/put_your_pk3_files_here.txt "../rtcwcoop_"$VER"_linux/main/"
        cd ..
        zip -r "rtcwcoop_"$VER"_linux.zip" "rtcwcoop_"$VER"_linux/"
        cd -
else
        echo ""
        echo "Usage: ./$0 <version>"
        echo ""
fi
