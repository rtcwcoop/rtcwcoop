#!/bin/sh

if [ "$1" = "release" ]; then
        TARGET="release"
        APPBUNDLE=rtcwcoop.app
else
        TARGET="debug"
        APPBUNDLE=rtcwcoop_debug.app
fi

BINARY=wolfsp.i386
DEDBIN=wolfspded.i386
PKGINFO=APPLWOLFSP
ICNS=misc/wolfsp.icns
DESTDIR=build/$TARGET-darwin-app
BASEDIR=main

BIN_OBJ="
	build/$TARGET-darwin-i386/wolfsp.i386
"
BIN_DEDOBJ="
	build/$TARGET-darwin-i386/wolfspded.i386
"

cd `dirname $0`
if [ ! -f Makefile ]; then
	echo "This script must be run from the wolfsp build directory"
	exit 1
fi

Q3_VERSION=`grep '^VERSION=' Makefile | sed -e 's/.*=\(.*\)/\1/'`

# We only care if we're >= 10.4, not if we're specifically Tiger.
# "8" is the Darwin major kernel version.
TIGERHOST=`uname -r |perl -w -p -e 's/\A(\d+)\..*\Z/$1/; $_ = (($_ >= 8) ? "1" : "0");'`

# we want to use the oldest available SDK for max compatiblity
unset X86_SDK
unset X86_CFLAGS
unset X86_LDFLAGS
if [ -d /Developer/SDKs/MacOSX10.6.sdk ]; then
        X86_SDK=/Developer/SDKs/MacOSX10.6.sdk
        X86_CFLAGS="-arch i386 -isysroot /Developer/SDKs/MacOSX10.6.sdk \
                        -DMAC_OS_X_VERSION_MIN_REQUIRED=1060"
        X86_LDFLAGS=" -mmacosx-version-min=10.6"
fi
if [ -d /Developer/SDKs/MacOSX10.5.sdk ]; then
	X86_SDK=/Developer/SDKs/MacOSX10.5.sdk
	X86_CFLAGS="-arch i386 -isysroot /Developer/SDKs/MacOSX10.5.sdk \
			-DMAC_OS_X_VERSION_MIN_REQUIRED=1050"
	X86_LDFLAGS=" -mmacosx-version-min=10.5"
fi

if [ -d /Developer/SDKs/MacOSX10.4u.sdk ]; then
	X86_SDK=/Developer/SDKs/MacOSX10.4u.sdk
	X86_CFLAGS="-arch i386 -isysroot /Developer/SDKs/MacOSX10.4u.sdk \
			-DMAC_OS_X_VERSION_MIN_REQUIRED=1040"
	X86_LDFLAGS=" -mmacosx-version-min=10.4"
fi

echo "Building X86 Client/Dedicated Server against \"$X86_SDK\""
if [ "$X86_SDK" != "/Developer/SDKs/MacOSX10.4u.sdk" ]; then
	echo "\
WARNING: in order to build a binary with maximum compatibility you must
         build on Mac OS X 10.4 using Xcode 2.3 or 2.5 and have the
         MacOSX10.4u SDK installed from the Xcode 
         install disk Packages folder."
fi
#sleep 3

if [ ! -d $DESTDIR ]; then
	mkdir -p $DESTDIR
fi

# For parallel make on multicore boxes...
NCPU=`sysctl -n hw.ncpu`


if [ "$1" = "release" ]; then
        # intel client and server
        if [ -d build/release-darwin-i386 ]; then
                rm -r build/release-darwin-i386
        fi
        (ARCH=i386 CFLAGS=$X86_CFLAGS LDFLAGS=$X86_LDFLAGS make -j$NCPU release) || exit 1;
else
        (ARCH=i386 CFLAGS=$X86_CFLAGS LDFLAGS=$X86_LDFLAGS make -j$NCPU) || exit 1;
fi

echo "Creating .app bundle $DESTDIR/$APPBUNDLE"
if [ ! -d $DESTDIR/$APPBUNDLE/Contents/MacOS/$BASEDIR ]; then
	mkdir -p $DESTDIR/$APPBUNDLE/Contents/MacOS/$BASEDIR || exit 1;
fi
if [ ! -d $DESTDIR/$APPBUNDLE/Contents/Resources ]; then
	mkdir -p $DESTDIR/$APPBUNDLE/Contents/Resources
fi
cp $ICNS $DESTDIR/$APPBUNDLE/Contents/Resources/ || exit 1;
echo $PKGINFO > $DESTDIR/$APPBUNDLE/Contents/PkgInfo
echo "
	<?xml version=\"1.0\" encoding=\"UTF-8\"?>
	<!DOCTYPE plist
		PUBLIC \"-//Apple Computer//DTD PLIST 1.0//EN\"
		\"http://www.apple.com/DTDs/PropertyList-1.0.dtd\">
	<plist version=\"1.0\">
	<dict>
		<key>CFBundleDevelopmentRegion</key>
		<string>English</string>
		<key>CFBundleExecutable</key>
		<string>$BINARY</string>
		<key>CFBundleGetInfoString</key>
		<string>wolfsp $Q3_VERSION</string>
		<key>CFBundleIconFile</key>
		<string>wolfsp.icns</string>
		<key>CFBundleIdentifier</key>
		<string>org.wolf.wolfsp</string>
		<key>CFBundleInfoDictionaryVersion</key>
		<string>6.0</string>
		<key>CFBundleName</key>
		<string>wolfsp</string>
		<key>CFBundlePackageType</key>
		<string>APPL</string>
		<key>CFBundleShortVersionString</key>
		<string>$Q3_VERSION</string>
		<key>CFBundleSignature</key>
		<string>$PKGINFO</string>
		<key>CFBundleVersion</key>
		<string>$Q3_VERSION</string>
		<key>NSExtensions</key>
		<dict/>
		<key>NSPrincipalClass</key>
		<string>NSApplication</string>
	</dict>
	</plist>
	" > $DESTDIR/$APPBUNDLE/Contents/Info.plist

lipo -create -o $DESTDIR/$APPBUNDLE/Contents/MacOS/$BINARY $BIN_OBJ
lipo -create -o $DESTDIR/$APPBUNDLE/Contents/MacOS/$DEDBIN $BIN_DEDOBJ
cp src/libs/macosx/libSDL-1.2.0.dylib $DESTDIR/$APPBUNDLE/Contents/MacOS/
cp src/libs/macosx/libSDL-1.2.0.dylib build/$TARGET-darwin-i386/
cp src/libs/macosx/libcurl.dylib build/$TARGET-darwin-i386/


if [ "$1" = "pk3" ] || [ "$2" = "pk3" ]; then
        # create pk3 file if needed
        #OUTPUT=`svn diff media/sp_pak_coop1/`

        #if [ "$OUTPUT" != "" ]; then
                sh create_pk3.sh
        #fi
fi
