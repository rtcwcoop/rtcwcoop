#!/bin/bash

VER="$1"

if [ "$VER" != "" ]; then

        echo ""
        echo "Creating Windows version"
        echo ""

        mkdir -p "../rtcwcoop_"$VER"_windows/main/"

        cp Readme.txt msvcr100.dll SDL.dll Credits.txt knownbugs.txt *.exe start_dedicated_server.bat VERSION.txt "../rtcwcoop_"$VER"_windows"
        cp main/bin.pk3 main/sp_pak_coop1.pk3 main/cgamex86.dll main/uix86.dll main/qagamex86.dll main/put_your_pk3_files_here.txt "../rtcwcoop_"$VER"_windows/main/"
        cd ..
        zip -r "rtcwcoop_"$VER"_windows.zip" "rtcwcoop_"$VER"_windows/"
        cd -
else
        echo ""
        echo "Usage: ./$0 <version>"
        echo ""
fi
