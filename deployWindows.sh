#!/bin/bash

# Ensure the script exits if any command fails
set -e

# Check if the required parameters are provided
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <build_folder> <destination_folder>"
    exit 1
fi

# Assign parameters to variables
BUILD_FOLDER=$1
DESTINATION_FOLDER=$2
SCRIPT_DIR=$(dirname "$0")  # Get the directory of the current script
QML_DIR="$SCRIPT_DIR/src/qml"  # Set the QML directory
MODULES_SRC_DIR="$SCRIPT_DIR/src/modules"  # Source modules folder
MODULES_DEST_DIR="$DESTINATION_FOLDER/modules"  # Destination modules folder

# Ensure build folder exists
if [ ! -d "$BUILD_FOLDER" ]; then
    echo "Error: Build folder does not exist: $BUILD_FOLDER"
    exit 1
fi

# Clean and create the destination/output folder
echo "Cleaning and creating destination folder: $DESTINATION_FOLDER"
rm -rf "$DESTINATION_FOLDER"
mkdir -p "$DESTINATION_FOLDER"

# Copy all .exe and .dll files to the destination folder
echo "Copying .exe and .dll files to $DESTINATION_FOLDER"
find "$BUILD_FOLDER/src" -type f \( -name "*.exe" -o -name "*.dll" \) -exec cp {} "$DESTINATION_FOLDER" \;

# Copy src/modules to dest/modules
if [ -d "$MODULES_SRC_DIR" ]; then
    echo "Copying $MODULES_SRC_DIR to $MODULES_DEST_DIR"
    mkdir -p "$MODULES_DEST_DIR"
    cp -r "$MODULES_SRC_DIR"/* "$MODULES_DEST_DIR"
else
    echo "Warning: Source modules folder $MODULES_SRC_DIR does not exist. Skipping copy."
fi

# Create settings.ini in the destination folder
SETTINGS_FILE="$DESTINATION_FOLDER/settings.ini"
echo "Creating settings.ini in $DESTINATION_FOLDER"
cat <<EOL > "$SETTINGS_FILE"
includePath=modules/include
moduleDirPath=modules
EOL

# Run windeployqt with the QML directory
echo "Running windeployqt with QML directory: $QML_DIR"
windeployqt --qmldir "$QML_DIR" "$DESTINATION_FOLDER"

echo "Deployment completed successfully."
