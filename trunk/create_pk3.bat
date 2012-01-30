SET PATH=%CD%;%PATH%
cd media\sp_pak_coop1\
zip.exe -r sp_pak_coop1.pk3 .\ "*.svn*"
move sp_pak_coop1.pk3 ..\..\
cd ..\..\
copy sp_pak_coop1.pk3 "d:\STEAM\steamapps\common\return to castle wolfenstein\Main\"
