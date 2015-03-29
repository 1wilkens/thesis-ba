#!/bin/bash

latex="pdflatex -interaction=batchmode"

fileExt="thesis.tex"
file="thesis"

mode=${1:-"default"}

if [ $mode == "full" ]; then
	echo "Performing a full build inluding software configuration.."
	echo ""
	uname -a > misc/uname.cfg
	lscpu > misc/lscpu.cfg
	gcc --version > misc/gcc.version
	go version > misc/go.version
	rustc --version > misc/rustc.version
	cargo --version > misc/cargo.version
fi

echo "Running '$latex -draftmode $fileExt'"
echo ""
$latex -draftmode $fileExt
echo ""

echo "Running 'biber $file'"
echo ""
biber $file
echo ""

echo "Running 'makeglossaries -q $file'"
echo ""
makeglossaries -q $file
echo ""

echo "ReRunning '$latex $fileExt' twice"
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
 
rm $file.ac* &> /dev/null
rm $file.*lg &> /dev/null
rm $file.gl* &> /dev/null
rm $file.idx &> /dev/null
rm $file.xdy &> /dev/null

# Remove .aux files of the chapters
rm -rf chapters/*.aux
rm texput.log &> /dev/null

echo "Done! Wrote output to thesis.pdf"
