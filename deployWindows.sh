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
target_folder="$1/deploy-$build_type"
mkdir -p "$target_folder"
cp "$1/$build_type/Lichtsteuerung.exe" "$target_folder/Lichtsteuerung.exe"

#run windeployqt.exe
qt_base_dir="C:/Qt/"
exec_base_dir="$qt_base_dir/$qt_version/$mingw_folder"
echo "Running windeployqt"
result=$(eval "'$exec_base_dir/bin/windeployqt.exe' --qmldir '$exec_base_dir/qml' '$1/deploy-$build_type/Lichtsteuerung.exe'")
echo "Finish running windeployqt"

#copy qml shapes
cp -r "$exec_base_dir/qml/QtQuick/Shapes" "$target_folder/QtQuick/"

#copy boost libs

if [[ $bit == "32" ]]; then
	if [[ $biuld_type == "debug" ]]; then
		boost_context_lib="libboost_contextd.dll"
		boost_coroutine_lib="libboost_coroutined.dll"
	else
		boost_context_lib="libboost_context.dll"
		boost_coroutine_lib="libboost_coroutine.dll"
	fi
else
	if [[ $biuld_type == "debug" ]]; then
		debug_infix="d-"
	else
		debug_infix=''
	fi
	boost_context_lib="mingw73/libboost_context-mgw73-mt-${debug_infix}x64-1_69.dll"
	boost_coroutine_lib="mingw73/libboost_coroutine-mgw73-mt-${debug_infix}x64-1_69.dll"

fi
cp "src/lib/boost/$boost_context_lib" "$target_folder/"
cp "src/lib/boost/$boost_coroutine_lib" "$target_folder/"

#copy AudioFFT
cp "src/lib/AudioFFT/dll/win${bit}/AudioFFT.dll" "$target_folder"
cp "src/lib/AudioFFT/dll/win${bit}/libfftw3-3.dll" "$target_folder"
cp "src/lib/AudioFFT/dll/win${bit}/libfftw3f-3.dll" "$target_folder"
cp "src/lib/AudioFFT/dll/win${bit}/libfftw3l-3.dll" "$target_folder"

#copy Windows Capture Audio Output
if [[ $biuld_type == "debug" ]]; then
	cp "src/lib/WindowsSound/dll/Capture_Windows_SoundOutputd-x$bit.dll" "$target_folder"
else
	cp "src/lib/WindowsSound/dll/Capture_Windows_SoundOutput-x$bit.dll" "$target_folder"
fi

#copy QTJSONFile
cp "$1/QTJSONFile.json" "$target_folder"

#copy header files for modules
mkdir -p "$target_folder/modules"
cp -r "src/programms" "$target_folder/modules"

#copy some files that are only needed by the minge53 32 Bit version, because windeployqt make a bad job
if [[ $mingw_folder == "mingw53_32" ]]; then
	cp "$exec_base_dir/bin/libgcc_s_dw2-1.dll" "$target_folder"
	cp "$exec_base_dir/bin/libstdc++-6.dll" "$target_folder"
	cp "$exec_base_dir/bin/libwinpthread-1.dll" "$target_folder"
fi
