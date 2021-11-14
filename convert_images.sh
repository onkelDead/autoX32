#!/bin/bash

for FILE in `ls data/media` 
do
    NAME=`echo $FILE | cut -d '.' -f1`
    echo $NAME  
    gdk-pixbuf-csource --raw --name=${NAME}_inline data/media/${NAME}.png > src/embedded/${NAME}.h
done
