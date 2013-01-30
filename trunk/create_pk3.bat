SET PATH=%CD%;%PATH%
cd media\sp_pak_coop1\
zip.exe -r sp_pak_coop1.pk3 .\
move sp_pak_coop2.pk1 ..\..\main
cd ..\..\
copy sp_pak_coop2.pk1 "d:\STEAM\steamapps\common\return to castle wolfenstein\Main\"
