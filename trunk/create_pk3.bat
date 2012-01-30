SET PATH=%CD%;%PATH%
echo %PATH%
cd media\sp_pak_coop1\
zip.exe -r sp_pak_coop1.pk3 models\players\multi\ models\movespeeds\ maps\ scripts\ ui\ . levelshots
move sp_pak_coop1.pk3 ..\..\
pause