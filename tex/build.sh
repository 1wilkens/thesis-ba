#!/bin/bash

latex="pdflatex -interaction=batchmode"

fileExt="thesis.tex"
file="thesis"

echo "Running $latex -draftmode $fileExt"
echo ""
$latex -draftmode $fileExt
echo ""

echo "Running biber $file"
echo ""
biber $file
echo ""

echo "ReRunning $latex $fileExt twice"
echo ""
$latex $fileExt
$latex $fileExt

echo ""
echo "Cleaning up.."
echo ""

rm $file.aux &> /dev/null

# Remove temp files
rm $file.bbl &> /dev/null
rm $file.bcf &> /dev/null
rm $file.blg &> /dev/null
rm $file.lof &> /dev/null
rm $file.lot &> /dev/null
rm $file.lol &> /dev/null
rm $file.out &> /dev/null
rm $file.run* &> /dev/null
rm $file.toc &> /dev/null

# Remove .aux files of the chapters
rm -rf chapters/*.aux
rm texput.log &> /dev/null

echo "Done! Wrote output to thesis.pdf"
