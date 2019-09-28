#!/bin/bash
if [ -z "$1" ]; then
	echo "The first parameter must be the path to a build folder."
	exit 1
fi
if [[ $1 == *"MinGW_32"* ]];then
	mingw_folder="mingw53_32"
	bit="32"
elif [[ $1 == *"MinGW_64"* ]];then
	mingw_folder="mingw73_64"
	bit="64"
else
	echo "Only MinGW builds are supported."
	exit 2
fi

if [[ $1 == *"Debug"* ]];then
	build_type="debug"
else
	build_type="release"
fi

version_with_underscore=$( echo "$1" | cut -d'_' -f3,4,5 )
qt_version="${version_with_underscore//_/.}"

#copy Lichtsteuerung to new folder
target_folder="$1/../deploy-$qt_version-$build_type"
rm -f -r "$target_folder"
mkdir -p "$target_folder"
settings_file="$target_folder/settings.ini"
rm -f "$settings_file"
cp "$1/$build_type/Lichtsteuerung.exe" "$target_folder/Lichtsteuerung.exe"

#run windeployqt.exe
qt_base_dir="C:/Qt/"
exec_base_dir="$qt_base_dir/$qt_version/$mingw_folder"
echo "Running windeployqt"
result=$(eval "'$exec_base_dir/bin/windeployqt.exe' --qmldir 'src' '$target_folder/Lichtsteuerung.exe'")
echo "Finish running windeployqt"

#copy boost libs

if [[ $bit == "32" ]]; then
	echo "32-Bit boost build are no longer supported!"
else
	if [[ $biuld_type == "debug" ]]; then
		debug_infix="d-"
	else
		debug_infix=''
	fi
	cp "src/lib/boost/lib/libboost_coroutine-mt-${debug_infix}x64.dll" "$target_folder/"
	cp "src/lib/boost/lib/libboost_context-mt-${debug_infix}x64.dll" "$target_folder/"
fi

# copy aubio
cp "src/lib/aubio/lib/libaubio-5.dll" "$target_folder"

#copy AudioFFT
cp "src/lib/AudioFFT/dll/win${bit}/AudioFFT.dll" "$target_folder"
cp "src/lib/AudioFFT/dll/win${bit}/libfftw3-3.dll" "$target_folder"
cp "src/lib/AudioFFT/dll/win${bit}/libfftw3f-3.dll" "$target_folder"
cp "src/lib/AudioFFT/dll/win${bit}/libfftw3l-3.dll" "$target_folder"

#copy DrMinGw
if [[ $bit == "64" ]]; then
    for file in src/lib/DrMinGW/bin/*; do cp "$file" "$target_folder";done
fi

#copy Windows Capture Audio Output and add path to settings.ini
if [[ $biuld_type == "debug" ]]; then
	cp "src/lib/WindowsSound/dll/Capture_Windows_SoundOutputd-x$bit.dll" "$target_folder"
	echo "audioCaptureFilePath=Capture_Windows_SoundOutputd-x$bit.dll" >> "$settings_file"
else
	cp "src/lib/WindowsSound/dll/Capture_Windows_SoundOutput-x$bit.dll" "$target_folder"
	echo "audioCaptureFilePath=Capture_Windows_SoundOutput-x$bit.dll" >> "$settings_file"
fi

#copy QTJSONFile
cp "$1/QTJSONFile.json" "$target_folder"

#copy header files for modules and update settings.ini
mkdir -p "$target_folder/modules"
cp -r "src/modules" "$target_folder/modules/include"
echo "includePath=modules/include" >> "$settings_file"
echo "moduleDirPath=modules" >> "$settings_file"

#copy some files that are only needed by the minge53 32 Bit version, because windeployqt make a bad job
if [[ $mingw_folder == "mingw53_32" ]]; then
	cp "$exec_base_dir/bin/libgcc_s_dw2-1.dll" "$target_folder"
	cp "$exec_base_dir/bin/libstdc++-6.dll" "$target_folder"
	cp "$exec_base_dir/bin/libwinpthread-1.dll" "$target_folder"
fi
