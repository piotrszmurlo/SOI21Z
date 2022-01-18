#!/bin/sh
mkdir cache
./fs -create vdisk
echo ----------------
./fs -map vdisk
echo ----------------
echo put a, b, c files
./fs -put vdisk afile
./fs -put vdisk dfile
./fs -put vdisk bfile
echo ----------------
./fs -map vdisk
echo ----------------
echo rm d
./fs -rm vdisk dfile
echo ----------------
./fs -map vdisk
echo ---------------
echo put c
./fs -put vdisk cfile
echo ----------------
./fs -map vdisk
echo ---------------
echo put d twice
./fs -put vdisk dfile
./fs -put vdisk dfile
echo ----------------
./fs -map vdisk
echo ----------------
echo compare original files with copied from vdisk:
mv afile ./cache
mv bfile ./cache
mv cfile ./cache
mv dfile ./cache
./fs -cut vdisk afile
./fs -cut vdisk bfile
./fs -cut vdisk cfile
./fs -cut vdisk dfile
cat afile
cat cache/afile
cat bfile
cat cache/bfile
cat cfile
cat cache/cfile
cat dfile
cat cache/dfile
echo ----------------
rm afile
rm bfile
rm cfile
rm dfile
mv cache/afile ./
mv cache/bfile ./
mv cache/cfile ./
mv cache/dfile ./
echo -ls before remove
./fs -ls vdisk
./fs -rm vdisk afile
./fs -rm vdisk bfile
./fs -rm vdisk cfile
./fs -rm vdisk dfile
echo ----------------
./fs -map vdisk
echo ----------------
echo -ls after all files removed
./fs -ls vdisk
echo put big file
./fs -put vdisk bigfile
echo remove vdisk
./fs -wipe vdisk
rmdir cache