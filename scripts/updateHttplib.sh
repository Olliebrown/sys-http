#!/bin/bash

# Download latest httplib.h from github
echo "Downloading httplib.h from github ..."
python3 download.py
if [ $? -eq 0 -a -f "./httplib.h.temp" ]; then
    echo "Download of httplib success"
    echo
else
    echo "update httplib failed"
    exit 1
fi

# Rename temporary file
mv ./httplib.h.temp ./httplib.h

# Patch httplib.h
echo "Patching httplib.h ..."
python3 patch.py httplib.h.patch
if [ $? -eq 0 ]; then
    echo "Patch of httplib success"
    echo
else
    echo "Patch httplib failed"
    exit 2
fi

# Split the file
sleep 1
echo "Splitting httplib.h into httplib.h and httplib.cpp ..."
python3 split.py -e "cpp"
if [ $? -eq 0 -a -f "./out/httplib.h" -a -f "./out/httplib.cpp" ]; then
    echo "Split of httplib success"
    echo
else
    echo "Split httplib failed"
    exit 3
fi

# Move files to correct location
echo "Moving split files into ../lib folder ..."
mv ./out/httplib.h ../lib/httplib.h
mv ./out/httplib.cpp ../lib/httplib.cpp

# Remove temporary files
echo "Cleaning up ..."
rm -rf ./out

echo "Done!"
