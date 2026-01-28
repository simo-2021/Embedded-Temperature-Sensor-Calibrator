#!/bin/sh
# Tester script for assignment 1 and assignment 2
# Author: Arnaud Simo
# 24.11.2025

#set -e
#set -u

##Parameters
filesdir="$1"
searchstr="$2"

## Constants
NUMFILES=10 
NUMLINES=10
 
# Check argument count
if [ $# -lt 2 ]  
then	
	echo "Error::search string and file directory are missing."
	echo "      ./finder.sh  /dir1/subdir1/  stringToSearch"
	exit 1
fi

# Check if directory exists
if [ ! -d "$filesdir" ]; then
	echo "Directory ${filesdir} does not exist"
	exit 1
fi

# Nombre de fichiers: 
NUMFILES=$(find "$filesdir" -type f | wc -l)

# Nombre de lignes contenant le texte
NUMLINES=$(grep -R "$searchstr" "$filesdir" 2>/dev/null | wc -l)

echo "The number of files are ${NUMFILES} and the number of matching lines are ${NUMLINES}"

