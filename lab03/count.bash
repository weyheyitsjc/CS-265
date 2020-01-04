#!/bin/bash
#Author: Jacy Yu / jy529
#Date: 10/13/19
#Description: Prints the filename, # of lines, and # of words in each file in the current working directory.

for file in $(ls . ); do
	echo ${file} $(wc -l -w < $file)
done
