#!/bin/bash

#author : AYOUB EL MAHFOUDI

#file : finder.sh

#description: Shell script finder-app/finder.sh as described below:
# Accepts the following runtime arguments: the first argument is a path to a directory on the filesystem, referred to below as filesdir; the second # argument is a text string which will be searched within these files, referred to below as searchstr
# Exits with return value 1 error and print statements if any of the parameters above were not specified
# Exits with return value 1 error and print statements if filesdir does not represent a directory on the filesystem
# Prints a message 'The number of files are X and the number of matching lines are Y where X is the number of files in the directory and all #subdirectories and Y is the number of matching lines found in respective files, where a matching line refers to a line which contains searchstr (and may also contain additional content).


NUM_PARAMS=2
filesdir="$1"
searchstr="$2"

# Check for correct number of input parameters
if [ $# -ne $NUM_PARAMS ]
then
	echo "Incorrect number of parameters. Should be $NUM_PARAMS, was $#."
	echo "The first argument is a path to a directory on the filesystem."
	echo "The second argument is a text string to search for within those files."
	echo "Exiting..."
	exit 1
fi

# Check if input directory is valid
if [ ! -d "$filesdir" ]
then
	echo "$filesdir is not a valid directory."
	echo "Exiting..."
	exit 1
fi

# Count files and count lines
total_files=0
total_lines=0
for file in $filesdir/*
do
	total_files=$((total_files+1))
	total_lines=$((total_lines + $(grep -c "$searchstr" "$file")))
done

echo "The number of files are $total_files and the number of matching lines are $total_lines"
exit 0
