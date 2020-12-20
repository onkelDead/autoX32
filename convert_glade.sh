#!/bin/bash

cd data
for FILE in `ls *.glade` 
do
    NAME=`echo $FILE | cut -d '.' -f1`
    echo $NAME  
    echo "static char ${NAME}_inline_glade[] = " > ../src/embedded/${NAME}.h
    cat $FILE | sed "s/\"/\'/g" | sed 's/^/"/g' | sed 's/$/"/g' >> ../src/embedded/${NAME}.h
    echo ";" >> ../src/embedded/${NAME}.h

done
cd -
