@echo off

if not exist "%UserProfile%\.OpenRaider" (
	echo Setting up for %UserName%...
	mkdir "%UserProfile%\.OpenRaider"
	copy data\OpenRaider.ini %UserProfile%\.OpenRaider
	mkdir %UserProfile%\.OpenRaider\paks
	mkdir %UserProfile%\.OpenRaider\music
	mkdir %UserProfile%\.OpenRaider\data
	mkdir %UserProfile%\.OpenRaider\sshots
	copy data\*.tga %UserProfile%\.OpenRaider\data
	copy data\*.ttf %UserProfile%\.OpenRaider\data
	copy data\*.pc %UserProfile%\.OpenRaider\data
	echo Done!
) else (
	echo Already done
)
