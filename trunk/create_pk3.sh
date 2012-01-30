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
