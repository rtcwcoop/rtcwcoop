#!/bin/bash
PAKFILE=sp_pak_coop1.pk3
cd media/sp_pak_coop1

#zip -r $PAKFILE models/players/coop/* models/movespeeds/* maps/* scripts/* ui/* ./* levelshots/*
zip -r $PAKFILE ./ -x "*.svn*"

if [ "`uname`" = "Darwin" ]; then
        cp $PAKFILE ~/Library/Application\ Support/Wolfenstein/main/
fi
if [ "`uname`" = "Linux" ]; then
        cp $PAKFILE ~/.wolf/main
fi

mv $PAKFILE ../


if [ "`uname`" = "Darwin" ]; then
        # currently, only debug versions
        cd ../../build/debug-darwin-i386/main/
        zip -r bin.pk3 *.dylib
        cp bin.pk3 ~/Library/Application\ Support/Wolfenstein/main/
        mv bin.pk3 ../../../media/sp_pak_coop1 
fi
