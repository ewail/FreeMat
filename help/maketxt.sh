#!/bin/sh
# This is a script to build the text version of the help from 
# the HTML versions.  It is not integrated into the help system
# yet.  Ultimately, it should probably be replaced by some cleaner
# mechanism.
mkdir text
for file in html/*.html; do 
    echo "Processing file...$file"; 
    outfile=`echo $file | sed -e 's/html/text/'`
    outfile=`echo $outfile | sed -e 's/\/.*_/\//'`
    outfile=`echo $outfile | sed -e 's/html/mdc/'`
    html2text -style pretty -width 60 $file | tail -n +15 > $outfile
    echo "output file $outfile";
done
