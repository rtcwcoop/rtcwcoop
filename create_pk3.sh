#!/bin/bash
PAKFILE=sp_pak_coop1.pk3
BINFILE=bin.pk3

echo "Creating directories"

if [ "`uname`" = "Darwin" ]; then
        mkdir -p ~/Documents/rtcwcoop/coopmain/
fi

if [ "`uname`" = "Linux" ]; then
        mkdir -p ~/.wolf/coopmain
fi

echo "Creating " $PAKFILE

cd media/sp_pak_coop1

zip -qur $PAKFILE ./ -x "*.svn*"

if [ "`uname`" = "Darwin" ]; then
        cp $PAKFILE ~/Documents/rtcwcoop/coopmain/
fi

if [ "`uname`" = "Linux" ]; then
        cp $PAKFILE ~/.wolf/coopmain
fi

mv $PAKFILE ../

echo "Creating " $BINFILE

if [ "`uname`" = "Darwin" ]; then
        cd "../../build"
	for i in $(find . -type f -name "*.coop.*.dylib" ! -name "*qagame*"); do zip -qurj $BINFILE $i; done
        cp $BINFILE ~/Documents/rtcwcoop/coopmain/
        mv $BINFILE ../media/
fi

if [ "`uname`" = "Linux" ]; then
        cd "../../build"
	for i in $(find . -type f -name "*.coop.*.so" ! -name "*qagame*"); do zip -qurj $BINFILE $i; done
        cp $BINFILE ~/.wolf/coopmain/
        mv $BINFILE ../media/
fi

echo "Done"
