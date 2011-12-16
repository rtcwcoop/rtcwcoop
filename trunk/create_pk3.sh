#!/bin/bash
PAKFILE=sp_pak_coop1.pk3
cd media
zip -R $PAKFILE models/players/coop/* maps/* scripts/* ui/*

if [ "`uname`" = "Darwin" ]; then
        mv $PAKFILE ~/Library/Application\ Support/Wolfenstein/main/
fi
if [ "`uname`" = "Linux" ]; then
        mv $PAKFILE ~/.wolf/main
fi
