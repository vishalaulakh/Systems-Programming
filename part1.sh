#!/bin/bash

# Assignment Submission Too to create a Zipped File

# Check if a directory name is provided as a command line argument
# Note: if  Entering a Directory name on Command Line, Enter lab1, lab2, lab3, etc as command
if [ -z "$1" ]; then
    echo "Usage: $0 <directory_name>"
    exit 1
fi


# Constructing the ZIP file name using variables
ZIP_FILENAME="$USER"_"$1".zip

# Variable for Full path to the target sub-directory
TARGET_DIRECTORY="$HOME/3380/$1/"

# Checking if the specified directory exists
if [ ! -d "$TARGET_DIRECTORY" ]; then
    echo "Error: Directory '$TARGET_DIRECTORY' not found."
    exit 1
fi

# Creating the ZIP file in my  HOME directory
zip -r "$ZIP_FILENAME" $TARGET_DIRECTORY

# Checking if the ZIP command was successful
if [ $? -eq 0 ]; then
    echo "ZIP file '$ZIP_FILENAME' created successfully in '$TARGET_DIRECTORY'."
else
    echo "Error: Failed to create ZIP file."
fi

# End of the script
