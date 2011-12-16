#!/bin/bash
PAKFILE=sp_pak_coop1.pk3
cd media
zip -R $PAKFILE models/players/coop/* maps/* scripts/* ui/*

if [ "`uname`" = "Darwin" ]; then
        cp $PAKFILE ~/Library/Application\ Support/Wolfenstein/main/
fi
if [ "`uname`" = "Linux" ]; then
        cp $PAKFILE ~/.wolf/main
fi
