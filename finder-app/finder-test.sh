#!/bin/sh
# Tester script for assignment 1 and assignment 2
set -e
set -u



NUMFILES=10
WRITESTR=AELD_IS_FUN
WRITEDIR=/tmp/aeld-data
username=$(cat /etc/finder-app/conf/username.txt)
 
if [ $# -lt 3 ]; then
  echo "Using default value ${WRITESTR} for string to write"
  if [ $# -lt 1 ]; then
    echo "Using default value ${NUMFILES} for the number of files to write"
  else
    NUMFILES=$1
  fi
else
  NUMFILES=$1
  WRITESTR=$2
  WRITEDIR=/tmp/aeld-data/$3
fi

MATCHSTR="The number of files are ${NUMFILES} and the number of matching lines are ${NUMFILES}"

echo "Writing ${NUMFILES} files containing string ${WRITESTR} to ${WRITEDIR}"

# Remove the existing directory (if it exists)
rm -rf "${WRITEDIR}"

# create $WRITEDIR if not assignment1
assignment=$(cat /etc/finder-app/conf/assignment.txt)

if [ "$assignment" != 'assignment1' ]; then
  mkdir -p "$WRITEDIR"

  # Check if $WRITEDIR was created successfully
  if [ -d "$WRITEDIR" ]; then
    echo "$WRITEDIR created"
  else
    exit 1
  fi
fi

for i in $(seq 1 "$NUMFILES"); do
  /usr/bin/writer "$WRITEDIR/${username}$i.txt" "$WRITESTR"
done

# Execute finder.sh script using absolute path and redirect the output to a file
OUTPUTSTRING=$(/usr/bin/finder.sh "$WRITEDIR" "$WRITESTR") 
echo ${OUTPUTSTRING} > /tmp/assignment4-result.txt

# Remove temporary directories
rm -rf /tmp/aeld-data

set +e
echo "${OUTPUTSTRING}" | grep "${MATCHSTR}"
if [ $? -eq 0 ]; then
  echo "success"
  exit 0
else
  echo "failed: expected ${MATCHSTR} in ${OUTPUTSTRING} but instead found"
  exit 1
fi
